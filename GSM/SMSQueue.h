#pragma once

#include <windows.h>
#include <boost/serialization/deque.hpp>
#include <deque>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include "../../ControlData/ControlData/FuncTypes.h"

class SMS;

/**	Очередь для непосредственной отправки SMS-сообщений
 */
class SMSQueue
{
public:
	SMSQueue(void);
	~SMSQueue(void);

	void PushBack(SMS* sms);
	SMS* PopFront();
	bool IsEmpty();
	void PushFront(SMS* sms);

	void SetCallback(pSaveCmdPtr callback){ savePtr_ = callback; }
	void Load(const char* buffer);

	void RemoveByKey(DWORD key);

	void SincSave();

private:
	pSaveCmdPtr savePtr_;
	void Save();
	bool isModified_;

	std::deque<SMS*> deque_;
	CRITICAL_SECTION cs_;

	void Lock(){ EnterCriticalSection(&cs_); }
	void Unlock(){ LeaveCriticalSection(&cs_); }

	template<class Archive>
	void serialize(Archive &ar, const unsigned int version);

    friend class boost::serialization::access;
    friend std::ostream & operator<<(std::ostream &os, const SMSQueue &bs);
};
