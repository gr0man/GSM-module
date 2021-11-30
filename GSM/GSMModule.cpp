#include "StdAfx.h"
#include "GSMModule.h"
#include "SMS.h"
#include <tchar.h>

#include <process.h>
#include <vector>

#include "SMSTransiever.h"

#include "../../ControlData/ControlData/Logger.h"

#define SEND_TIMEOUT	10000
#define REC_TIMEOUT		60000

#ifdef FICTIVE_SEND
std::fstream sendLog("d:\\send_sms.txt", std::ios_base::out | std::ios_base::trunc);
#endif

#ifdef FICTIVE_RECEIVE
std::fstream recFile("d:\\receive_sms.txt", std::ios_base::in);
#endif

GSMModule::GSMModule(SMSTransiever& transiever)
: transiever_(transiever)
, recTimer_(INVALID_HANDLE_VALUE)
, stopEv_(CreateEvent(NULL, TRUE, FALSE, NULL))
{
	recTimer_ = CreateWaitableTimer(NULL, FALSE, NULL);
}

GSMModule::~GSMModule(void)
{
	Stop();

	while( !transQueue_.IsEmpty() )
		transQueue_.PopFront();

	CloseHandle(recTimer_);

#ifdef FICTIVE_SEND
	sendLog.close();
#endif
}

bool GSMModule::Transmit(const char* phone, const char* text, SMSPostCmd* postCmd, SMSUnsendCmd* unsendCmd)
{
	if( phone == 0 )
		return true;
	if( strlen(phone) == 0 )
		return true;
	if( text == 0 )
		return true;
	if( strlen(text) == 0 )
		return true;

	SMS* sms = new SMS(phone, text, postCmd, unsendCmd);

	transQueue_.PushBack(sms);

	return true;
}

void GSMModule::TransmitHalt(const char* phone, const char* text)
{
	if( phone == 0 )
	{
		return;
	}
	if( strlen(phone) == 0 )
	{
		return;
	}

	SMS* sms = new SMS(phone, text, SMSPostCmd::CreateStop(), 0);

	transQueue_.PushFront(sms);

	size_t cntr = 0;
	while( WaitForSingleObject(stopEv_, 200) != WAIT_OBJECT_0 )
	{
		SwitchToThread();
		if( cntr > 10 )
			Stop();
		cntr++;
	}
}

bool GSMModule::Start(const wchar_t* portName)
{
	portName_ = std::wstring(portName);

#ifndef DEVICE_EMULATION
	if( !comm_.Start(portName, CBR_115200) )
		return false;
#endif

	ResetEvent(stopEv_);

	LARGE_INTEGER li;
	li.QuadPart = -600000000;
	SetWaitableTimer(recTimer_, &li, 60000, NULL, NULL, 0);

	transThread_ = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, GSMModule::TransThread, this, 0, NULL));

	return true;
}

void GSMModule::Stop()
{
	SetEvent(stopEv_);
	CancelWaitableTimer(recTimer_);
	comm_.Stop();
}

#ifdef DEVICE_EMULATION

unsigned int WINAPI GSMModule::TransThread(void* param)
{
	GSMModule* base = reinterpret_cast<GSMModule*>(param);

	try
	{
		while( WaitForSingleObject(base->stopEv_, 100) != WAIT_OBJECT_0 )
		{
			SMS* sms = base->transQueue_.PopFront();
			if( sms )
			{
				if( base->Send(sms) )
				{
#ifdef USE_LOG
					char buffer[500];
					sprintf_s(buffer, 500, "SMS is sended: \"%s\", \"%s\"\n", sms->Address(), sms->Text());
					logger.Add("GSM", buffer);
#endif
					// отложенные действи€ после отправки sms
					// ...
					if( sms->PostCmd() )
						sms->PostCmd()->PostProcess(base->transiever_);

					delete sms;
				}
			}

			if( WaitForSingleObject(base->recTimer_, 10) == WAIT_OBJECT_0 )
			{
				base->FictiveReceive();
			}
		}
	}
	catch(...)
	{
	}

	return 0;
}

void GSMModule::FictiveReceive()
{
	static bool sending = false;

	if( !sending )
	{
		sending = true;
		
		transiever_.ProcessRecSMS("1111,10,10012349,01:30");
	}
}


#else

