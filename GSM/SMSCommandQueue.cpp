#include "StdAfx.h"
#include "SMSCommandQueue.h"
#include "SMSCommandImpl.h"
#include <sstream>

#include <time.h>

#include "../../ControlData/ControlData/Logger.h"

SMSCommandQueue::SMSCommandQueue(void)
: savePtr_(0)
, isModified_(false)
{
	InitializeCriticalSection(&cs_);
}

SMSCommandQueue::~SMSCommandQueue(void)
{
	DeleteCriticalSection(&cs_);
}

void SMSCommandQueue::Lock()
{ 
	EnterCriticalSection(&cs_);
}

void SMSCommandQueue::Unlock()
{ 
	LeaveCriticalSection(&cs_);
}

// ищет все значения с задаваемыми ключом и кодом и складывает их в буфер
size_t SMSCommandQueue::Find(DWORD key, BYTE code, SMSCommand** buffer, size_t bufSize)
{
	size_t fcntr = 0;
	Lock();

	std::list< SMSCommand* >::const_iterator iter;
	for(iter=list_.begin(); iter!=list_.end(); ++iter)
	{
		if( ((*iter)->Code() == code) && ((*iter)->Key() == key) )
		{
			if( fcntr < bufSize )
				buffer[fcntr] = *iter;
			fcntr++;
		}
	}

	Unlock();
	return fcntr;
}

void SMSCommandQueue::Remove(DWORD key, BYTE code)
{
	Lock();

	list_.remove_if( is_eq_remove(key, code) );
	isModified_ = true;

	//Save();

	Unlock();
}

void SMSCommandQueue::PushBack(SMSCommand* cmd)
{
	Lock();

	list_.push_back(cmd);
	isModified_ = true;

	//Save();

	Unlock();
}

void SMSCommandQueue::PushFront(SMSCommand* cmd)
{
	Lock();

	list_.push_front(cmd);
	isModified_ = true;

	//Save();

	Unlock();
}

SMSCommand* SMSCommandQueue::PopFront()
{
	SMSCommand* cmd = 0;
	Lock();

	if( !list_.empty() )
	{
		cmd = list_.front();
		list_.pop_front();

		isModified_ = true;

		//Save();
	}

	Unlock();
	return cmd;
}

bool SMSCommandQueue::IsEmpty()
{
	bool ret = 0;
	Lock();

	ret = list_.empty();

	Unlock();
	return ret;
}

size_t SMSCommandQueue::GetText(char* buffer, size_t bufLen)
{
	size_t len = 0;
	Lock();

	std::list< SMSCommand* >::iterator iter;
	for(iter=list_.begin(); (iter!=list_.end()) && (len<bufLen); ++iter)
	{
		len += (*iter)->GetText(buffer+len, bufLen-len);
	}

	Unlock();
	return len;
}

void SMSCommandQueue::Clear()
{
	Lock();
	std::list< SMSCommand* >::iterator iter;
	for(iter=list_.begin(); iter!=list_.end(); ++iter)
	{
		delete *iter;
		isModified_ = true;
	}
	list_.clear();

	//Save();

	Unlock();
}


template<class Archive>
void SMSCommandQueue::serialize(Archive &ar, const unsigned int version)
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


void SMSCommandQueue::SincSave()
{
	Lock();
	if( isModified_ )
		Save();
	isModified_ = false;
	Unlock();
}

void SMSCommandQueue::Save()
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

void SMSCommandQueue::Load(const char* buffer)
{
	std::string str(buffer);
	if( str.empty() )
		return;

	std::istringstream is(str);
	boost::archive::text_iarchive ia(is);
	ia >> *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <sstream>
//#include <boost/archive/archive_exception.hpp>
//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/xml_oarchive.hpp>
//#include <boost/archive/xml_iarchive.hpp>

#include <iostream>


void WINAPI SaveCallback(const char* text)
{
	std::cout << text << std::endl;
}

void SMSCommandQueue_test()
{
	/*
	SMSCommandQueue cq;

	cq.SetCallback(SaveCallback);

	time_t crTime;
	time(&crTime);

	cq.PushBack(SMSCommand::Create(0, crTime, 10000, 0));
	cq.PushBack(SMSCommand::Create(1, crTime, 10001, 0));
	cq.PushBack(SMSCommand::Create(2, 10000, "9031234567"));
	cq.PushBack(SMSCommand::Create("0111,3", 111));
	cq.PushBack(SMSCommand::Create("0111,4,00010000,100", 111));
	cq.PushBack(SMSCommand::Create(5, crTime, 10000, "9031234567", 100));
	cq.PushBack(SMSCommand::Create("0111,6,00010000", 111));
	cq.PushBack(SMSCommand::Create("0111,7,00010000", 111));
	cq.PushBack(SMSCommand::Create("0111,8,00010000,9031234567", 111));
	cq.PushBack(SMSCommand::Create("0111,9,00010000", 111));
	cq.PushBack(SMSCommand::Create("0111,10,00010000,12:10", 111));
	cq.PushBack(SMSCommand::Create("0111,11,9031234567", 111));
	cq.PushBack(SMSCommand::Create("0111,12,xxxxx", 111));
	cq.PushBack(SMSCommand::Create(13, "9031234567"));
	cq.PushBack(SMSCommand::Create("0111,16", 111));


	char buffer[10000];
	size_t len=0;
	len = cq.GetText(buffer, 10000);

	std::fstream fs;
	fs.open("d:\\projects\\AccessControl_projects\\CmdQueueTest.txt", std::ios_base::out);
	fs << buffer << std::endl;
	fs.close();


	try
	{
		//std::stringstream ss;
		std::fstream ofs;
		ofs.open("d:\\projects\\AccessControl_projects\\arch.xml", std::ios_base::out);
		boost::archive::text_oarchive oa(ofs);
		oa << cq;
		//ofs << ss.str();
		ofs.close();

		cq.Clear();

		ofs.open("d:\\projects\\AccessControl_projects\\arch.xml", std::ios_base::in);
		boost::archive::text_iarchive ia(ofs);
		ia >> cq;
		buffer[0] = 0;
		len = cq.GetText(buffer, 10000);
		ofs.close();

		std::fstream fs;
		fs.open("d:\\projects\\AccessControl_projects\\CmdQueueTest2.txt", std::ios_base::out);
		fs << buffer << std::endl;
		fs.close();

	}
	catch(boost::archive::archive_exception& exc)
	{
		std::cout << exc.what() << std::endl;
	}


	cq.Clear();
	*/

}