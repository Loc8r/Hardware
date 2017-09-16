#include <SoftwareSerial.h>
#include <String.h>

#define SERVER_PHONE_NUMBER "+16472164936" //incl. country code

SoftwareSerial SoftwareSerialGPRS(7, 8);

void setup()
{
	SoftwareSerialGPRS.begin(19200);               // the GPRS baud rate   
	Serial.begin(19200);    //must match the GPRS baud rate
	delay(500);
	Serial.println("GPRS INITIALIZED");
}

void loop()
{
	//after start up the program, you can using terminal to connect the serial of gprs shield,
	//if you input 't' in the terminal, the program will execute SendTextMessage(), it will show how to send a sms message,
	//if input 'd' in the terminal, it will execute DialVoiceCall(), etc.

	if (Serial.available())
	{
		Serial.println("Command Received");
		switch (Serial.read())
		{
		case 't':
			SendTextMessage("LOOOOL TESTING AT 4:30 AM");
			break;
		}

		if (SoftwareSerialGPRS.available())
		{
			Serial.write(SoftwareSerialGPRS.read());
		}
	}
}

void SendTextMessage(String text)
{
	SoftwareSerialGPRS.print("AT+CMGF=1\r");    //SMS in text mode
	delay(100);
	SoftwareSerialGPRS.print("AT + CMGS = \"");//send sms message,
	delay(100);
	SoftwareSerialGPRS.print(SERVER_PHONE_NUMBER);
	delay(100);
	SoftwareSerialGPRS.println("\"");
	delay(100);
	SoftwareSerialGPRS.println(text);//contents of msg
	delay(100);
	SoftwareSerialGPRS.println((char)26);//the ASCII code of the ctrl+z is 26
	delay(100);
	SoftwareSerialGPRS.println();
}
