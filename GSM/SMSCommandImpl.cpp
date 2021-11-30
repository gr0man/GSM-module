#include "StdAfx.h"

#include "SMSCommandImpl.h"
#include "SMStransiever.h"

SMSCommand_0::SMSCommand_0(time_t smsTime, DWORD key, BYTE flag)
: SMSCommand(smsTime, key)
, flag_(flag)
{
}

SMSCommand_0::SMSCommand_0()
: SMSCommand(0, 0)
, flag_(0)
{
}

SMSCommand_0::~SMSCommand_0()
{
}

bool SMSCommand_0::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_0::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d| %s %s %d %d\n", Code(), DateString(), TimeString(), Key(), Flag());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}

//////////////////////////////////////////////////////////

SMSCommand_1::SMSCommand_1(time_t smsTime, DWORD key, BYTE flag)
: SMSCommand(smsTime, key)
, flag_(flag)
{
}

SMSCommand_1::SMSCommand_1()
: SMSCommand(0, 0)
, flag_(0)
{
}

SMSCommand_1::~SMSCommand_1()
{
}

bool SMSCommand_1::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_1::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d| %s %s %d %d\n", Code(), DateString(), TimeString(), Key(), Flag());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}

///////////////////////////////////////////////////////////

SMSCommand_2::SMSCommand_2(DWORD key, const char* phone)
: SMSCommand(0, key)
, phone_(phone)
{
}

SMSCommand_2::SMSCommand_2()
: SMSCommand(0, 0)
{
}

SMSCommand_2::~SMSCommand_2()
{
}

bool SMSCommand_2::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_2::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d| %d %s\n", Code(), Key(), Phone());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}

///////////////////////////////////////////////////////////

SMSCommand_3::SMSCommand_3(time_t smsTime, const char* text)
: SMSCommand(smsTime, 0)
, text_(text)
{
}

SMSCommand_3::SMSCommand_3()
: SMSCommand(0, 0)
{
}

SMSCommand_3::~SMSCommand_3()
{
}

bool SMSCommand_3::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_3::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d| %s %s %s\n", Code(), DateString(), TimeString(), Text());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}

//////////////////////////////////////////////////////////////

SMSCommand_4::SMSCommand_4(time_t smsTime, DWORD key, unsigned int addPotent)
: SMSCommand(smsTime, key)
, addPotent_(addPotent)
{
}

SMSCommand_4::SMSCommand_4()
: SMSCommand(0, 0)
, addPotent_(0)
{
}

SMSCommand_4::~SMSCommand_4()
{
}

bool SMSCommand_4::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_4::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d| %s %s %d %d\n", Code(), DateString(), TimeString(), Key(), AddPotential());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}

/////////////////////////////////////////////////////////////////////////////////

SMSCommand_5::SMSCommand_5(time_t smsTime, DWORD key, const char* phone, unsigned int addParam)
: SMSCommand(smsTime, key)
, addParam_(addParam)
, phone_(phone)
{
}

SMSCommand_5::SMSCommand_5()
: SMSCommand(0, 0)
, addParam_(0)
{
}

SMSCommand_5::~SMSCommand_5()
{
}

bool SMSCommand_5::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_5::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d| %s %s %d %s %d\n", Code(), DateString(), TimeString(), Key(), Phone(), AddParam());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}

/////////////////////////////////////////////////////////////////////////////////

SMSCommand_6::SMSCommand_6(time_t smsTime, DWORD key)
: SMSCommand(smsTime, key)
{
}

SMSCommand_6::SMSCommand_6()
: SMSCommand(0, 0)
{
}

SMSCommand_6::~SMSCommand_6()
{
}

bool SMSCommand_6::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_6::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d| %s %s %d\n", Code(), DateString(), TimeString(), Key());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}
//////////////////////////////////////////////////////////////////////////////////

SMSCommand_7::SMSCommand_7(time_t smsTime, DWORD key)
: SMSCommand(smsTime, key)
{
}

