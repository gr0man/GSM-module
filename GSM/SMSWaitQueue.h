#pragma once
#include <windows.h>

#include <boost/serialization/map.hpp>

#include "SMSCommand.h"
#include "../../ControlData/ControlData/FuncTypes.h"
#include <map>

/**	Очередь SMS-команд с синхронизацией многопоточного доступа.
 *	Использование: очередь SMSwait (см. ТЗ).
 */
class SMSWaitQueue
{
public:
	SMSWaitQueue(void);
	~SMSWaitQueue(void);

	bool IsEmpty();
	void Push(SMSCommand* cmd, time_t sendTime);

	size_t PopEarly(time_t crTime, SMSCommand** buffer, size_t bufSize);
	size_t Find(DWORD key, BYTE code, SMSCommand** buffer, size_t bufSize);
	bool Remove(DWORD key, BYTE code);

	size_t GetText(char* buffer, size_t bufLen);
	void Clear();

	void SetCallback(pSaveCmdPtr callback){ savePtr_ = callback; }
	void Load(const char* buffer);

	void SincSave();

private:

	bool isModified_;

	pSaveCmdPtr savePtr_;
	void Save();

	std::multimap< time_t, SMSCommand* > multimap_;
	CRITICAL_SECTION cs_;

	void Lock() { EnterCriticalSection(&cs_); }
	void Unlock(){ LeaveCriticalSection(&cs_); }

	template<class Archive>
	void serialize(Archive &ar, const unsigned int version);
	/*
	{
		ar.register_type(static_cast<SMSCommand_0*>(NULL));
		ar.register_type(static_cast<SMSCommand_1*>(NULL));
		//ar.register_type(static_cast<SMSCommand_2*>(NULL));
		//ar.register_type(static_cast<SMSCommand_3*>(NULL));
		//ar.register_type(static_cast<SMSCommand_4*>(NULL));
		//ar.register_type(static_cast<SMSCommand_5*>(NULL));
		//ar.register_type(static_cast<SMSCommand_6*>(NULL));
		//ar.register_type(static_cast<SMSCommand_7*>(NULL));
		//ar.register_type(static_cast<SMSCommand_8*>(NULL));
		//ar.register_type(static_cast<SMSCommand_9*>(NULL));
		//ar.register_type(static_cast<SMSCommand_10*>(NULL));
		//ar.register_type(static_cast<SMSCommand_11*>(NULL));
		//ar.register_type(static_cast<SMSCommand_12*>(NULL));
		//ar.register_type(static_cast<SMSCommand_13*>(NULL));
		//ar.register_type(static_cast<SMSCommand_16*>(NULL));

		ar & multimap_;
	}*/

	friend class boost::serialization::access;
};
