#include "StdAfx.h"
#include "SMSQueue.h"
#include <sstream>

#include "SMS.h"
#include "SMSPostCmd.h"

SMSQueue::SMSQueue(void)
: isModified_(false)
, savePtr_(0)
{
	InitializeCriticalSection(&cs_);
}

SMSQueue::~SMSQueue(void)
{
	DeleteCriticalSection(&cs_);
}

void SMSQueue::PushBack(SMS* sms)
{
	Lock();
	deque_.push_back(sms);
	isModified_ = true;
	Unlock();
}

SMS* SMSQueue::PopFront()
{
	SMS* sms = 0;
	
	Lock();
	if( !deque_.empty() )
	{
		sms = deque_.front();
		deque_.pop_front();
		isModified_ = true;
	}
	Unlock();

	return sms;
}

bool SMSQueue::IsEmpty()
{
	bool empty;
	
	Lock();
	empty = deque_.empty();
	Unlock();

	return empty;
}

void SMSQueue::PushFront(SMS* sms)
{
	Lock();
	deque_.push_front(sms);
	isModified_ = true;
	Unlock();
}

void SMSQueue::RemoveByKey(DWORD key)
{
	Lock();
	std::deque< SMS* >::iterator iter;
	for(iter=deque_.begin(); iter!=deque_.end(); )
	{
		SMSPostCmd* pcmd = (*iter)->PostCmd();
		if( pcmd )
		{	
			if( pcmd->Key() == key )
			{
				if( (pcmd->Code() == 0) || (pcmd->Code() == 1) )
					iter = deque_.erase(iter);
				else
					++iter;
			}
			else
				++iter;
		}
		else
			++iter;
	}
	Unlock();
}

template<class Archive>
void SMSQueue::serialize(Archive &ar, const unsigned int version)
{
	ar & deque_;
}

void SMSQueue::SincSave()
{
	Lock();
	if( isModified_ )
		Save();
	isModified_ = false;
	Unlock();
}

void SMSQueue::Save()
{
	if( savePtr_ )
	{
		std::string str;
		std::ostringstream os(str);
		boost::archive::text_oarchive oa(os);
		oa << *this;

		savePtr_(os.str().c_str());
	}
}

void SMSQueue::Load(const char* buffer)
{
	std::string str(buffer);
	if( str.empty() )
		return;

	std::istringstream is(str);
	boost::archive::text_iarchive ia(is);
	ia >> *this;
}
