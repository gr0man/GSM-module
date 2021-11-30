#include "StdAfx.h"
#include "SMSCommand.h"
#include "SMSCommandImpl.h"

#include "../../ControlData/ControlData/Logger.h"

#define INP_SMS_MIN_LEN		6
#define INP_SMS_CODE_POS	5

BYTE const SMSCommand::inpSmsLen[17] = 
{	0, 0, 0, 6, 15, 5, 15, 15, 26, 15, 22, 16, 0, 0, 0, 0, 6
};

bool VerifySmsKey(DWORD key)
{
	if( key>=1 && key<=99999999 )
		return true;
	else 
		return false;
}

bool VerifySmsPotential(unsigned int potential)
{
	if( potential>=1 && potential<=10000 )
		return true;
	else
		return false;
}

bool VerifySmsPhone(const char* phone)
{
	if( strlen(phone) == 10 )
	{
		for(size_t i=0; i<10; ++i)
			if( isdigit(phone[i]) == 0 )
				return false;

		return true;
	}
	else
		return false;
}

SMSCommand* SMSCommand::Create(const char* text, WORD passSmsParam, const char* addText)
{
	SMSCommand* cmd = 0;
	
	size_t len = strlen(text);
	if( len < INP_SMS_MIN_LEN )
		return cmd;

	if( text[4] != ',' )
		return cmd;

	if( isdigit(text[5]) == 0 )
		return cmd;

	// извлечение кода доступа
	WORD passSms = atoi(text);
	if( passSms != passSmsParam )
		return cmd;

	time_t crTime;
	time(&crTime);

	// код доступа валиден, продолжение разбора
	BYTE code = atoi(text+INP_SMS_CODE_POS);
	if( code <= 16 && len >= inpSmsLen[code] )
	{
		switch(code)
		{
		case 3:
			cmd = new SMSCommand_3(crTime, addText ? addText : "запрос");
			break;
		case 4:
			{
				DWORD key = atoi(text+4+3);
				unsigned int potential = atoi(text+4+3+9);
				if( VerifySmsKey(key) && VerifySmsPotential(potential) )
					cmd = new SMSCommand_4(crTime, key, potential);
			}
			break;
		case 6:
			{
				DWORD key = atoi(text+4+3);
				if( VerifySmsKey(key) )
					cmd = new SMSCommand_6(crTime, key);
			}
			break;
		case 7:
			{
				DWORD key = atoi(text+4+3);
				if( VerifySmsKey(key) )
					cmd = new SMSCommand_7(crTime, key);
			}
			break;
		case 8:
			{
				DWORD key = atoi(text+4+3);
				char phone[100];
				strcpy_s(phone, 100, text+4+3+9);
				if( VerifySmsKey(key) && VerifySmsPhone(phone) )
					cmd = new SMSCommand_8(crTime, key, phone);
			}
			break;
		case 9:
			{
				DWORD key = atoi(text+4+3);
				if( VerifySmsKey(key) )
					cmd = new SMSCommand_9(crTime, key);
			}
			break;
		case 10:
			{
				DWORD key = atoi(text+4+4);
				//struct tm urgTimeTm;
				//localtime_s(&urgTimeTm, &crTime);
				unsigned int hour = atoi(text+4+4+9);
				unsigned int min = atoi(text+4+4+9+3);
				//urgTimeTm.tm_hour = hour;
				//urgTimeTm.tm_min = min;
				time_t urgTime = hour*60*60 + min*60;//mktime(&urgTimeTm);

				if( VerifySmsKey(key) && hour<24 && min<60 )
					cmd = new SMSCommand_10(crTime, key, urgTime);
			}
			break;
		case 11:
			{
				char phone[100];
				strcpy_s(phone, 100, text+4+4);
				if( VerifySmsPhone(phone) )
					cmd = new SMSCommand_11(crTime, phone);
			}
			break;
		case 16:
			{
				cmd = new SMSCommand_16();
			}
			break;
		}
	}

	if( cmd )
		return cmd;
	
	// формирование сообщения об ошибке команды
	char newText[100];
	sprintf_s(newText, 100, "%s?", text+5);
	cmd = new SMSCommand_12(crTime, newText);

#ifdef USE_LOG
	if( cmd == 0 )
	{
		char buffer[500];
		sprintf_s(buffer, 500, "new SMSCommand error: %s, %d",
			text, passSmsParam);
		logger.Add("SMSCommand::Create", buffer);
	}
#endif

	return cmd;
}

SMSCommand* SMSCommand::Create(BYTE code, time_t smsTime, DWORD key, BYTE flag)
{
	SMSCommand* cmd ;

	switch( code )
	{
	case 0:
		cmd = new SMSCommand_0(smsTime, key, flag);
		break;
	case 1:
		cmd = new SMSCommand_1(smsTime, key, flag);
		break;
	}

#ifdef USE_LOG
	if( cmd == 0 )
	{
		char buffer[500];
		sprintf_s(buffer, 500, "new SMSCommand error: %d, %u, %d, %d",
			code, smsTime, key, flag);
		logger.Add("SMSCommand::Create", buffer);
	}
#endif

	return cmd;
}

SMSCommand* SMSCommand::Create(BYTE code, DWORD key, const char* phone)
{
	SMSCommand* cmd = 0;

	if( code == 2 )
	{
		cmd = new SMSCommand_2(key, phone);
	}

#ifdef USE_LOG
	if( cmd == 0 )
	{
		char buffer[500];
		sprintf_s(buffer, 500, "new SMSCommand error: %d, %d, %s",
			code, key, phone);
		logger.Add("SMSCommand::Create", buffer);
	}
#endif

	return cmd;
}

SMSCommand* SMSCommand::Create(BYTE code, time_t smsTime, DWORD key, const char* phone, unsigned int potential)
{
	SMSCommand* cmd = 0;

	if( code == 5 )
	{
		cmd = new SMSCommand_5(smsTime, key, phone, potential);
	}

#ifdef USE_LOG
	if( cmd == 0 )
	{
		char buffer[500];
		sprintf_s(buffer, 500, "new SMSCommand error: %d, %u, %d, %s, %u",
			code, smsTime, key, phone, potential);
		logger.Add("SMSCommand::Create", buffer);
	}
#endif

	return cmd;
}

SMSCommand* SMSCommand::Create(BYTE code, const char* phone)
{
	SMSCommand* cmd = 0;

	if( code == 13 )
	{
		cmd = new SMSCommand_13(phone);
	}

#ifdef USE_LOG
	if( cmd == 0 )
	{
		char buffer[500];
		sprintf_s(buffer, 500, "new SMSCommand error: %d, %s",
			code, phone);
		logger.Add("SMSCommand::Create", buffer);
	}
#endif

	return cmd;
}

SMSCommand::SMSCommand(time_t smsTime, DWORD key)
: smsTime_(smsTime)
, key_(key)
, sendTime_(smsTime)
{
	char buffer[20];

	struct tm smsTimeTm;
	localtime_s(&smsTimeTm, &smsTime_);

	sprintf_s(buffer, 20, "%02d.%02d.%02d",
		smsTimeTm.tm_mday, smsTimeTm.tm_mon+1, smsTimeTm.tm_year-100);
	dateStr_ = std::string(buffer);

	sprintf_s(buffer, 20, "%02d:%02d", smsTimeTm.tm_hour, smsTimeTm.tm_min);
	timeStr_ = std::string(buffer);
}



SMSCommand::~SMSCommand(void)
{
}

time_t SMSCommand::SmsTime() const
{
	return smsTime_;
}

time_t SMSCommand::SendTime() const
{
	return sendTime_;
}