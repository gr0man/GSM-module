#include "StdAfx.h"
#include "SMSProcessor.h"
#include "SMSCommandImpl.h"

#include "SMSPostCmd.h"
#include "SMSPostCmdImpl.h"

#include <process.h>

#include "..\..\ControlData\ControlData\ControlDB.h"
#include "..\..\ControlData\ControlData\ControlParams.h"

#include "../../ControlData/ControlData/Logger.h"

#include "SMSLogic.h"

//#include <boost/bind.hpp>

SMSProcessor::SMSProcessor(SMSLogic& logic)
: logic_(logic)
, stopEv_(CreateEvent(NULL, TRUE, FALSE, NULL))
, gsmModem_(*this)
, thread_(NULL)
{
	SetEvent(stopEv_);
}

SMSProcessor::~SMSProcessor(void)
{
}

void SMSProcessor::AddSMS1(BYTE code, time_t crTime, DWORD key, BYTE flag)
{
	SMSCommand* sms = SMSCommand::Create(code, crTime, key, flag);
	if( sms )
	{
		queue1_.PushBack(sms);
		queue1_.SincSave();
	}
}

void SMSProcessor::AddSMS2(BYTE code, const char* phone)
{
	SMSCommand* sms = SMSCommand::Create(code, phone);
	if( sms )
	{
		queue2_.PushBack(sms);
		queue2_.SincSave();
	}
}

void SMSProcessor::AddStartSMS(const char* text)
{
	SMSCommand* sms = SMSCommand::Create(text, logic_.GetPassSMS(), "start");
	if( sms )
	{
		queue1_.PushBack(sms);
		queue1_.SincSave();
	}
}

bool SMSProcessor::ProcessSendQueue1()
{
	SMSCommand* cmd = queue1_.PopFront();
	if( cmd )
	{
		if( cmd->Execute(*this) )
		{
			delete cmd;
		}
		else
		{
			SetErrorSMS(cmd);
			queue1_.PushBack(cmd);
		}
		return true;
	}
	return false;
}

void SMSProcessor::SetErrorSMS(SMSCommand* cmd)
{
	logic_.ErrorSMS();
}

void SMSProcessor::ProcessSendQueue2()
{
	// обработка второй очереди только если первая пуста
	if( queue1_.IsEmpty() )
	{
		SMSCommand* cmd = queue2_.PopFront();
		if( cmd )
		{
			if( cmd->Execute(*this) )
			{
				delete cmd;
			}
			else
			{
				SetErrorSMS(cmd);
				queue2_.PushBack(cmd);
			}
		}
	}
}

void SMSProcessor::ProcessSendWait()
{
	time_t crTime;
	time(&crTime);	
	
	SMSCommand* cmds[1000];
	size_t len = queueWait_.PopEarly(crTime, cmds, 1000);
	size_t i;
	for(i=0; (i<len) && (i<1000); i++)
	{
		cmds[i]->SendTime(crTime);
		queue1_.PushBack(cmds[i]);
	}
}

unsigned int WINAPI SMSProcessor::WorkThread(void* param)
//void SMSProcessor::WorkThread()
{
	SMSProcessor* base = reinterpret_cast<SMSProcessor*>(param);
	
	try
	{
#ifdef USE_LOG
		logger.Add("SMSProcessor", "EXIT");
#endif		
		
		// цикл write/read
		HANDLE timer = CreateWaitableTimer(NULL, FALSE, NULL);

		LARGE_INTEGER li;
		li.QuadPart = 60*10000000;
		SetWaitableTimer(timer, &li, 60*1000, NULL, NULL, FALSE);

		HANDLE ev[] = { base->stopEv_, timer };

		while( true )
		{
			if( !base->ProcessSendQueue1())
			{
				base->ProcessSendQueue2();
			}

			DWORD ret = WaitForMultipleObjects(2, ev, FALSE, 10);
			if( ret == WAIT_OBJECT_0 )
				break;
			else if( ret == (WAIT_OBJECT_0+1) )
			{
				base->ProcessSendWait();
			}

			base->queue1_.SincSave();
			base->queue2_.SincSave();
			base->queueWait_.SincSave();

		}

		CloseHandle(timer);
	}
	catch(...)
	{
		return 100;
	}

#ifdef USE_LOG
	logger.Add("SMSProcessor", "EXIT");
#endif

	return 0;
}

