
#include <time.h>

#include "SMSCommand.h"


class SMSCommand_0 : public SMSCommand
{	
	SMSCommand_0(time_t smsTime, DWORD key, BYTE flag);
	SMSCommand_0();
public:
	virtual ~SMSCommand_0();

	virtual BYTE Code() const { return 0; }
	virtual bool Execute(SMSTransiever& transiever);

	BYTE Flag() const { return flag_; }

	void SetSmsTime(time_t smsTime){ SMSCommand::SetSmsTime(smsTime); }

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
		ar & flag_;
	}

private:
	BYTE flag_;

	friend class SMSCommand;
	friend class boost::serialization::access;
};


class SMSCommand_1 : public SMSCommand
{
	SMSCommand_1(time_t smsTime, DWORD key, BYTE flag);
	SMSCommand_1();
public:	
	virtual ~SMSCommand_1();

	virtual BYTE Code() const { return 1; }
	virtual bool Execute(SMSTransiever& transiever);

	BYTE Flag() const { return flag_; }

	void SetSmsTime(time_t smsTime){ SMSCommand::SetSmsTime(smsTime); }

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
		ar & flag_;
	}

private:
	BYTE flag_;

	friend class SMSCommand;
	friend class boost::serialization::access;
};


class SMSCommand_2 : public SMSCommand
{
	SMSCommand_2(DWORD key, const char* phone);
	SMSCommand_2();
public:	
	virtual ~SMSCommand_2();

	virtual BYTE Code() const { return 2; }
	virtual bool Execute(SMSTransiever& transiever);

	const char* Phone() const { return phone_.c_str(); }

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
		ar & phone_;
	}

private:
	std::string phone_;

	friend class SMSCommand;
	friend class boost::serialization::access;
};


class SMSCommand_3 : public SMSCommand
{
	SMSCommand_3(time_t smsTime, const char* text);
	SMSCommand_3();
public:	
	virtual ~SMSCommand_3();

	virtual BYTE Code() const { return 3; }
	virtual bool Execute(SMSTransiever& transiever);

	const char* Text() const { return text_.c_str(); }

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
		ar & text_;
	}

private:
	std::string text_;

	friend class SMSCommand;
	friend class boost::serialization::access;
};


class SMSCommand_4 : public SMSCommand
{
	SMSCommand_4(time_t smsTime, DWORD key, unsigned int addPotent);
	SMSCommand_4();
public:
	virtual ~SMSCommand_4();

	virtual BYTE Code() const { return 4; }
	virtual bool Execute(SMSTransiever& transiever);

	unsigned int AddPotential() const { return addPotent_; }

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
		ar & addPotent_;
	}

private:
	unsigned int addPotent_;

	friend class SMSCommand;
	friend class boost::serialization::access;
};


class SMSCommand_5 : public SMSCommand
{
	SMSCommand_5(time_t smsTime, DWORD key, const char* phone, unsigned int addParam);
	SMSCommand_5();
public:	
	virtual ~SMSCommand_5();

	virtual BYTE Code() const { return 5; }
	virtual bool Execute(SMSTransiever& transiever);

	unsigned int AddParam() const { return addParam_; }
	const char* Phone() const { return phone_.c_str(); }

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
		ar & addParam_ & phone_;
	}

private:
	unsigned int addParam_;
	std::string phone_;

	friend class SMSCommand;
	friend class boost::serialization::access;
};


class SMSCommand_6 : public SMSCommand
{
	SMSCommand_6(time_t smsTime, DWORD key);
	SMSCommand_6();
public:
	virtual ~SMSCommand_6();

	virtual BYTE Code() const { return 6; }
	virtual bool Execute(SMSTransiever& transiever);

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
	}

private:

	friend class SMSCommand;
	friend class boost::serialization::access;
};


class SMSCommand_7 : public SMSCommand
{
	SMSCommand_7(time_t smsTime, DWORD key);
	SMSCommand_7();
public:
	virtual ~SMSCommand_7();

	virtual BYTE Code() const { return 7; }
	virtual bool Execute(SMSTransiever& transiever);

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
	}

private:

	friend class SMSCommand;
	friend class boost::serialization::access;
};


class SMSCommand_8 : public SMSCommand
{
	SMSCommand_8(time_t smsTime, DWORD key, const char* phone);
	SMSCommand_8();
public:
	virtual ~SMSCommand_8();

	virtual BYTE Code() const { return 8; }
	virtual bool Execute(SMSTransiever& transiever);

	const char* Phone() const { return phone_.c_str(); }

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
		ar & phone_;
	}

private:
	std::string phone_;

	friend class SMSCommand;
	friend class boost::serialization::access;
};


class SMSCommand_9 : public SMSCommand
{
	SMSCommand_9(time_t smsTime, DWORD key);
	SMSCommand_9();
public:
	virtual ~SMSCommand_9();

	virtual BYTE Code() const { return 9; }
	virtual bool Execute(SMSTransiever& transiever);

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
	}

private:

	friend class SMSCommand;
	friend class boost::serialization::access;
};


class SMSCommand_10 : public SMSCommand
{
	SMSCommand_10(time_t smsTime, DWORD key, time_t newUrgency);
	SMSCommand_10();
public:
	virtual ~SMSCommand_10();

	virtual BYTE Code() const { return 10; }
	virtual bool Execute(SMSTransiever& transiever);

	time_t Urgency() const { return newUrgency_; }

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
		ar & newUrgency_;
	}

private:
	time_t newUrgency_;

	friend class SMSCommand;
	friend class boost::serialization::access;
};


class SMSCommand_11 : public SMSCommand
{
	SMSCommand_11(time_t smsTime, const char* phone);
	SMSCommand_11();
public:
	virtual ~SMSCommand_11();

	virtual BYTE Code() const { return 11; }
	virtual bool Execute(SMSTransiever& transiever);

	const char* Phone() const { return phone_.c_str(); }

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
		ar & phone_;
	}

private:
	std::string phone_;

	friend class SMSCommand;
	friend class boost::serialization::access;
};


class SMSCommand_12 : public SMSCommand
{
	SMSCommand_12(time_t smsTime, const char* text);
	SMSCommand_12();
public:
	virtual ~SMSCommand_12();

	virtual BYTE Code() const { return 12; }
	virtual bool Execute(SMSTransiever& transiever);

	const char* Text() const { return text_.c_str(); }

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
		ar & text_;
	}

private:
	std::string text_;

	friend class SMSCommand;
	friend class boost::serialization::access;
};

class SMSCommand_13 : public SMSCommand
{
	SMSCommand_13(const char* phone);
	SMSCommand_13();
public:
	virtual ~SMSCommand_13();

	virtual BYTE Code() const {return 13;}
	virtual bool Execute(SMSTransiever& transiever);

	const char* Phone() const { return phone_.c_str(); }

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
		ar & phone_;
	}

private:
	std::string phone_;

	friend class SMSCommand;
	friend class boost::serialization::access;
};

class SMSCommand_16 : public SMSCommand
{
	SMSCommand_16();
public:
	virtual ~SMSCommand_16(){};

	virtual BYTE Code() const { return 16; }
	virtual bool Execute(SMSTransiever& transiever);

	size_t GetText(char* buffer, size_t bufLen);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SMSCommand>(*this);
	}

private:

	friend class SMSCommand;
	friend class boost::serialization::access;
};