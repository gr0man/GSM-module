#pragma once

#include <windows.h>

#include <time.h>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include <string>


class SMSTransiever;

/**	Базовый абстрактный класс.
 *	Инкапсулирует данные команды на отправку SMS.
 */
class SMSCommand
{
	static BYTE const inpSmsLen[17];

public:
	time_t SmsTime() const;

	time_t SendTime() const;
	void SendTime(time_t sendTime){ sendTime_ = sendTime; }

	// обобщенная
	static SMSCommand* Create(const char* text, WORD passSmsParam, const char* addText = 0);
	// для создания команд 0, 1
	static SMSCommand* Create(BYTE code, time_t smsTime, DWORD key, BYTE flag);
	// для создания команды 2
	static SMSCommand* Create(BYTE code, DWORD key, const char* phone);
	// для создания команды 5
	static SMSCommand* Create(BYTE code, time_t smsTime, DWORD key, const char* phone, unsigned int potential);
	// для создания команды 13
	static SMSCommand* Create(BYTE code, const char* phone);

	// для создания команды с кодом ошибки
	//static SMSCommand* Create_12(const char* text);

	virtual bool Execute(SMSTransiever& transiever)=0;
	virtual BYTE Code() const =0;


	SMSCommand(time_t smsTime, DWORD key);
	virtual ~SMSCommand(void);

	DWORD Key() const { return key_; }

	const char* DateString() const { return dateStr_.c_str(); }
	const char* TimeString() const { return timeStr_.c_str(); }

	virtual size_t GetText(char* buffer, size_t bufLen) = 0;

protected:
	void SetSmsTime(time_t smsTime){ smsTime_ = smsTime; }

private:
	time_t smsTime_;
	time_t sendTime_;
	DWORD key_;
	std::string dateStr_;
	std::string timeStr_;

	
	template<class Archive>
	void serialize(Archive & ar, const unsigned int)
	{
		ar & smsTime_;
		ar & sendTime_;
		ar & key_;
		ar & dateStr_ & timeStr_;
	}

    friend std::ostream & operator<<(std::ostream &os, const SMSCommand &cmd);
    friend class boost::serialization::access;
	
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(SMSCommand)



