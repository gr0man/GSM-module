#pragma once

#include <boost/serialization/string.hpp>

#include <string>
#include "SMSPostCmd.h"

class SMSPostCmd_0 : public SMSPostCmd
{
	SMSPostCmd_0(DWORD key, time_t smsTime, int potential);
	SMSPostCmd_0();

public:
	~SMSPostCmd_0(void);

	DWORD Key() const { return key_; }
	time_t SmsTime() const { return smsTime_; }
	int Potential() const { return potential_; }

	virtual BYTE Code() const { return 0; }

	virtual void PostProcess(SMSTransiever& transiever);

	const char* TimeString() const { return timeStr_.c_str(); }

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSPostCmd>(*this);
		ar & key_;
		ar & smsTime_;
		ar & potential_;
		ar & timeStr_;
	}

private:
	DWORD key_;
	time_t smsTime_;
	int potential_;

	std::string timeStr_;

	friend class SMSPostCmd;
	friend class boost::serialization::access;
};

class SMSPostCmd_1 : public SMSPostCmd
{
	SMSPostCmd_1(DWORD key, time_t smsTime, int potential);
	SMSPostCmd_1();
public:
	~SMSPostCmd_1(void);

	virtual DWORD Key() const { return key_; }
	time_t SmsTime() const { return smsTime_; }
	int Potential() const { return potential_; }

	virtual BYTE Code() const { return 1; }

	virtual void PostProcess(SMSTransiever& transiever);

	const char* TimeString() const { return timeStr_.c_str(); }

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSPostCmd>(*this);
		ar & key_;
		ar & smsTime_;
		ar & potential_;
		ar & timeStr_;
	}

private:
	DWORD key_;
	time_t smsTime_;
	int potential_;

	std::string timeStr_;


	friend class SMSPostCmd;
	friend class boost::serialization::access;
};

class SMSPostCmd_2 : public SMSPostCmd
{
	SMSPostCmd_2(DWORD key);
	SMSPostCmd_2();
public:
	~SMSPostCmd_2(void);

	virtual DWORD Key() const { return key_; }

	virtual BYTE Code() const { return 2; }

	virtual void PostProcess(SMSTransiever& transiever);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSPostCmd>(*this);
		ar & key_;
	}

private:
	DWORD key_;

	friend class SMSPostCmd;
	friend class boost::serialization::access;
};

class SMSPostCmd_6 : public SMSPostCmd
{
	SMSPostCmd_6(DWORD key);
	SMSPostCmd_6();
public:
	~SMSPostCmd_6(void);

	virtual DWORD Key() const { return key_; }

	virtual BYTE Code() const { return 6; }

	virtual void PostProcess(SMSTransiever& transiever);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSPostCmd>(*this);
		ar & key_;
	}

private:
	DWORD key_;

	friend class SMSPostCmd;
	friend class boost::serialization::access;
};

class SMSPostCmd_7 : public SMSPostCmd
{
	SMSPostCmd_7(DWORD key);
	SMSPostCmd_7();
public:
	~SMSPostCmd_7(void);

	virtual DWORD Key() const { return key_; }

	virtual BYTE Code() const { return 7; }

	virtual void PostProcess(SMSTransiever& transiever);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSPostCmd>(*this);
		ar & key_;
	}

private:
	DWORD key_;

	friend class SMSPostCmd;
	friend class boost::serialization::access;
};

class SMSPostCmd_N : public SMSPostCmd
{
	SMSPostCmd_N();
public:
	~SMSPostCmd_N(void);

	virtual DWORD Key() const { return 0; }

	virtual BYTE Code() const { return 99; }

	virtual void PostProcess(SMSTransiever& transiever);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSPostCmd>(*this);
	}

private:

	friend class SMSPostCmd;
	friend class boost::serialization::access;
};

class SMSPostCmd_STOP : public SMSPostCmd
{
	SMSPostCmd_STOP();
public:
	~SMSPostCmd_STOP(void);

	virtual DWORD Key() const { return 0; }

	virtual BYTE Code() const { return 100; }

	virtual void PostProcess(SMSTransiever& transiever);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		//ar & boost::serialization::base_object<SMSPostCmd>(*this);
	}

private:

	friend class SMSPostCmd;
	friend class boost::serialization::access;
};