bool SMSProcessor::Start(const wchar_t* portname)
{
	//if( WaitForSingleObject(stopEv_, 0) == WAIT_OBJECT_0 )
	{
		if( !gsmModem_.Start(portname) )
			return false;

		ResetEvent(stopEv_);
		//thread_ = boost::thread( boost::bind(&SMSProcessor::WorkThread, this) );
		if( thread_ == NULL )
			thread_ = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, SMSProcessor::WorkThread, this, 0, NULL));
	}

#ifdef DEVICE_EMULATION
	//MessageBoxA(NULL, logic_.GetLastTime(10012349), "GetLastTime(10012349)", MB_OK);
	ProcessRecSMS("1111,4,10012349,1000");
#endif

	return true;
}

void SMSProcessor::Stop()
{
	//stop_ = true;
	SetEvent(stopEv_);
	gsmModem_.Stop();
	// ждем
	SwitchToThread();
	WaitForSingleObject(thread_, 500);
	CloseHandle(thread_);
	thread_ = NULL;
	//Sleep(500);
}

void SMSProcessor::ProcessRecSMS(const char* text)
{
	SMSCommand* newCmd = SMSCommand::Create(text, logic_.GetPassSMS());
	if( newCmd )
	{
		queue1_.PushBack(newCmd);
		queue1_.SincSave();
	}
}

void SMSProcessor::DateTimeString(time_t inTime, char strDate[10], char strTime[10])
{
	struct tm inTimeTm;
	localtime_s(&inTimeTm, &inTime);
	sprintf_s(strDate, 10, "%02d.%02d.%02d", inTimeTm.tm_mday, inTimeTm.tm_mon+1, inTimeTm.tm_year-100);
	sprintf_s(strTime, 10, "%02d:%02d", inTimeTm.tm_hour, inTimeTm.tm_min);
}

