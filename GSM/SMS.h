#pragma once



#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include <boost/serialization/string.hpp>

class SMSPostCmd;
class SMSUnsendCmd;

/**	»нкапсулирует массив данных SMS дл€ передачи
 *	и извлечение информации из прин€того массива.
 */
class SMS
{	
	SMS();
public:

	SMS(const char* data);
	SMS(const char* phone, const char* text, SMSPostCmd* postCmd, SMSUnsendCmd* unsendCmd = 0);
	~SMS(void);
	
	const char* Address() const { return phone_.c_str(); }
	const char* Text() const { return text_.c_str(); }

	const char* Data() const { return data_.c_str(); }

	SMSPostCmd* PostCmd() { return postCmd_; }
	SMSUnsendCmd* UnsendCmd() { return unsendCmd_; }

	void DeletePostCmd();

private:
	// последовательность байт дл€ передачи в GSM-модем
	std::string data_;

	std::string phone_;
	std::string text_;

	_locale_t locale_;

	SMSPostCmd* postCmd_;
	SMSUnsendCmd* unsendCmd_;

	void ConvertToUCS2(const std::string& src, std::string& dst);
	void ConvertFromUCS2(const std::string& src, std::string& dst);
	void ConvertFrom7bit(const std::string& src, std::string& dst);

	
	template<class Archive>
	void serialize(Archive & ar, const unsigned int);
	//{
	//	ar & data_;
	//	ar & phone_;
	//	ar & text_;
	//	ar & postCmd_;
	//	ar & unsendCmd_;
	//}
	

    friend std::ostream & operator<<(std::ostream &os, const SMS &sms);
    friend class boost::serialization::access;
};
