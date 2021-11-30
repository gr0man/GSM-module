#include "StdAfx.h"
#include "SMS.h"

#include "SMSPostCmd.h"
#include "SMSUnsendCmd.h"

#include "../../ControlData/ControlData/Logger.h"

/*
 *	Конструктор по-умолчанию.
 *	Нужен для сериализации.
 */
SMS::SMS()
: locale_(_create_locale(LC_CTYPE, "Russian"))
, postCmd_(0)
, unsendCmd_(0)
{
}

/*
 *	Формирует сообщение из массива данных, полученного от модема.
 */
SMS::SMS(const char* data)
: data_(data)
, locale_(_create_locale(LC_CTYPE, "Russian"))
, postCmd_(0)
, unsendCmd_(0)
{
	size_t addrLen;
	// извлекаем длину адреса сервис-центра
	char sLen[3];
	sLen[0] = data[0];
	sLen[1] = data[1];
	sLen[2] = 0;
	char* endptr;
	addrLen = strtol(sLen, &endptr, 16);

	// отсчитываем смещение до адреса отправителя
	size_t transAddrCntr = (1+addrLen+1)*2;
	sLen[0] = data[transAddrCntr];
	sLen[1] = data[transAddrCntr+1];
	addrLen = strtol(sLen, &endptr, 16);
	if( addrLen % 2 )
		addrLen++;

	// схема кодирования (DCS)
	char sdcs[3];
	BYTE dcs;
	size_t sdcsCntr = transAddrCntr+2+2+addrLen+2;
	sdcs[0] = data[sdcsCntr];
	sdcs[1] = data[sdcsCntr+1];
	sdcs[2] = 0;
	dcs = strtol(sdcs, &endptr, 16);

	// смещение до размера PDU-текста.
	size_t pduCntr = transAddrCntr+2+2+addrLen+2+2+7*2; // тип адреса, адрес, PID, DSC, Timestamp
	sLen[0] = data[pduCntr];
	sLen[1] = data[pduCntr+1];
	addrLen = strtol(sLen, &endptr, 16);

	std::string tmpData(data+pduCntr+2);
	if( dcs == 0x08 ) // USC2
		ConvertFromUCS2(tmpData, text_);
	else //if( dcs == 0x00 ) // 7 bit
		ConvertFrom7bit(tmpData, text_);

#ifdef USE_LOG
	logger.Add("rec SMS", text_.c_str());
#endif
}

/*
 *	Формирует сообщения для записи в модем и последующей отправки.
 *	номер телефона - 10-значный (без +7)
 */
SMS::SMS(const char* phone, const char* text, SMSPostCmd* postCmd, SMSUnsendCmd* unsendCmd)
: phone_(std::string("7")+phone) // добавляем "7" в начало номера
, text_(text)
, locale_(_create_locale(LC_CTYPE, "Russian"))
, postCmd_(postCmd)
, unsendCmd_(unsendCmd)
{
	//if( phone_.length() != 11 )
	//	throw 1;

	// адрес сервис-центра: длина адреса установлена в 0,
	// чтобы использовался адрес, прошитый в SIM-карте.
	data_.append("00");

	// параметры SMS, длина и формат номера адресата.
	data_.append("11000B91");

	if( phone_.length() % 2 )
		phone_.append("F"); // дополнение номера до четного количества цифр

	size_t i;
	for(i=0; i<phone_.length(); i+=2)
	{
		char tmp[3];
		tmp[0] = phone_[i+1];
		tmp[1] = phone_[i];
		tmp[2] = 0;
		data_.append(tmp);
	}

	// удаляем дополнение
	if( phone_.at(phone_.length()-1) == 'F' )
		phone_.erase(phone_.length(), 1);

	// PID
	data_.append("00");
	// DCS
	data_.append("08");
	// timestamp
	data_.append("17"); // 2 часа


	// кодирование сообщения в UTF16.
	std::string msgText;
	ConvertToUCS2(text, msgText);

	// длина данных сообщения
	char sLen[3];
	sprintf_s(sLen, 3, "%02X", msgText.length()/2);
	data_.append(sLen);

	// само сконвертированное сообщение
	data_.append(msgText);

	// окончание сообщения
	//data_.append(1, char(0x1A));
}

