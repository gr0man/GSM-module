#include "stdafx.h"
#include "SMSUnsendCmd.h"

#include "SMSTransiever.h"


SMSUnsendCmd::SMSUnsendCmd(BYTE code, DWORD key, time_t smsTime, int urgency)
: code_(code)
, key_(key)
, urgency_(urgency)
, lastSendTime_(smsTime + urgency)
{
	char buffer[20];

	struct tm smsTimeTm;
	localtime_s(&smsTimeTm, &smsTime);

	sprintf_s(buffer, 20, "%02d.%02d.%02d",
		smsTimeTm.tm_mday, smsTimeTm.tm_mon+1, smsTimeTm.tm_year-100);
	dateString_ = std::string(buffer);

	sprintf_s(buffer, 20, "%02d:%02d", smsTimeTm.tm_hour, smsTimeTm.tm_min);
	timeString_ = std::string(buffer);
}

SMSUnsendCmd::~SMSUnsendCmd()
{
}

void SMSUnsendCmd::Execute(SMSTransiever& transiever)
{
	transiever.UnsendCmd(this);
}