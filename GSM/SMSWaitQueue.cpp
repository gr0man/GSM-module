#include "StdAfx.h"

#include <algorithm>
#include <sstream>

#include "SMSWaitQueue.h"
#include "SMSCommandImpl.h"

SMSWaitQueue::SMSWaitQueue(void)
: savePtr_(0)
, isModified_(false)
{
	InitializeCriticalSection(&cs_);
}

SMSWaitQueue::~SMSWaitQueue(void)
{
	DeleteCriticalSection(&cs_);
}

bool SMSWaitQueue::IsEmpty()
{
	bool isEmpty = false;
	Lock();

	isEmpty = multimap_.empty();

	Unlock();
	return isEmpty;
}

void SMSWaitQueue::Push(SMSCommand* cmd, time_t sendTime)
{
	Lock();

	multimap_.insert( std::pair< time_t, SMSCommand* >(sendTime, cmd) );
	
	isModified_ = true;
	//Save();

	Unlock();
}

size_t SMSWaitQueue::PopEarly(time_t crTime, SMSCommand** buffer, size_t bufSize)
{
	size_t len = 0;
	Lock();

	// указатель на последний меньший или равный элемент
	std::multimap< time_t, SMSCommand* >::iterator firstLate = multimap_.upper_bound(crTime);
	if( firstLate != multimap_.begin() )
	{
		std::multimap< time_t, SMSCommand* >::iterator iter;
		std::multimap< time_t, SMSCommand* >::iterator lastRemoveIter;
		// перемещаются элементы от первого до найденного
		for(iter=multimap_.begin(); iter!=firstLate; ++iter)
		{
			if( len < bufSize )
			{
				buffer[len] = iter->second;
				lastRemoveIter = iter;
			}
			len++;
		}

		multimap_.erase(multimap_.begin(), lastRemoveIter);
		multimap_.erase(lastRemoveIter);

		isModified_ = true;

		//Save();	
	}

	Unlock();
	return len;
}

size_t SMSWaitQueue::Find(DWORD key, BYTE code, SMSCommand** buffer, size_t bufSize)
{
	size_t len = 0;
	Lock();

	std::multimap< time_t, SMSCommand* >::iterator iter;
	for(iter=multimap_.begin(); iter!=multimap_.end(); ++iter)
	{
		if( (iter->second->Key() == key) && (iter->second->Code() == code) )
		{
			if( len < bufSize )
				buffer[len] = iter->second;
			len++;
		}
	}

	Unlock();
	return len;
}

bool SMSWaitQueue::Remove(DWORD key, BYTE code)
{
	bool ret = false;
	Lock();

	std::multimap< time_t, SMSCommand* >::iterator iter;
	std::multimap< time_t, SMSCommand* >::iterator delIter;
	for(iter=multimap_.begin(), delIter=multimap_.end(); iter!=multimap_.end(); ++iter)
	{
		if( delIter!=multimap_.end() )
		{
			multimap_.erase(delIter);
			delIter = multimap_.end();
			ret = true;
		}

		if( (iter->second->Key() == key) && (iter->second->Code() == code) )
			delIter = iter;
	}
	
	if( delIter!=multimap_.end() )
	{
		multimap_.erase(delIter);
		delIter = multimap_.end();
		ret = true;

		//Save();
	}

	isModified_ = ret;

	Unlock();

	return ret;
}

size_t SMSWaitQueue::GetText(char* buffer, size_t bufLen)
{
	size_t len = 0;
	Lock();

	char tmBuf[100];
	struct tm sendTm;

	std::multimap< time_t, SMSCommand* >::iterator iter;
	for(iter=multimap_.begin(); (iter!=multimap_.end()) && (len<bufLen); ++iter)
	{
		localtime_s(&sendTm, &(iter->first));
		sprintf_s(tmBuf, 100, "%02d.%02d.%02d %02d:%02d |", 
			sendTm.tm_mday, sendTm.tm_mon+1, sendTm.tm_year-100, sendTm.tm_hour, sendTm.tm_min);
		size_t tmpLen = strlen(tmBuf);
		strcpy_s(buffer+len, bufLen-len, tmBuf);
		len += tmpLen;		

		len += iter->second->GetText(buffer+len, bufLen-len);
	}

	Unlock();
	return len;
}

void SMSWaitQueue::Clear()
{
	Lock();

	std::multimap< time_t, SMSCommand* >::iterator iter;
	for(iter=multimap_.begin(); iter!=multimap_.end(); ++iter)
	{
		delete iter->second;
		isModified_ = true;
	}
	multimap_.clear();

	//Save();

	Unlock();
}

void SMSWaitQueue::SincSave()
{
	Lock();
	if( isModified_ )
		Save();
	isModified_ = false;
	Unlock();
}

void SMSWaitQueue::Save()
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

void SMSWaitQueue::Load(const char* buffer)
{
	std::string str(buffer);
	if( str.empty() )
		return;

	std::istringstream is(str);
	boost::archive::text_iarchive ia(is);
	ia >> *this;
}


template<class Archive>
void SMSWaitQueue::serialize(Archive &ar, const unsigned int version)
{
	ar.register_type(static_cast<SMSCommand_0*>(NULL));
	ar.register_type(static_cast<SMSCommand_1*>(NULL));

	ar & multimap_;
}


///////////////////////////////////////////////////////////////////////////////////

#include <fstream>

void SMSWaitQueue_test()
{
	/*
	SMSWaitQueue wq;

	time_t crTime;
	time(&crTime);

	wq.Push(SMSCommand::Create(0, crTime, 10000, 0), crTime);
	crTime += 60;
	wq.Push(SMSCommand::Create(0, crTime, 10000, 1), crTime);
	crTime += 60;
	wq.Push(SMSCommand::Create(1, crTime, 10000, 0), crTime);
	crTime += 60;
	wq.Push(SMSCommand::Create(1, crTime, 10000, 1), crTime);
	crTime += 60;

	char buffer[10000];
	size_t len=0;
	len = wq.GetText(buffer, 10000);

	std::fstream fs;
	fs.open("d:\\projects\\AccessControl_projects\\CmdWaitQueueTest.txt", std::ios_base::out);
	fs << buffer << std::endl;
	fs.close();

	wq.Clear();
	*/
}