bool SMSProcessor::SendSMS(SMSCommand_0* cmd)
{
	SMSCommand* findCmds[1000];
	size_t len = queueWait_.Find(cmd->Key(), 0, findCmds, 1000);
	size_t i;
	for(i=0; i<len; ++i)
	{
		reinterpret_cast<SMSCommand_0*>(findCmds[i])->SetSmsTime(cmd->SmsTime());
	}
	
	if( queueWait_.Remove(cmd->Key(), 1) )
	{
		return true;
	}

	if( len > 0 )
		return true;

	time_t tmpTime = 0;
	len = queue1_.Find(cmd->Key(), 0, findCmds, 1000);
	len += queue1_.Find(cmd->Key(), 1, findCmds+len, 1000-len);
	for(i=0; i<len; ++i)
	{
		if( findCmds[i]->SmsTime() > tmpTime )
			tmpTime = findCmds[i]->SmsTime();
	}

	if( len )
	{
		if( (cmd->SendTime()-tmpTime) > logic_.GetTSMS() )
		{
			SMSCommand_0* newCmd(new SMSCommand_0(*cmd));
			queueWait_.Push(newCmd, tmpTime+logic_.GetTSMS());

			return true;
		}
	}
	else
	{
		tmpTime = logic_.GetLastTimeFromControl(cmd->Key());

		if( ((int)cmd->SendTime()-(int)tmpTime) < (int)logic_.GetTSMS() )
		{
			SMSCommand_0* newCmd(new SMSCommand_0(*cmd));
			queueWait_.Push(newCmd, (int)tmpTime+(int)logic_.GetTSMS());

			return true;
		}
	}


	// п.3
	time_t act;
	time_t crTime;
	time(&crTime);
	act = crTime - cmd->SmsTime();
	int urgency = int(logic_.GetUrgency(cmd->Key()));

	SMSUnsendCmd* unsendCmd = new SMSUnsendCmd(0, cmd->Key(), cmd->SmsTime(), urgency);

	if( act > urgency )
	{
		unsendCmd->Execute(*this);
		delete unsendCmd;

		return true;
	}

	// 4.
	time_t ct1;
	time_t ct2;
	logic_.GetCTs(cmd->Key(), &ct1, &ct2);
	struct tm ct1Tm;
	struct tm ct2Tm;

	ct1Tm.tm_hour = static_cast<int>(ct1/(60*60));
	ct1Tm.tm_min = static_cast<int>((ct1/60)%60);
	ct1Tm.tm_sec = static_cast<int>(ct1%60);

	ct2Tm.tm_hour = static_cast<int>(ct2/(60*60));
	ct2Tm.tm_min = static_cast<int>((ct2/60)%60);
	ct2Tm.tm_sec = static_cast<int>(ct2%60);

	int p = logic_.GetPotential(cmd->Key()) - logic_.GetSMSpr();

	char name[200];
	strcpy_s(name, logic_.GetName(cmd->Key()));
	
	char buffer[200];
	sprintf_s(buffer, 200, "%s %s %s %s %s %02d:%02d-%02d:%02d ост.%d",
		cmd->DateString(), name,
		cmd->Flag()==0 ? "Вход:" : "Вход(без карты!):",
		cmd->TimeString(), logic_.GetText1(), ct1Tm.tm_hour, ct1Tm.tm_min, ct2Tm.tm_hour, ct2Tm.tm_min, p);
	size_t smsLen = strlen(buffer);
	if( smsLen > 70 )
	{
		size_t diff = smsLen-70;
		size_t oldLen = strlen(name);
		name[oldLen-diff] = 0;
		sprintf_s(buffer, 200, "%s %s %s %s %s %02d:%02d-%02d:%02d ост.%d",
			cmd->DateString(), name,
			cmd->Flag()==0 ? "Вход:" : "Вход(без карты!):",
			cmd->TimeString(), logic_.GetText1(), ct1Tm.tm_hour, ct1Tm.tm_min, ct2Tm.tm_hour, ct2Tm.tm_min, p);
	}

	SMSPostCmd* postCmd = SMSPostCmd::Create(cmd->Key(), cmd->SmsTime(), p, 0);

	if( gsmModem_.Transmit(logic_.GetPhone(cmd->Key()), buffer, postCmd, unsendCmd) )
	{
		return true;
	}

	return false;
}

