#ifndef DACx0501_h
#define DACx0501_h

#include "Arduino.h"
#include "Wire.h"

class DACx0501
{
	public:
		DACx0501(TwoWire* I2CBus, uint8_t address); //Invoke with DACx0501(&WireN)
		DACx0501(uint8_t address);
		DACx0501();
		~DACx0501();
		bool isConnected();
		uint8_t getAddress();
		void setAddress(uint8_t address);
		float getVRef();
		void setVRefExt(float VRef);
		float getVRefExt();
		void useVRefInt(bool EnableVrefInt);
		void setOutput(float OutputSetting);
		void setOutput(uint16_t OutputSetting);
	private:
		static const uint8_t DefaultAddress;
		static const uint16_t I2CTimeout;
		static const uint8_t NOOPCommand;
		static const uint8_t DEVIDCommand;
		static const uint8_t SYNCCommand;
		static const uint8_t CONFIGCommand;
		static const uint8_t GAINCommand;
		static const uint8_t TRIGGERCommand;
		static const uint8_t STATUSCommand;
		static const uint8_t DACDATACommand;
		static const uint8_t CONFIG_RefPower;
		static const uint8_t CONFIG_Power;
		static const uint8_t GAIN_RefDiv;
		static const uint8_t GAIN_BuffGain;
		static const uint8_t TRIGGER_LDAC;
		static const uint8_t TRIGGER_ResetCode;
		static const float VRefInt;
		TwoWire *WireBus;
		uint8_t Address;
		uint8_t CommandByte;
		uint16_t ConfigReg;
		float VRefExt;
		bool UseVRefInt;
		void SendI2C();
		uint16_t ReceiveI2C();
};

#endif