unsigned int WINAPI GSMModule::TransThread(void* param)
{
	GSMModule* base = reinterpret_cast<GSMModule*>(param);

	try
	{
#ifdef USE_LOG
		logger.Add("GSMModule", "StartThread");
#endif
	
		while( WaitForSingleObject(base->stopEv_, 100) != WAIT_OBJECT_0 )
		{
			// 1. извлекаем SMS из очереди
			SMS* sms = base->transQueue_.PopFront();
			// 2. пытаемс€ отслать его
			
			if( sms )
			{
				if( base->Send(sms) )
				{
					// отложенные действи€ после отправки sms
					// ...
					if( sms->PostCmd() )
						sms->PostCmd()->PostProcess(base->transiever_);

					delete sms;
				}
				else
				{
#ifdef USE_LOG
					char buffer[500];
					sprintf_s(buffer, 500, "SMS send fail: \"%s\", \"%s\"\n", sms->Address(), sms->Text());
					logger.Add("GSM", buffer);
#endif

					// отправл€ем в конец очереди, чтобы отправить позже
					base->transQueue_.PushBack(sms);
				}

				base->transQueue_.SincSave();
			}
			

			if( WaitForSingleObject(base->recTimer_, 10) == WAIT_OBJECT_0 )
			{
				base->TryReceive();
			}
		}
	}
	catch(DWORD err)
	{
#ifdef USE_LOG
		char buffer[500];
		sprintf_s(buffer, 500, "EXIT on error: %d", err);
		logger.Add("GSMModule", buffer);
#endif
		if( err == 5 )
			base->transiever_.ErrorMessage("GSMModem port error");
	}
	catch(...)
	{
	}

#ifdef USE_LOG
	logger.Add("GSMModule", "EXIT");
#endif

	return 0;
}
#endif

void GSMModule::RemoveSMS(DWORD key)
{
	transQueue_.RemoveByKey(key);
}

bool GSMModule::TryReadSendSmsCmdReq()
{
	// правильный ответ - строка "\r\n> " (без завершающего '\0').

	char reqBuffer[100];
	size_t i;
	char cmpStr[] = "\r\n> ";
	for(i=0; i<4; ++i)
	{
		char bt = 0;
		bool stop = false;
		DWORD errCode = 0;
		if( !comm_.ReadByte(&bt, stopEv_, stop, 10000, errCode) )
		{
			if( stop )
				throw errCode;
		}
		reqBuffer[i] = bt;
	}
	reqBuffer[i] = 0;

	if( strcmp(cmpStr, reqBuffer) == 0 )
		return true;
	else
	{
#ifdef USE_LOG
		char buffer[500];
		sprintf_s(buffer, 500, "read send sms cmd req: \"%s\"", reqBuffer);
		logger.Add("GSM", buffer);
#endif
		if( strcmp("+CMS", reqBuffer) == 0 )
		{
			for(i=4; i<99; ++i)
			{
				char bt = 0;
				bool stop = false;
				DWORD errCode = 0;
				if( !comm_.ReadByte(&bt, stopEv_, stop, 10000, errCode) )
				{
					if( stop )
						throw errCode;
					else
					{
						i++;
						break;
					}
				}
				reqBuffer[i] = bt;
			}
			reqBuffer[i] = 0;

			if( strcmp("+CMS ERROR: 301", reqBuffer) == 0 )
			{
				// ничего не делаем
			}
		}

		return false;
	}
}