SMSCommand_7::SMSCommand_7()
: SMSCommand(0, 0)
{
}

SMSCommand_7::~SMSCommand_7()
{
}

bool SMSCommand_7::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_7::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d| %s %s %d\n", Code(), DateString(), TimeString(), Key());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}

/////////////////////////////////////////////////////////////////////////////////////

SMSCommand_8::SMSCommand_8(time_t smsTime, DWORD key, const char* phone)
: SMSCommand(smsTime, key)
, phone_(phone)
{
}

SMSCommand_8::SMSCommand_8()
: SMSCommand(0, 0)
{
}

SMSCommand_8::~SMSCommand_8()
{
}

bool SMSCommand_8::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_8::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d| %s %s %d %s\n", Code(), DateString(), TimeString(), Key(), Phone());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}

///////////////////////////////////////////////////////////////////////////////////////

SMSCommand_9::SMSCommand_9(time_t smsTime, DWORD key)
: SMSCommand(smsTime, key)
{
}

SMSCommand_9::SMSCommand_9()
: SMSCommand(0, 0)
{
}

SMSCommand_9::~SMSCommand_9()
{
}

bool SMSCommand_9::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_9::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d| %s %s %d\n", Code(), DateString(), TimeString(), Key());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}

///////////////////////////////////////////////////////////////////////////////////////

SMSCommand_10::SMSCommand_10(time_t smsTime, DWORD key, time_t newUrgency)
: SMSCommand(smsTime, key)
, newUrgency_(newUrgency)
{
}

SMSCommand_10::SMSCommand_10()
: SMSCommand(0, 0)
, newUrgency_(0)
{
}

SMSCommand_10::~SMSCommand_10()
{
}

bool SMSCommand_10::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_10::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	struct tm urgTm;
	localtime_s(&urgTm, &newUrgency_);
	sprintf_s(buf, 200, "%d| %s %s %d %02d:%02d\n", Code(), DateString(), TimeString(), Key(),
		urgTm.tm_hour, urgTm.tm_min);
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}

////////////////////////////////////////////////////////////////////////////////////////

SMSCommand_11::SMSCommand_11(time_t smsTime, const char* phone)
: SMSCommand(smsTime, 0)
, phone_(phone)
{
}

SMSCommand_11::SMSCommand_11()
: SMSCommand(0, 0)
{
}

SMSCommand_11::~SMSCommand_11()
{
}

bool SMSCommand_11::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_11::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d| %s %s %s\n", Code(), DateString(), TimeString(), Phone());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}

///////////////////////////////////////////////////////////////////////////////////

SMSCommand_12::SMSCommand_12(time_t smsTime, const char* text)
: SMSCommand(smsTime, 0)
, text_(text)
{
}

SMSCommand_12::SMSCommand_12()
: SMSCommand(0, 0)
{
}

SMSCommand_12::~SMSCommand_12()
{
}

bool SMSCommand_12::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_12::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d| %s %s %s\n", Code(), DateString(), TimeString(), Text());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}

/////////////////////////////////////////////////////////////////////////////////////

SMSCommand_13::SMSCommand_13(const char* phone)
: SMSCommand(0, 0)
, phone_(phone)
{
}

SMSCommand_13::SMSCommand_13()
: SMSCommand(0, 0)
{
}

SMSCommand_13::~SMSCommand_13()
{
}

bool SMSCommand_13::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_13::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d| %s\n", Code(), Phone());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}

/////////////////////////////////////////////////////////////////////////////////////

SMSCommand_16::SMSCommand_16()
: SMSCommand(0,0)
{
}

bool SMSCommand_16::Execute(SMSTransiever& transiever)
{
	return transiever.SendSMS(this);
}

size_t SMSCommand_16::GetText(char* buffer, size_t bufLen)
{
	char buf[200];
	sprintf_s(buf, 200, "%d|\n", Code());
	size_t len = strlen(buf);

	if( len<bufLen )
		strcpy_s(buffer, bufLen, buf);

	return len;
}