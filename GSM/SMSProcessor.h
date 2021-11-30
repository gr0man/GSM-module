#pragma once

//#include <boost/thread.hpp>

#include "SMSTransiever.h"
#include "SMSCommandQueue.h"
#include "SMSWaitQueue.h"

#include "GSMModule.h"

class ControlDB;
class ControlParams;

class SMSLogic;

/**	Реализует интерфейс SMSTransiever
 */
class SMSProcessor : public SMSTransiever
{
public:
	SMSProcessor(SMSLogic& logic);
	virtual ~SMSProcessor(void);

	virtual bool SendSMS(SMSCommand_0* cmd);
	virtual bool SendSMS(SMSCommand_1* cmd);
	virtual bool SendSMS(SMSCommand_2* cmd);
	virtual bool SendSMS(SMSCommand_3* cmd);
	virtual bool SendSMS(SMSCommand_4* cmd);
	virtual bool SendSMS(SMSCommand_5* cmd);
	virtual bool SendSMS(SMSCommand_6* cmd);
	virtual bool SendSMS(SMSCommand_7* cmd);
	virtual bool SendSMS(SMSCommand_8* cmd);
	virtual bool SendSMS(SMSCommand_9* cmd);
	virtual bool SendSMS(SMSCommand_10* cmd);
	virtual bool SendSMS(SMSCommand_11* cmd);
	virtual bool SendSMS(SMSCommand_12* cmd);
	virtual bool SendSMS(SMSCommand_13* cmd);
	virtual bool SendSMS(SMSCommand_16* cmd);

	virtual void PostSMS(SMSPostCmd_0* cmd);
	virtual void PostSMS(SMSPostCmd_1* cmd);
	virtual void PostSMS(SMSPostCmd_2* cmd);
	virtual void PostSMS(SMSPostCmd_6* cmd);
	virtual void PostSMS(SMSPostCmd_7* cmd);
	virtual void PostSMS(SMSPostCmd_N* cmd);
	virtual void PostSMS(SMSPostCmd_STOP* cmd);

	virtual void UnsendCmd(SMSUnsendCmd* cmd);

	virtual void ProcessRecSMS(const char* text);

	virtual void ErrorMessage(const char* msg);//{ logic_.GSMErrorMessage(msg); }

	bool ProcessSendQueue1();
	void ProcessSendQueue2();
	void ProcessSendWait();

	bool Start(const wchar_t* portname);
	void Stop();

	void AddSMS1(BYTE code, time_t crTime, DWORD key, BYTE flag);
	void AddSMS2(BYTE code, const char* phone);
	void AddStartSMS(const char* text);

	void SendHaltSMS(int okSms, int unSms, int tmpSms, int bugReports);


	void Init_SMS1Save(pSaveCmdPtr callback)	{ queue1_.SetCallback(callback); }
	void Init_SMS2Save(pSaveCmdPtr callback)	{ queue2_.SetCallback(callback); }
	void Init_SMSwaitSave(pSaveCmdPtr callback)	{ queueWait_.SetCallback(callback); }
	void LoadSMS1(const char* buffer)	{ queue1_.Load(buffer); }
	void LoadSMS2(const char* buffer)	{ queue2_.Load(buffer); }
	void LoadSMSwait(const char* buffer){ queueWait_.Load(buffer); }

	void Init_SMSQueueSave(pSaveCmdPtr callback)	{ gsmModem_.Init_SMSQueueSave(callback); }
	void LoadSMSQueue(const char* buffer)	{ gsmModem_.LoadSMSQueue(buffer); }

	size_t GetTextSMS1(char* buffer, size_t bufLen)		{ return queue1_.GetText(buffer, bufLen); }
	size_t GetTextSMS2(char* buffer, size_t bufLen)		{ return queue2_.GetText(buffer, bufLen); }
	size_t GetTextSMSwait(char* buffer, size_t bufLen)	{ return queueWait_.GetText(buffer, bufLen); }

private:

	SMSLogic& logic_;

	SMSCommandQueue queue1_;

	GSMModule gsmModem_;

	SMSCommandQueue queue2_;
	SMSWaitQueue queueWait_;

	
	void SetErrorSMS(SMSCommand* cmd);
	void RemoveCmd(SMSCommandQueue& queue, BYTE code, DWORD key);

	void DateTimeString(time_t inTime, char strDate[10], char strTime[10]);

	//boost::thread thread_;
	HANDLE thread_;
	//volatile bool stop_;
	HANDLE stopEv_;
	//void WorkThread();
	static unsigned int WINAPI WorkThread(void* param);

};
