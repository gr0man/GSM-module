#include "StdAfx.h"
#include "SMSPostCmd.h"
#include "SMSPostCmdImpl.h"

SMSPostCmd::SMSPostCmd(void)
{
}

SMSPostCmd::~SMSPostCmd(void)
{
}


// 0 и 1
SMSPostCmd* SMSPostCmd::Create(DWORD key, time_t smsTime, int potential, BYTE flag)
{
	SMSPostCmd* cmd = 0;

	switch( flag )
	{
	case 0:
		cmd = new SMSPostCmd_0(key, smsTime, potential);
		break;
	case 1:
		cmd = new SMSPostCmd_1(key, smsTime, potential);
		break;
	}

	return cmd;
}

// 2
SMSPostCmd* SMSPostCmd::Create(DWORD key)
{
	SMSPostCmd* cmd = 0;

	cmd = new SMSPostCmd_2(key);

	return cmd;
}

// 6 и 7
SMSPostCmd* SMSPostCmd::Create(DWORD key, BYTE numb)
{
	SMSPostCmd* cmd = 0;

	switch( numb )
	{
	case 6:
		cmd = new SMSPostCmd_6(key);
		break;
	case 7:
		cmd = new SMSPostCmd_7(key);
		break;
	}

	return cmd;
}

// остальные
SMSPostCmd* SMSPostCmd::Create()
{
	SMSPostCmd* cmd = 0;

	cmd = new SMSPostCmd_N();

	return cmd;
}

SMSPostCmd* SMSPostCmd::CreateStop()
{
	return new SMSPostCmd_STOP();
}