SMS::~SMS(void)
{
	_free_locale(locale_);
	if( postCmd_ )
	{
		delete postCmd_;
		postCmd_ = 0;
	}

	if( unsendCmd_ )
	{
		delete unsendCmd_;
		unsendCmd_ = 0;
	}
}

void SMS::DeletePostCmd() 
{
	if( postCmd_ )
	{
		delete postCmd_; 
		postCmd_ = 0;
	}
}

// конвертирует текстовую строку однобайтовых символов в UCS2 (UTF16) вариант (1 символ представлен 4-мя текстовыми полуоктетами)
void SMS::ConvertToUCS2(const std::string& src, std::string& dst)
{
	wchar_t buffer[1000];
	size_t converted = 0;
	// 1. Конвертация в UTF16
	_mbstowcs_s_l(&converted, buffer, 1000, src.c_str(), src.length(), locale_);
	
	// 2. преобразование в текстовую строку
	dst.clear();
	std::wstring bufStr(buffer);
	size_t len = bufStr.length();
	size_t i;
	for(i=0; i<len; ++i)
	{
		char buf[10];
		sprintf_s(buf, 10, "%04X", (unsigned short)bufStr.at(i));
		dst.append(buf);
	}
}

// конвертирует строку, представленную USC2 (UTF16) символами в текстовую строку однобайтовых символов.
void SMS::ConvertFromUCS2(const std::string& src, std::string& dst)
{
	std::wstring wstr;
	size_t len = src.length();
	size_t i;
	size_t cntr = 0;
	for(i=0; i<len/4; ++i)
	{
		char* endptr;
		char tmpStr[5];
		tmpStr[0] = src.at(i*4);
		tmpStr[1] = src.at(i*4 + 1);
		tmpStr[2] = src.at(i*4 + 2);
		tmpStr[3] = src.at(i*4 + 3);
		tmpStr[4] = 0;
		wchar_t symb = static_cast<wchar_t>(strtol(tmpStr, &endptr, 16));
		wstr.append(1, symb);
	}

	char buffer[1000];
	size_t converted;

	//_locale_t loc = _create_locale(LC_CTYPE, "Russian");

	converted = _wcstombs_l(buffer, wstr.c_str(), wstr.length()+1, locale_);

	dst = std::string(buffer);

	//_free_locale(loc);
}

// конвертирует строку в 7-битной кодировке в текстовую строку однобайтных символов.
void SMS::ConvertFrom7bit(const std::string& src, std::string& dst)
{
	char buffer[1000];
	size_t i;
	char* endptr;
	char crSym[3];
	crSym[2] = 0;
	crSym[0] = src[0];
	crSym[1] = src[1];
	BYTE sym = strtol(crSym, &endptr, 16);
	buffer[0] = sym & 0x7F;
	BYTE lastSym = 0;//(sym >> 7) & 0x01;
	size_t cntr = 0;
	for(i=0; i<src.length()/2; ++i)
	{

		crSym[0] = src[i*2];
		crSym[1] = src[i*2+1];

		sym = strtol(crSym, &endptr, 16);
	
		buffer[cntr++] = ( sym << (i%7) | lastSym) & 0x7F;

		if( (i+1)%7 == 0 )
		{
			buffer[cntr++] = (sym>>1) & 0x7F;
			lastSym = (sym >> 7) & 0x01;
		}
		else
			lastSym = (sym >> (7-i%7));
	}

	buffer[cntr] = 0;

	dst = std::string(buffer);

}