bool GSMModule::TryReadSendSmsReq()
{
	// ответ - "\r\n+CMGS: n\r\n" или "\r\n+CMS ERROR: n\r\n"
	size_t i=0;
	char okStr[] = "\r\nOK\r\n";
	char rightStr[] = "\r\n+CMGS: ";
	char errStr[] = "\r\n+CMS ERROR: ";
	char readStr[1000];

	char bt = 0;
	bool stop = false;
	DWORD errCode = 0;
	size_t errcntr = 0;
	do
	{
		bt = 0;

		if( comm_.ReadByte(&bt, stopEv_, stop, 60000, errCode) )
		{
			readStr[i++] = bt;
		}
		else
			errcntr++;

		if( stop )
			throw errCode;

		if( i>5 )
		{
			if( (readStr[i-2] == '\r') && (readStr[i-1] == '\n') )
				break;
		}
	}
	while( (i < 999) && (errcntr < 1) );

	readStr[i] = 0;
	if( strncmp(rightStr, readStr, strlen(rightStr)) == 0 )
	{
		// считываем OK
		for(i=0; i<strlen(okStr); ++i)
		{
			if( !comm_.ReadByte(&bt, stopEv_, stop, 10000, errCode) )
			{
				if( stop )
					throw errCode;

				return true;
			}

			if( bt != okStr[i] )
			{
				return true;
			}
		}

		return true;
	}
	else if( strncmp(errStr, readStr, strlen(errStr)) == 0 )
	{
		// анализ кода ошибки
		int err = atoi(readStr+strlen(errStr));

#ifdef USE_LOG
		char buffer[500];
		sprintf_s(buffer, 500, "modem request sms1: %s", readStr);
		logger.Add("GSM", buffer);
#endif
		if( err == 28 )
		{
			// закончились деньги на счете или абонент вне сети
		}

		transiever_.ErrorMessage(readStr);

		// удалить неудачно отправленныю SMS
		DeleteUnsended();

		Sleep(1000);

		return false;
		//return true;
	}

	return false;
}

bool GSMModule::Send(SMS* sms)
{
#ifdef FICTIVE_SEND
	char buffer[500];
	time_t crTime;
	time(&crTime);
	struct tm crTm;
	localtime_s(&crTm, &crTime);
	sprintf_s(buffer, 500, "send \"%s\" to \"%s\" at %02d:%02d:%02d",
		sms->Text(), sms->Address(), crTm.tm_hour, crTm.tm_min, crTm.tm_sec);
	sendLog << buffer << std::endl;

	return true;
#endif

	// проверка актуальности
	if( sms->UnsendCmd() )
	{
		time_t crTime;
		time(&crTime);
		if( crTime > sms->UnsendCmd()->LastSendTime() )
		{
#ifdef USE_LOG
			char buffer[500];
			sprintf_s(buffer, 500, "SMS is not send (last send time out): \"%s\", \"%s\"\n", sms->Address(), sms->Text());
			logger.Add("GSM", buffer);
#endif
			sms->UnsendCmd()->Execute(transiever_);
			sms->DeletePostCmd();

			return true;
		}
	}

	int failCode = 0;

#ifdef DEVICE_EMULATION
	return true;
#endif

	// посылка команды
	size_t len = strlen(sms->Data());
	char cmd[100];
	sprintf_s(cmd, 100, "AT+CMGS=%d\r\n", (len-2)/2);

	const char endCmd[3] = { 0x1A, '\r', '\n' };

	bool stop = false;
	DWORD errCode = 0;
	if( !comm_.Write(cmd, strlen(cmd), stopEv_, stop, 10000, errCode) )
	{
		failCode = 1;
		if( stop )
			throw errCode;
	}
	else
	if( !TryReadSendSmsCmdReq() )
	{
		failCode = 2;
	}
	else
	if( !comm_.Write(sms->Data(), strlen(sms->Data()), stopEv_, stop, 10000, errCode) )
	{
		failCode = 3;
		if( stop )
			throw errCode;
	}
	else
	if( !comm_.Write(endCmd, 3, stopEv_, stop, 10000, errCode) )
	{
		failCode = 4;
		if( stop )
			throw errCode;
	}
	else
	// чтение ответа модема
	if( !TryReadSendSmsReq() )
	{
		failCode = 5;
	}

	Sleep(20);

	if( failCode != 0 )
	{
#ifdef USE_LOG
		char buffer[500];
		sprintf_s(buffer, 500, "SMS send failCode = %d\n", failCode);
		logger.Add("GSM", buffer);
#endif
		return false;
	}

#ifdef USE_LOG
	char buffer[500];
	sprintf_s(buffer, 500, "SMS is sended: \"%s\", \"%s\"\n", sms->Address(), sms->Text());
	logger.Add("GSM", buffer);
#endif

	return true;
}