bool SMSProcessor::SendSMS(SMSCommand_1* cmd)
{
	SMSCommand* findCmds[1000];
    size_t len = queueWait_.Find(cmd->Key(), 1, findCmds, 1000);
    size_t i;
    for(i=0; i<len; ++i)
    {
        reinterpret_cast<SMSCommand_0*>(findCmds[i])->SetSmsTime(cmd->SmsTime());
    }
    
    if( queueWait_.Remove(cmd->Key(), 0) )
	{
		return true;
	}

	if( len )
		return true;


    time_t tmpTime = 0;
    len = queue1_.Find(cmd->Key(), 0, findCmds, 1000);
    len += queue1_.Find(cmd->Key(), 1, findCmds+len, 1000-len);
    for(i=0; i<len; ++i)
    {
		if( findCmds[i]->SmsTime() > tmpTime )
			tmpTime = findCmds[i]->SmsTime();
    }

    if( len )
    {
        if( (cmd->SendTime()-tmpTime) < logic_.GetTSMS() )
        {
			SMSCommand_1* newCmd(new SMSCommand_1(*cmd));
            queueWait_.Push(newCmd, tmpTime+logic_.GetTSMS());
   
			return true;
        }
	}
    else
    {
		tmpTime = logic_.GetLastTimeFromControl(cmd->Key());

		if( ((int)cmd->SendTime()-(int)tmpTime) < (int)logic_.GetTSMS() )
		{
			SMSCommand_1* newCmd(new SMSCommand_1(*cmd));
			queueWait_.Push(newCmd, (int)tmpTime+(int)logic_.GetTSMS());

			return true;
		}
    }

    // п.3
    time_t crTime;
    time(&crTime);
    time_t act = crTime - cmd->SmsTime();
    int urgency = int(logic_.GetUrgency(cmd->Key()));

	SMSUnsendCmd* unsendCmd = new SMSUnsendCmd(0, cmd->Key(), cmd->SmsTime(), urgency);

    if( act > urgency )
    {
		unsendCmd->Execute(*this);
		delete unsendCmd;

        return true;
    }

    time_t ct1;
    time_t ct2;
    logic_.GetCTs(cmd->Key(), &ct1, &ct2);

    struct tm ct1Tm;
    //localtime_s(&ct1Tm, &ct1);
	ct1Tm.tm_hour = static_cast<int>(ct1/(60*60));
	ct1Tm.tm_min = static_cast<int>((ct1/60)%60);
	ct1Tm.tm_sec = static_cast<int>(ct1%60);

    struct tm ct2Tm;
    //localtime_s(&ct2Tm, &ct2);
	ct2Tm.tm_hour = static_cast<int>(ct2/(60*60));
	ct2Tm.tm_min = static_cast<int>((ct2/60)%60);
	ct2Tm.tm_sec = static_cast<int>(ct2%60);

	char name[200];
	strcpy_s(name, logic_.GetName(cmd->Key()));
    
	int p = logic_.GetPotential(cmd->Key()) - logic_.GetSMSpr();
    
	char buffer[200];
    sprintf_s(buffer, 200, "%s %s %s%s %s %02d:%02d-%02d:%02d ост.%d",
		cmd->DateString(), name, cmd->Flag() == 0 ? "Выход:" : "Выход(без карты)!:",
		cmd->TimeString(), logic_.GetText1(),
        ct1Tm.tm_hour, ct1Tm.tm_min, ct2Tm.tm_hour, ct2Tm.tm_min, p);

	size_t smsLen = strlen(buffer);
	if( smsLen > 70 )
	{
		size_t diff = smsLen-70;
		size_t oldLen = strlen(name);
		name[oldLen-diff] = 0;

		sprintf_s(buffer, 200, "%s %s %s%s %s %02d:%02d-%02d:%02d ост.%d",
			cmd->DateString(), name, cmd->Flag() == 0 ? "Выход:" : "Выход(без карты)!:",
			cmd->TimeString(), logic_.GetText1(),
		    ct1Tm.tm_hour, ct1Tm.tm_min, ct2Tm.tm_hour, ct2Tm.tm_min, p);
	}

    SMSPostCmd* postCmd = SMSPostCmd::Create(cmd->Key(), cmd->SmsTime(), p, 1);

	if( gsmModem_.Transmit(logic_.GetPhone(cmd->Key()), buffer, postCmd, unsendCmd) )
    {
        return true;
    }
    
    return false;
}

bool SMSProcessor::SendSMS(SMSCommand_2* cmd)
{
    SMSPostCmd* postCmd = SMSPostCmd::Create(cmd->Key());
    if( gsmModem_.Transmit(cmd->Phone(), logic_.GetText2(), postCmd) )
    {
        return true;	
    }

    return false;
}

bool SMSProcessor::SendSMS(SMSCommand_3* cmd)
{
    // 1. опрос устройств
    bool ethernetState = logic_.EthernetState();
    bool webCamState = logic_.WebCamState();
    BYTE ldState = logic_.LockDeviceState();
    char ldStateText[10];
    if( ldState == 0xFF )
        strcpy_s(ldStateText, 10, "C=0");
    else
        sprintf_s(ldStateText, 10, "C=%d%d%d%d",
            (ldState>>3)&0x1, (ldState>>2)&0x1, (ldState>>1)&0x1, ldState&0x1);

    char smsText[100];
    sprintf_s(smsText, 100, "%s %s %s E=%d W=%d %s",
		cmd->DateString(), cmd->TimeString(),
        cmd->Text(), ethernetState ? 1 : 0, webCamState ? 1 : 0, ldStateText);

    SMSPostCmd* postCmd = SMSPostCmd::Create();
    if( gsmModem_.Transmit(logic_.GetPhone(), smsText, postCmd) )
    {
        return true;
    }

    return false;
}

