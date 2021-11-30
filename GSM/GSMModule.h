#pragma once

#include <string>
//#include <boost/thread.hpp>
#include "SMSQueue.h"

#include <fstream>

#include "../../ControlData/ControlData/Comm.h"

class SMSTransiever;
class SMSPostCmd;
class SMSUnsendCmd;

class GSMModule
{
public:
	GSMModule(SMSTransiever& receiver);
	virtual ~GSMModule(void);

	bool Start(const wchar_t* portName);
	void Stop();

	bool Transmit(const char* phone, const char* text, SMSPostCmd* postCmd, SMSUnsendCmd* unsendCmd = 0);
	void TransmitHalt(const char* phone, const char* text);

	void Init_SMSQueueSave(pSaveCmdPtr callback)	{ transQueue_.SetCallback(callback); }
	void LoadSMSQueue(const char* buffer)	{ transQueue_.Load(buffer); }

	void RemoveSMS(DWORD key);

private:

	SMSTransiever& transiever_;
	Comm comm_;

	SMSQueue transQueue_;
	//boost::thread transThread_;
	HANDLE transThread_;
	HANDLE stopEv_;
	HANDLE recTimer_;
	std::wstring portName_;

	static unsigned int WINAPI TransThread(void* param);

	bool Send(SMS* sms);
	bool TryReadSendSmsCmdReq();
	bool TryReadSendSmsReq();

	void TryReceive();
	void TryReadRecSmsReq();

#ifdef DEVICE_EMULATION
	void FictiveReceive();
#endif

	void DeleteUnsended();
	void TryReadUnsendSmsReq();

	void DeleteSMS(size_t index);

};