void GSMModule::TryReadUnsendSmsReq()
{
	// провер€ем список новых сообщений
	
	char okStr[] = "OK\r\n";
	char reqStr[] = "+CMGL: ";
	char readStr[1000];
	size_t i;
	std::vector<int> idxs;

	char bt = 0;
	
	while(true)
	{
		bool stop = false;
		DWORD errCode = 0;
		size_t errcntr = 0;
		i=0;
		do
		{
			bt = 0;

			if( comm_.ReadByte(&bt, stopEv_, stop, 60000, errCode) )
			{
				if( i == 0 && (bt == '\r' || bt == '\n') )
				{
				}
				else
					readStr[i++] = bt;
			}
			else
				errcntr++;

			if( stop )
				throw errCode;

			if( i>3 )
			{
				if( (readStr[i-2] == '\r') && (readStr[i-1] == '\n') )
					break;
			}

		}
		while( (i < 999) && (errcntr < 1) );

		readStr[i] = 0;
		if( strncmp(okStr, readStr, strlen(okStr)) == 0 )
		{
			// удаление полученных строк
			std::vector<int>::const_reverse_iterator riter;
			for(riter=idxs.rbegin(); riter!=idxs.rend(); ++riter)
			{
				DeleteSMS(*riter);
			}

			return;
		}

		if( strncmp(reqStr, readStr, strlen(okStr)) == 0 )
		{
			// парсинг строки дл€ получени€ индекса нового сообщени€
			size_t idx = 0;
			idx = atoi(readStr + strlen(reqStr));
			idxs.push_back(idx);


			// чтение сообщени€
			for(i=0, bt=0; bt!='\n'; i++)
			{
				bool stop = false;
				DWORD errCode = 0;
				if( !comm_.ReadByte(&bt, stopEv_, stop, 1000, errCode) )
				{
					if( stop )
						throw errCode;

					return;
				}
				readStr[i] = bt;
			}

			readStr[i-2] = 0;
		}
	}
}

void GSMModule::DeleteUnsended()
{
	char cmd[] = "AT+CMGL=2\r\n";

	bool stop = false;
	DWORD errCode = 0;
	if( !comm_.Write(cmd, strlen(cmd), stopEv_, stop, 1000, errCode) )
	{
		if( stop )
			throw errCode;
		return;
	}

	TryReadUnsendSmsReq();
}

void GSMModule::TryReadRecSmsReq()
{
	// провер€ем список новых сообщений
	
	char okStr[] = "OK\r\n";
	char reqStr[] = "+CMGL: ";
	char readStr[1000];
	size_t i;
	std::vector<int> idxs;

	char bt = 0;
	
	while(true)
	{

		bool stop = false;
		DWORD errCode = 0;
		size_t errcntr = 0;
		i=0;
		do
		{
			bt = 0;

			if( comm_.ReadByte(&bt, stopEv_, stop, 60000, errCode) )
			{
				if( i == 0 && (bt == '\r' || bt == '\n') )
				{
				}
				else
					readStr[i++] = bt;
			}
			else
				errcntr++;

			if( stop )
				throw errCode;

			if( i>3 )
			{
				if( (readStr[i-2] == '\r') && (readStr[i-1] == '\n') )
					break;
			}

		}
		while( (i < 999) && (errcntr < 1) );

		readStr[i] = 0;

		if( strncmp(okStr, readStr, strlen(okStr)) == 0 )
		{
			// удаление полученных сообщений - начина€ с последнего
			std::vector<int>::const_reverse_iterator riter;
			for(riter=idxs.rbegin(); riter!=idxs.rend(); ++riter)
			{
				DeleteSMS(*riter);
			}

			return;
		}

		if( strncmp(reqStr, readStr, strlen(okStr)) == 0 )
		{
			// парсинг строки дл€ получени€ индекса нового сообщени€
			size_t idx = 0;
			idx = atoi(readStr + strlen(reqStr));
			idxs.push_back(idx);


			// чтение сообщени€
			for(i=0, bt=0; bt!='\n'; i++)
			{
				bool stop = false;
				DWORD errCode = 0;
				if( !comm_.ReadByte(&bt, stopEv_, stop, 10000, errCode) )
				{
					if( stop )
						throw errCode;

					return;
				}

				readStr[i] = bt;
			}

			readStr[i-2] = 0;

			SMS sms(readStr);
			transiever_.ProcessRecSMS(sms.Text());
		}
	}
}