bool SMSProcessor::SendSMS(SMSCommand_4* cmd)
{
	int lastPotential = logic_.GetLastPotentialFromControl(cmd->Key());

    // 1.
    int potential = logic_.GetPotential(cmd->Key());
    int newPotential = 0;
    if( potential >= 0 )
    {
        newPotential = potential + cmd->AddPotential();
        logic_.SetPotential(cmd->Key(), newPotential);
        logic_.RemoveFromSuspendedLog(cmd->Key());
    }
    else
    {
        newPotential = -int((abs(potential)+cmd->AddPotential()));
        logic_.SetPotential(cmd->Key(), newPotential);
    }

    // 2.
    queue2_.Remove(cmd->Key(), 2);

    // 3. перенесено в начало
    //int lastPotential = logic_.GetLastPotentialFromControl(cmd->Key());

    char smsText[100];
    char phone[15];
    strcpy_s(phone, 15, logic_.GetPhone(cmd->Key()));
    if( strlen(phone) )
    {
        time_t crTime;
        time(&crTime);
		SMSCommand* newCmd = SMSCommand::Create(5, crTime, cmd->Key(), phone, cmd->AddPotential());
		if( newCmd )
			queue1_.PushBack(newCmd);
        
        sprintf_s(smsText, 100, "%08d %d %d", cmd->Key(), lastPotential, newPotential); 
    }
    else
    {
        sprintf_s(smsText, 100, "%08d Тел.-нет %d %d", cmd->Key(), lastPotential, newPotential);
    }

    SMSPostCmd* postCmd = SMSPostCmd::Create();
    if( gsmModem_.Transmit(logic_.GetPhone(), smsText, postCmd) )
    {
        return true;
    }

    return false;
}

bool SMSProcessor::SendSMS(SMSCommand_5* cmd)
{
    char smsText[200];

    sprintf_s(smsText, 200, "%s %s %08d %s %d",
        cmd->DateString(), cmd->TimeString(),
        cmd->Key(), logic_.GetText3(), cmd->AddParam());

    SMSPostCmd* postCmd = SMSPostCmd::Create();
    if( gsmModem_.Transmit(cmd->Phone(), smsText, postCmd) )
    {
        return true;
    }

    return false;
}

bool SMSProcessor::SendSMS(SMSCommand_6* cmd)
{
    int potential = logic_.GetPotential(cmd->Key());
    int newPotential = -abs(potential);
    logic_.SetPotential(cmd->Key(), newPotential);

    char smsText[200];
    sprintf_s(smsText, 200, "%s %s %08d заблокирован %d",
        cmd->DateString(), cmd->TimeString(),
        cmd->Key(), newPotential);

    SMSPostCmd* postCmd = SMSPostCmd::Create(cmd->Key(), cmd->Code());
    if( gsmModem_.Transmit(logic_.GetPhone(), smsText, postCmd) )
    {
        return true;
    }

    return false;
}

bool SMSProcessor::SendSMS(SMSCommand_7* cmd)
{
    int potential = logic_.GetPotential(cmd->Key());
    int newPotential = abs(potential);
    logic_.SetPotential(cmd->Key(), newPotential);

    char smsText[200];

    sprintf_s(smsText, 200, "%s %s %08d разблокирован %d",
        cmd->DateString(), cmd->TimeString(),
        cmd->Key(), newPotential);

    SMSPostCmd* postCmd = SMSPostCmd::Create(cmd->Key(), cmd->Code());
    if( gsmModem_.Transmit(logic_.GetPhone(), smsText, postCmd) )
    {
        return true;
    }

    return false;
}

bool SMSProcessor::SendSMS(SMSCommand_8* cmd)
{
    logic_.SetPhone(cmd->Key(), cmd->Phone());
    char smsText[200];

    sprintf_s(smsText, 200, "%s %s %08d OK %s",
        cmd->DateString(), cmd->TimeString(),
        cmd->Key(), cmd->Phone());

    SMSPostCmd* postCmd = SMSPostCmd::Create();
    if( gsmModem_.Transmit(logic_.GetPhone(), smsText, postCmd) )
    {
        return true;
    }

    return false;
}