template<class Archive>
void SMS::serialize(Archive & ar, const unsigned int)
{
	ar.register_type(static_cast<SMSPostCmd_0*>(NULL));
	ar.register_type(static_cast<SMSPostCmd_1*>(NULL));
	ar.register_type(static_cast<SMSPostCmd_2*>(NULL));
	ar.register_type(static_cast<SMSPostCmd_6*>(NULL));
	ar.register_type(static_cast<SMSPostCmd_7*>(NULL));
	ar.register_type(static_cast<SMSPostCmd_STOP*>(NULL));

	ar & data_;
	ar & phone_;
	ar & text_;
	ar & postCmd_;
	ar & unsendCmd_;
}

///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <windows.h>
#include <fstream>

#include "SMSPostCmdImpl.h"

void SMS_test()
{
	bool completed = true;

	std::cout << "SMS test..." << std::endl;

	char readSmsData[] = "07919730071111F1040B919730664755F100083110515175336110041F0440043504320435043400320021";
	SMS recSms(readSmsData);
	// сравнение текста
	// ...
	char readSmsText[] = "Превед2!";
	if( strcmp(readSmsText, recSms.Text()) != 0 )
	{
		std::cout << "error build SMS from receive data" << std::endl;
		completed = false;
	}

	// данные в 7-битной кодировке
	char readSmsData7bit[] = "07919730071111F1040B919730664755F10000313050411541611AB1582CC6C2B16230584C36A3E55839D8CC66BBD16AB518";
	SMS recSms7bit(readSmsData7bit);
	char readSmsText7bit[] = "1111,8,10012349,9036674551";
	if( strcmp(readSmsText7bit, recSms7bit.Text()) != 0 )
	{
		std::cout << "error build SMS from receive 7 bit data" << std::endl;
		completed = false;
	}


	try
	{
		SMSPostCmd* postCmd = SMSPostCmd::Create(100, 5, -8, 0);
		SMSUnsendCmd* unsendCmd = new SMSUnsendCmd(1, 100, 5, 8);
		SMS sendSms("9036674551", "Превед!", postCmd, unsendCmd);
		char sendSmsData[] = "0011000B919730664755F10008170E041F044004350432043504340021";
		//sendSmsData[strlen(sendSmsData)-1] = 0x1A;
		// сравнение массива данных для передачи
		// ...
		if( strcmp(sendSmsData, sendSms.Data()) != 0 )
		{
			std::cout << "error build SMS from phone and text:" << std::endl;
			std::cout << "sms: " << sendSms.Data() << std::endl;
			std::cout << "cmd: " << sendSmsData << std::endl;
			completed = false;

			std::fstream log;
			log.open("d:\\projects\\AccessControl_projects\\sms_test.txt", std::ios_base::out);
			log << "sms: " << sendSms.Data() << std::endl;
			log << "cmd: " << sendSmsData << std::endl;
			log.close();

			Sleep(10000);
		}

		// проверка сериализации SMS
		std::fstream ofs;
		ofs.open("d:\\projects\\AccessControl_projects\\sms.txt", std::ios_base::out);
		boost::archive::text_oarchive oa(ofs);
		oa << sendSms;
		ofs.close();

		SMS newSms("");
		ofs.open("d:\\projects\\AccessControl_projects\\sms.txt", std::ios_base::in);
		boost::archive::text_iarchive ia(ofs);
		ia >> newSms;
		ofs.close();

		size_t i;
		for(i=0; i<strlen(sendSms.Data()); ++i)
		{
			if( sendSms.Data()[i] != newSms.Data()[i] )
				completed = false;
		}

		SMSPostCmd* postCmd1 = newSms.PostCmd();
		SMSUnsendCmd* unsendCmd1 = newSms.UnsendCmd();


		if( completed )
			std::cout << "TEST COMPLETE!" << std::endl;
		else
			std::cout << "TEST FAILED!" << std::endl;	
	
	}
	catch(std::exception& exc)
	{
		std::cout << exc.what() << std::endl;
	}


}