void GSMModule::TryReceive()
{
#ifdef FICTIVE_RECEIVE
	// читать из файла список SMS и и отдавать на обработку
	std::string str;
	recFile >> str;
	transiever_.ProcessRecSMS(str.c_str());
	return;
#endif	
	
	char cmd[] = "AT+CMGL=4\r\n";

	bool stop = false;
	DWORD errCode = 0;
	int len = strlen(cmd);
	if( !comm_.Write(cmd, len, stopEv_, stop, 10000, errCode) )
	{
		if( stop )
			throw errCode;

		return;
	}

	TryReadRecSmsReq();
}

void GSMModule::DeleteSMS(size_t index)
{
	char cmd[100];
	sprintf_s(cmd, 100, "AT+CMGD=%d\r\n", index);

	bool stop = false;
	DWORD errCode = 0;
	if( !comm_.Write(cmd, strlen(cmd), stopEv_, stop, 10000, errCode) )
	{
		if( stop )
			throw errCode;
		return;
	}

	size_t i;
	char cmpStr[] = "\r\nOK\r\n";
	for(i=0; i<strlen(cmpStr); ++i)
	{
		char bt = 0;
		if( !comm_.ReadByte(&bt, stopEv_, stop, 10000, errCode) )
		{
			if( stop )
				throw errCode;

			return;
		}

		if( bt != cmpStr[i] )
			return;
	}
}

///////////////////////////////////////////////////////
// небольшиой функциональный тест.

#include <iostream>

class TestTransiever : public SMSTransiever
{
public:

	TestTransiever()
	{
		recLog_.open("D:\\projects\\AccessControl_projects\\rec_sms.txt", std::ios_base::out | std::ios_base::trunc);
	}

	virtual ~TestTransiever()
	{
		recLog_.close();
	}

	virtual void ProcessRecSMS(const char* text)
	{
		std::cout << "rec SMS: " << text << std::endl;
		recLog_ << "rec SMS: \"" << text << "\"" << std::endl;
	};

	virtual bool SendSMS(SMSCommand_0* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_1* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_2* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_3* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_4* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_5* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_6* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_7* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_8* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_9* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_10* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_11* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_12* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_13* cmd){ return true; }
	virtual bool SendSMS(SMSCommand_16* cmd){ return true; }

	virtual void PostSMS(SMSPostCmd_0* cmd){};
	virtual void PostSMS(SMSPostCmd_1* cmd){};
	virtual void PostSMS(SMSPostCmd_2* cmd){};
	virtual void PostSMS(SMSPostCmd_6* cmd){};
	virtual void PostSMS(SMSPostCmd_7* cmd){};
	virtual void PostSMS(SMSPostCmd_N* cmd){};
	virtual void PostSMS(SMSPostCmd_STOP* cmd){};

	virtual void UnsendCmd(SMSUnsendCmd* cmd){};

	virtual void ErrorMessage(const char* msg){ std::cout << "error: " << msg << std::endl; }

	bool IsSendEnable(DWORD key){ return true; }
private:
	std::fstream recLog_;
};

class TestPostCmd : public SMSPostCmd
{
public:
	TestPostCmd(const char* text)
		: text_(text)
	{
	}

	void PostProcess(SMSTransiever&)
	{
		std::cout << "sending: " << text_ << std::endl;
	}

private:
	std::string text_;
};

#include <iostream>

void GSMModule_test()
{
	TestTransiever trans;
	GSMModule gsm(trans);
	if( !gsm.Start(_T("COM7")))
	{
		std::cout << "error start GSM" << std::endl;
	}
	else
	{
		for(size_t i=0; i<10; ++i)
		{
			Sleep(6000);
			//gsm.Transmit("9036674551", "“естова€ SMS 1", new TestPostCmd("“естова€ SMS 1"));
			Sleep(60000);
			//gsm.Transmit("9036674551", "“естова€ SMS 2", new TestPostCmd("“естова€ SMS 2"));
			Sleep(60000);
			//gsm.Transmit("9036674551", "“естова€ SMS 3", new TestPostCmd("“естова€ SMS 3"));
			Sleep(60000);
			//gsm.Transmit("9036674551", "“естова€ SMS 4", new TestPostCmd("“естова€ SMS 4"));
			Sleep(60000);
			//gsm.Transmit("9036674551", "“естова€ SMS 5", new TestPostCmd("“естова€ SMS 5"));
			Sleep(60000);
		}
	}

	Sleep(120000);
	gsm.Stop();
}