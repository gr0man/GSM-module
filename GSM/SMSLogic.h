#pragma once


/**	Интерфейс класса, реализующего логику
 *	обработки SMS-команд.
 */
class SMSLogic
{
public:

	virtual void IncUnSMS()=0;
	virtual void IncOkSMS()=0;
	virtual void IncTmpSMS()=0;

	virtual WORD GetPassSMS()=0;
	virtual time_t GetTSMS()=0;
	virtual BYTE GetSMSpr()=0;
	
	virtual void AddToControl(DWORD key, time_t Time, BYTE flag, int potential)=0;
	//virtual void ChangeInputRegistration(DWORD key, time_t crTime)=0;

	virtual void AddToUnsendedLog(const char* text)=0;
	virtual void AddToSuspendedLog(DWORD key)=0;
	virtual void RemoveFromSuspendedLog(DWORD key)=0;
	virtual int GetLastPotentialFromControl(DWORD key)=0;
	virtual void AddToSendedLog(const char* text)=0;
	virtual const char* GetLastTime(DWORD key)=0;

	virtual time_t GetUrgency(DWORD key)=0;
	virtual void SetUrgency(DWORD key, time_t newUrgency)=0;
	
	virtual void GetCTs(DWORD key, time_t* ct1, time_t* ct2)=0;
	virtual int GetPotential(DWORD key)=0;
	virtual void SetPotential(DWORD key, int newPotential)=0;

	virtual const char* GetName(DWORD key)=0;
	
	virtual void SetPhone(DWORD key, const char* phone)=0;
	virtual const char* GetPhone(DWORD key)=0;

	virtual const char* GetPhone() const=0;
	virtual void SetPhone(const char* phone)=0;

	virtual const char* GetText0() const=0;
	virtual const char* GetText1() const=0;
	virtual const char* GetText2() const=0;
	virtual const char* GetText3() const=0;


	virtual bool EthernetState()=0;
	virtual bool WebCamState()=0;
	virtual BYTE LockDeviceState()=0;

	virtual void ErrorSMS()=0;

	virtual void SetHaltSystem()=0;

	virtual void GSMErrorMessage(const char* msg) = 0;

	//virtual time_t GetLastIncomingTime(DWORD key) = 0;
	//virtual time_t GetLastOutgoingTime(DWORD key) = 0;

	virtual time_t GetLastTimeFromControl(DWORD key) = 0;

};