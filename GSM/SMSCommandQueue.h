#pragma once

#include <windows.h>
#include <boost/serialization/list.hpp>
#include <list>

#include "SMSCommand.h"

#include "../../ControlData/ControlData/FuncTypes.h"

/**	Очередь SMS-команд с синхронизацией многопоточного доступа.
 *	Использование: очереди SMS1, SMS2 (см. ТЗ).
 */
class SMSCommandQueue
{
	class is_eq_remove : public std::unary_function<SMSCommand*, bool> 
	{
	public:
		is_eq_remove(DWORD key, BYTE code)
			: key_(key)
			, code_(code)
		{
		}

		bool operator()( SMSCommand*& val1 ) 
		{
			if( (val1->Key() == key_) && (val1->Code() == code_) )
			{
				delete val1;
				return true;
			}
			return false;
		}
	private: 
		DWORD key_;
		BYTE code_;
	};

public:
	SMSCommandQueue(void);
	~SMSCommandQueue(void);

	size_t Find(DWORD key, BYTE code, SMSCommand** buffer, size_t bufSize);
	void Remove(DWORD key, BYTE code);
	void PushBack(SMSCommand* cmd);
	void PushFront(SMSCommand* cmd);
	SMSCommand* PopFront();
	bool IsEmpty();

	size_t GetText(char* buffer, size_t bufLen);
	void Clear();

	void SetCallback(pSaveCmdPtr callback){ savePtr_ = callback; }
	void Load(const char* buffer);

	void SincSave();

private:

	pSaveCmdPtr savePtr_;
	void Save();
	bool isModified_;

	CRITICAL_SECTION cs_;	
	
	std::list< SMSCommand* > list_;


	void Lock();//	{ EnterCriticalSection(&cs_); }
	void Unlock();// { LeaveCriticalSection(&cs_); }

	template<class Archive>
	void serialize(Archive &ar, const unsigned int version);
	/*
	{
		ar.register_type(static_cast<SMSCommand_0*>(NULL));
		ar.register_type(static_cast<SMSCommand_1*>(NULL));
		ar.register_type(static_cast<SMSCommand_2*>(NULL));
		ar.register_type(static_cast<SMSCommand_3*>(NULL));
		ar.register_type(static_cast<SMSCommand_4*>(NULL));
		ar.register_type(static_cast<SMSCommand_5*>(NULL));
		ar.register_type(static_cast<SMSCommand_6*>(NULL));
		ar.register_type(static_cast<SMSCommand_7*>(NULL));
		ar.register_type(static_cast<SMSCommand_8*>(NULL));
		ar.register_type(static_cast<SMSCommand_9*>(NULL));
		ar.register_type(static_cast<SMSCommand_10*>(NULL));
		ar.register_type(static_cast<SMSCommand_11*>(NULL));
		ar.register_type(static_cast<SMSCommand_12*>(NULL));
		ar.register_type(static_cast<SMSCommand_13*>(NULL));
		ar.register_type(static_cast<SMSCommand_16*>(NULL));

		ar & list_;
	}
	*/

    friend class boost::serialization::access;
    friend std::ostream & operator<<(std::ostream &os, const SMSCommandQueue &bs);
};