bool SMSProcessor::SendSMS(SMSCommand_9* cmd)
{
    int lastPotential = logic_.GetLastPotentialFromControl(cmd->Key());
    int potential = logic_.GetPotential(cmd->Key());

    char smsText[200];

    int urgTime = int(logic_.GetUrgency(cmd->Key()));

    char urgTimeBuffer[10];
	sprintf_s(urgTimeBuffer, 10, "%02d:%02d", urgTime/(60*60), (urgTime/60)%60);

    sprintf_s(smsText, 200, "%s %s %08d %d %s %s",
        cmd->DateString(), cmd->TimeString(),
        cmd->Key(), potential,
        urgTimeBuffer,
        logic_.GetLastTime(cmd->Key()));

    SMSPostCmd* postCmd = SMSPostCmd::Create();
    if( gsmModem_.Transmit(logic_.GetPhone(), smsText, postCmd) )
    {
        return true;
    }

    return false;
}

bool SMSProcessor::SendSMS(SMSCommand_10* cmd)
{
    logic_.SetUrgency(cmd->Key(), cmd->Urgency());
    
    int urgTime = int(cmd->Urgency());
    char urgTimeBuffer[10];
	sprintf_s(urgTimeBuffer, 10, "%02d:%02d", urgTime/(60*60), (urgTime/60)%60);

    char smsText[200];
    sprintf_s(smsText, 200, "%s %s %08d Акт.-%s",
        cmd->DateString(), cmd->TimeString(),
        cmd->Key(), urgTimeBuffer);

    SMSPostCmd* postCmd = SMSPostCmd::Create();
    if( gsmModem_.Transmit(logic_.GetPhone(), smsText, postCmd) )
    {
        return true;
    }

    return false;
}

bool SMSProcessor::SendSMS(SMSCommand_11* cmd)
{
	logic_.SetPhone(cmd->Phone());

	SMSPostCmd* postCmd = SMSPostCmd::Create();
	if( gsmModem_.Transmit(logic_.GetPhone(), "OK", postCmd) )
	{
		return true;
	}

	return false;
}

bool SMSProcessor::SendSMS(SMSCommand_12* cmd)
{
	SMSPostCmd* postCmd = SMSPostCmd::Create();
	if( gsmModem_.Transmit(logic_.GetPhone(), cmd->Text(), postCmd) )
	{
		return true;
	}

	return false;
}

bool SMSProcessor::SendSMS(SMSCommand_13* cmd)
{
	SMSPostCmd* postCmd = SMSPostCmd::Create();
	if( gsmModem_.Transmit(cmd->Phone(), logic_.GetText0(), postCmd) )
	{
		return true;
	}

	return false;
}

bool SMSProcessor::SendSMS(SMSCommand_16* cmd)
{
	logic_.SetHaltSystem();

	return true;
}

void SMSProcessor::PostSMS(SMSPostCmd_0* cmd)
{
	logic_.SetPotential(cmd->Key(), cmd->Potential());
	char buffer[200];
	// формирование записи для лог-файла "контроль"
	// ...
	time_t crTime;
	time(&crTime);
	char strDate[10];
	char strTime[10];
	DateTimeString(crTime, strDate, strTime);

	logic_.AddToControl(cmd->Key(), cmd->SmsTime(), 0, cmd->Potential());
	
	sprintf_s(buffer, 200, "%s %s %d 0 %s %d", strDate, strTime, cmd->Key(), cmd->TimeString(), cmd->Potential());
	logic_.AddToSendedLog(buffer);

	logic_.IncOkSMS();

	if( cmd->Potential() < logic_.GetSMSpr() )
	{
		queue1_.Remove(cmd->Key(), 0);
		queue1_.Remove(cmd->Key(), 1);
		queueWait_.Remove(cmd->Key(), 0);
		queueWait_.Remove(cmd->Key(), 1);

		gsmModem_.RemoveSMS(cmd->Key());

		SMSCommand* newCmd = SMSCommand::Create(2, cmd->Key(), logic_.GetPhone(cmd->Key()));
		if( newCmd )
		{
			queue2_.PushBack(newCmd);
			queue2_.SincSave();
		}
	}
}

