#pragma once

#include <windows.h>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/assume_abstract.hpp>

class SMSTransiever;

/**	Базовый абстрактный класс, инкапсулирующий
 *	поведение SMS-команды после неудачной отправки
 *	SMS-сообщения.
 */
class SMSUnsendCmd
{
public:

	SMSUnsendCmd(BYTE code, DWORD key, time_t smsTime, int urgency);
	virtual ~SMSUnsendCmd();

	virtual void Execute(SMSTransiever& transmiever);

	time_t LastSendTime() const { return lastSendTime_; }
	BYTE Code() const { return code_; }
	const char* DateString() { return dateString_.c_str(); }
	const char* TimeString() { return timeString_.c_str(); }
	DWORD Key() const { return key_; }
	int Urgency() const { return urgency_; }

private:

	time_t lastSendTime_;
	BYTE code_;
	DWORD key_;
	int urgency_;
	std::string dateString_;
	std::string timeString_;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int)
	{
		ar & lastSendTime_;
		ar & code_;
		ar & key_;
		ar & urgency_;
		ar & dateString_ & timeString_;
	}

	SMSUnsendCmd(){};

	friend class boost::serialization::access;
};