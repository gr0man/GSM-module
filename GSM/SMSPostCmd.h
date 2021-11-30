#pragma once

#include <windows.h>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/assume_abstract.hpp>

class SMSTransiever;

/**	������� ����������� �����, ���������������
 *	��������� SMS-������� ����� �������� ��������
 *	SMS-���������.
 */
class SMSPostCmd
{
public:
	// 0 � 1
	static SMSPostCmd* Create(DWORD key, time_t smsTime, int potential, BYTE flag);
	// 2
	static SMSPostCmd* Create(DWORD key);
	// 6 � 7
	static SMSPostCmd* Create(DWORD key, BYTE numb);
	// ���������
	static SMSPostCmd* Create();
	//
	static SMSPostCmd* CreateStop();

	virtual void PostProcess(SMSTransiever& transiever) = 0;

	virtual DWORD Key() const = 0;

	virtual BYTE Code() const = 0;

	SMSPostCmd(void);	
	virtual ~SMSPostCmd(void);

private:

	template<class Archive>
	void serialize(Archive & ar, const unsigned int)
	{
	}


    friend std::ostream & operator<<(std::ostream &os, const SMSPostCmd &cmd);
    friend class boost::serialization::access;

};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(SMSPostCmd)