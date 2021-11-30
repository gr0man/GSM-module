#pragma once


#include "SMSCommand.h"
#include "SMSPostCmd.h"
#include "SMSUnsendCmd.h"

class SMSCommand_0;
class SMSCommand_1;
class SMSCommand_2;
class SMSCommand_3;
class SMSCommand_4;
class SMSCommand_5;
class SMSCommand_6;
class SMSCommand_7;
class SMSCommand_8;
class SMSCommand_9;
class SMSCommand_10;
class SMSCommand_11;
class SMSCommand_12;
class SMSCommand_13;
class SMSCommand_16;

class SMSPostCmd_0;
class SMSPostCmd_1;
class SMSPostCmd_2;
class SMSPostCmd_6;
class SMSPostCmd_7;
class SMSPostCmd_N;
class SMSPostCmd_STOP;

/**
 *	Абстрактный базовый класс - интерфейс обработчика SMS-команд.
 */
class SMSTransiever
{
public:

	SMSTransiever(){};
	virtual ~SMSTransiever(){};

	virtual void ProcessRecSMS(const char* text) = 0;

	virtual bool SendSMS(SMSCommand_0* cmd)=0;
	virtual bool SendSMS(SMSCommand_1* cmd)=0;
	virtual bool SendSMS(SMSCommand_2* cmd)=0;
	virtual bool SendSMS(SMSCommand_3* cmd)=0;
	virtual bool SendSMS(SMSCommand_4* cmd)=0;
	virtual bool SendSMS(SMSCommand_5* cmd)=0;
	virtual bool SendSMS(SMSCommand_6* cmd)=0;
	virtual bool SendSMS(SMSCommand_7* cmd)=0;
	virtual bool SendSMS(SMSCommand_8* cmd)=0;
	virtual bool SendSMS(SMSCommand_9* cmd)=0;
	virtual bool SendSMS(SMSCommand_10* cmd)=0;
	virtual bool SendSMS(SMSCommand_11* cmd)=0;
	virtual bool SendSMS(SMSCommand_12* cmd)=0;
	virtual bool SendSMS(SMSCommand_13* cmd)=0;
	virtual bool SendSMS(SMSCommand_16* cmd)=0;

	virtual void PostSMS(SMSPostCmd_0* cmd)=0;
	virtual void PostSMS(SMSPostCmd_1* cmd)=0;
	virtual void PostSMS(SMSPostCmd_2* cmd)=0;
	virtual void PostSMS(SMSPostCmd_6* cmd)=0;
	virtual void PostSMS(SMSPostCmd_7* cmd)=0;
	virtual void PostSMS(SMSPostCmd_N* cmd)=0;
	virtual void PostSMS(SMSPostCmd_STOP* cmd)=0;

	virtual void UnsendCmd(SMSUnsendCmd* cmd)=0;

	virtual void ErrorMessage(const char* msg) = 0;

};