void SMSProcessor::PostSMS(SMSPostCmd_1* cmd)
{
	logic_.SetPotential(cmd->Key(), cmd->Potential());

	logic_.AddToControl(cmd->Key(),cmd->SmsTime(), 1, cmd->Potential());
	
	char buffer[200];
	// формирование записи для лог-файла "контроль"
	// ...
	time_t crTime;
	time(&crTime);
	char strDate[10];
	char strTime[10];
	DateTimeString(crTime, strDate, strTime);

	sprintf_s(buffer, 200, "%s %s %d 1 %s %d", strDate, strTime, cmd->Key(), cmd->TimeString(), cmd->Potential());
	logic_.AddToSendedLog(buffer);

	logic_.IncOkSMS();

	if( cmd->Potential() < logic_.GetSMSpr() )
	{
		queue1_.Remove(cmd->Key(), 0);
		queue1_.Remove(cmd->Key(), 1);
		queueWait_.Remove(cmd->Key(), 0);
		queueWait_.Remove(cmd->Key(), 1);

		gsmModem_.RemoveSMS(cmd->Key());

		SMSCommand* newCmd = SMSCommand::Create(2, cmd->Key(), logic_.GetPhone(cmd->Key()));
		if( newCmd )
		{
			queue2_.PushBack(newCmd);
			queue2_.SincSave();
		}
	}
}

void SMSProcessor::PostSMS(SMSPostCmd_2* cmd)
{
	logic_.AddToSuspendedLog(cmd->Key());

	logic_.IncTmpSMS();
}

void SMSProcessor::PostSMS(SMSPostCmd_6* cmd)
{
	logic_.AddToSuspendedLog(cmd->Key());

	logic_.IncTmpSMS();
}

void SMSProcessor::PostSMS(SMSPostCmd_7* cmd)
{
	logic_.RemoveFromSuspendedLog(cmd->Key());

	logic_.IncTmpSMS();
}

void SMSProcessor::PostSMS(SMSPostCmd_N* cmd)
{
	logic_.IncTmpSMS();
}

void SMSProcessor::PostSMS(SMSPostCmd_STOP* cmd)
{
	gsmModem_.Stop();
}

void SMSProcessor::UnsendCmd(SMSUnsendCmd* cmd)
{
	char buffer[200];
    time_t crTime;
	time(&crTime);
    char strDate[10];
    char strTime[10];
    DateTimeString(crTime, strDate, strTime);

	sprintf_s(buffer, 200, "%s %s %d %s %s %08d %02d:%02d",
		strDate, strTime, cmd->DateString(), cmd->Code(),
        cmd->TimeString(), cmd->Key(), cmd->Urgency()/(60*60), (cmd->Urgency()/60)%60);

    logic_.AddToUnsendedLog(buffer);
    logic_.IncUnSMS();
}

void SMSProcessor::SendHaltSMS(int okSms, int unSms, int tmpSms, int bugReports)
{
	// сначала сохранить
	queue1_.Clear();
	queue2_.Clear();
	queueWait_.Clear();

	char buffer[200];
	char strDate[10];
	char strTime[10];
	time_t crTime;
	time(&crTime);
	DateTimeString(crTime, strDate, strTime);

	sprintf_s(buffer, 200, "%s %s stop Ok:%d Un:%d Tmp:%d Bug:%d",
		strDate, strTime, okSms, unSms, tmpSms, bugReports);

#ifdef USE_LOG
	logger.Add("SMSProcessor SendHaltSMS:", buffer);
#endif

	gsmModem_.TransmitHalt(logic_.GetPhone(), buffer);
}

void SMSProcessor::ErrorMessage(const char* msg)
{
#ifdef USE_LOG
	char buffer[500];
	sprintf_s(buffer, 500, "error SMS message \"%s\"", msg);
	logger.Add("SMSProcessor", buffer);
#endif

	logic_.GSMErrorMessage(msg); 
}


///////////////////////////////////////////////////////////////////////////////////////////

void SMSProcessor_test()
{
	//ControlDB cDB;
	//ControlParams cParams;
	//SMSProcessor processor(cDB, cParams);
}