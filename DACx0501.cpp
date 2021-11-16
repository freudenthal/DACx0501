#include "DACx0501.h"

const uint8_t DACx0501::DefaultAddress = 72; //B10010000
const uint16_t DACx0501::I2CTimeout =  1000;

const uint8_t DACx0501::NOOPCommand = 0x0;
const uint8_t DACx0501::DEVIDCommand = 0x1;
const uint8_t DACx0501::SYNCCommand = 0x2;
const uint8_t DACx0501::CONFIGCommand = 0x3;
const uint8_t DACx0501::GAINCommand = 0x4;
const uint8_t DACx0501::TRIGGERCommand = 0x5;
const uint8_t DACx0501::STATUSCommand = 0x7;
const uint8_t DACx0501::DACDATACommand = 0x8;

const uint8_t DACx0501::CONFIG_RefPower = 8;
const uint8_t DACx0501::CONFIG_Power = 0;

const uint8_t DACx0501::GAIN_RefDiv = 8;
const uint8_t DACx0501::GAIN_BuffGain = 0;

const uint8_t DACx0501::TRIGGER_LDAC = 4;
const uint8_t DACx0501::TRIGGER_ResetCode = 10;

const float DACx0501::VRefInt = 2.5;

DACx0501::DACx0501(uint8_t I2CAddress)
{
	Address = I2CAddress;
}

DACx0501::DACx0501()
{
	Address = DefaultAddress;
}

DACx0501::~DACx0501()
{

}

uint8_t DACx0501::getAddress()
{
	return Address;
}

void DACx0501::setAddress(uint8_t address)
{
	Address = address;
}

bool DACx0501::isConnected()
{
	int Status = 5;
	Wire.beginTransmission(Address);
	Status = Wire.endTransmission();
	if (Status == 0)
	{
		return true;
	}	
	else
	{
		return false;
	}
}

void DACx0501::useVRefInt(bool EnableVrefInt)
{
	UseVRefInt = EnableVrefInt;
	CommandByte = CONFIGCommand;
	ConfigReg = 0;
	bitWrite(ConfigReg, CONFIG_RefPower, !UseVRefInt);
	SendI2C();
}

void DACx0501::setOutput(float OutputSetting)
{
	setOutput( (uint16_t)(OutputSetting / getVRef() * 65535) );
}

void DACx0501::setOutput(uint16_t OutputSetting)
{
	CommandByte = DACDATACommand;
	ConfigReg = OutputSetting;
	SendI2C();
}

float DACx0501::getVRef()
{
	if (UseVRefInt)
	{
		return VRefInt;
	}
	else
	{
		return getVRefExt();
	}
}

void DACx0501::setVRefExt(float VRef)
{
	VRefExt = VRef;
}

float DACx0501::getVRefExt()
{
	return VRefExt;
}

void DACx0501::SendI2C()
{
	union UInt16tToUInt8tDataConverter
	{
		uint16_t UIntLargeData;
		uint8_t UIntSmallData[2];
	};
	UInt16tToUInt8tDataConverter TempData;
	TempData.UIntLargeData = ConfigReg;
	bool MoveOn = false;
	const int MaxAttempts = 16;
	int CurrentAttempt = 0;
	int SendSuccess = 5;
	while (!MoveOn)
	{
		Wire.beginTransmission(Address);
		Wire.write(CommandByte);
		Wire.write(TempData.UIntSmallData[1]);
		Wire.write(TempData.UIntSmallData[0]);
		SendSuccess = Wire.endTransmission(I2C_STOP, I2CTimeout);
		if(SendSuccess != 0)
		{
			Wire.finish();
			Wire.resetBus();
			CurrentAttempt++;
			if (CurrentAttempt > MaxAttempts)
			{
				MoveOn = true;
				Serial.println("Unrecoverable I2C transmission error with DACx0501.");
			}
		}
		else
		{
			MoveOn = true;
		}
	}
}

uint16_t DACx0501::ReceiveI2C()
{
	union UInt16tToUInt8tDataConverter
	{
		uint16_t UIntLargeData;
		uint8_t UIntSmallData[2];
	};
	UInt16tToUInt8tDataConverter TempData;
	TempData.UIntLargeData = 0;
	bool MoveOn = false;
	const int MaxAttempts = 16;
	int CurrentAttempt = 0;
	int SendSuccess = 5;
	while (!MoveOn)
	{
		Wire.beginTransmission(Address);
		Wire.write(CommandByte);
		SendSuccess = Wire.requestFrom(Address, 2, I2C_STOP, I2CTimeout);
		TempData.UIntSmallData[1] = Wire.readByte();
		TempData.UIntSmallData[0] = Wire.readByte();
		Wire.endTransmission(I2C_STOP, I2CTimeout);
		if(SendSuccess != 0)
		{
			Wire.finish();
			Wire.resetBus();
			CurrentAttempt++;
			if (CurrentAttempt > MaxAttempts)
			{
				MoveOn = true;
				Serial.println("Unrecoverable I2C transmission error with DACx0501.");
			}
		}
		else
		{
			MoveOn = true;
		}
	}
	return TempData.UIntLargeData;
}