#include "StdAfx.h"
#include "SMSPostCmdImpl.h"
#include "SMSTransiever.h"

#include <time.h>

SMSPostCmd_0::SMSPostCmd_0(DWORD key, time_t smsTime, int potential)
: key_(key)
, smsTime_(smsTime)
, potential_(potential)
{
	char buffer[20];

	struct tm smsTimeTm;
	localtime_s(&smsTimeTm, &smsTime_);

	sprintf_s(buffer, 20, "%02d:%02d", smsTimeTm.tm_hour, smsTimeTm.tm_min);
	timeStr_ = std::string(buffer);
}

SMSPostCmd_0::SMSPostCmd_0()
: key_(0)
, smsTime_(0)
, potential_(0)
{
}

SMSPostCmd_0::~SMSPostCmd_0(void)
{
}

void SMSPostCmd_0::PostProcess(SMSTransiever& transiever)
{
	transiever.PostSMS(this);
}

/////////////////////////////////////////////////////////////////////////////////////////////

SMSPostCmd_1::SMSPostCmd_1(DWORD key, time_t smsTime, int potential)
: key_(key)
, smsTime_(smsTime)
, potential_(potential)
{
	char buffer[20];

	struct tm smsTimeTm;
	localtime_s(&smsTimeTm, &smsTime_);

	sprintf_s(buffer, 20, "%02d:%02d", smsTimeTm.tm_hour, smsTimeTm.tm_min);
	timeStr_ = std::string(buffer);
}

SMSPostCmd_1::SMSPostCmd_1()
: key_(0)
, smsTime_(0)
, potential_(0)
{
}

SMSPostCmd_1::~SMSPostCmd_1()
{
}

void SMSPostCmd_1::PostProcess(SMSTransiever& transiever)
{
	transiever.PostSMS(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////

SMSPostCmd_2::SMSPostCmd_2(DWORD key)
: key_(key)
{
}

SMSPostCmd_2::SMSPostCmd_2()
: key_(0)
{
}

SMSPostCmd_2::~SMSPostCmd_2()
{
}

void SMSPostCmd_2::PostProcess(SMSTransiever& transiever)
{
	transiever.PostSMS(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////

SMSPostCmd_6::SMSPostCmd_6(DWORD key)
: key_(key)
{
}

SMSPostCmd_6::SMSPostCmd_6()
: key_(0)
{
}

SMSPostCmd_6::~SMSPostCmd_6()
{
}

void SMSPostCmd_6::PostProcess(SMSTransiever& transiever)
{
	transiever.PostSMS(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////

SMSPostCmd_7::SMSPostCmd_7(DWORD key)
: key_(key)
{
}

SMSPostCmd_7::SMSPostCmd_7()
: key_(0)
{
}

SMSPostCmd_7::~SMSPostCmd_7()
{
}

void SMSPostCmd_7::PostProcess(SMSTransiever& transiever)
{
	transiever.PostSMS(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////

SMSPostCmd_N::SMSPostCmd_N()
{
}

SMSPostCmd_N::~SMSPostCmd_N()
{
}

void SMSPostCmd_N::PostProcess(SMSTransiever& transiever)
{
	transiever.PostSMS(this);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

SMSPostCmd_STOP::SMSPostCmd_STOP()
{
}

SMSPostCmd_STOP::~SMSPostCmd_STOP()
{
}

void SMSPostCmd_STOP::PostProcess(SMSTransiever& transiever)
{
	transiever.PostSMS(this);
}