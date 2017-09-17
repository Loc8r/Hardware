#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <String.h>

#define SERVER_PHONE_NUMBER "+16475287299" //incl. country code

#define GPSECHO  false

SoftwareSerial SoftwareSerialGPRS(7, 8);
SoftwareSerial SoftwareSerialGPS(12, 11);

Adafruit_GPS GPS(&SoftwareSerialGPS);

uint32_t timer = millis();
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

							// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
	char c = GPS.read();
}

void useInterrupt(boolean v) {
	if (v) {
		// Timer0 is already used for millis() - we'll just interrupt somewhere
		// in the middle and call the "Compare A" function above
		OCR0A = 0xAF;
		TIMSK0 |= _BV(OCIE0A);
		usingInterrupt = true;
	}
	else {
		// do not call the interrupt function COMPA anymore
		TIMSK0 &= ~_BV(OCIE0A);
		usingInterrupt = false;
	}
}

void setup()
{
	SoftwareSerialGPRS.begin(19200);               // the GPRS baud rate   
	Serial.begin(19200);    //must match the GPRS baud rate
	delay(500);
	Serial.println("GPRS INITIALIZED");

	GPS.begin(9600);
	GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
	GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
	useInterrupt(true);
	delay(1000);
	// Ask for firmware version
	SoftwareSerialGPS.println(PMTK_Q_RELEASE);
	Serial.println("GPS INITIALIZED");
	updateLocation();
}

void loop()
{
	if (!usingInterrupt) {
		// read data from the GPS in the 'main loop'
		char c = GPS.read();
		// if you want to debug, this is a good time to do it!
		if (GPSECHO)
			if (c) Serial.print(c);
	}

	// if a sentence is received, we can check the checksum, parse it...
	if (GPS.newNMEAreceived()) {
		// a tricky thing here is if we print the NMEA sentence, or data
		// we end up not listening and catching other sentences!
		// so be very wary if using OUTPUT_ALLDATA and trytng to print out data
		//Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

		if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
			return;  // we can fail to parse a sentence in which case we should just wait for another
	}

	// if millis() or timer wraps around, we'll just reset it
	if (timer > millis())
	{
		timer = millis();
	}
	if (millis() - timer > 15L * 1000) { //15 seconds 
		
		Serial.print(millis());
		Serial.print('\t');
		Serial.print(timer);
		Serial.print('\t');
		Serial.print(millis() - timer);
		Serial.print('\t');
		
		/*Serial.print(GPS.latitude);
		Serial.print('\t');
		Serial.print(GPS.longitude);
		Serial.print('\t');*/
		Serial.print(GPS.latitudeDegrees);
		Serial.print('\t');
		Serial.println(GPS.longitudeDegrees);
		Serial.print('\t');
		/*Serial.print(GPS.altitude);
		Serial.print('\t');
		Serial.print(GPS.angle);
		Serial.print('\t');
		Serial.print(GPS.day);
		Serial.print('\t');
		Serial.print(GPS.month);
		Serial.print('\t');
		Serial.println(GPS.year);*/

		//
		updateLocation();

	}
	if (SoftwareSerialGPRS.available())
	{
		Serial.write(SoftwareSerialGPRS.read());
	}
}

void updateLocation() {
	String location;
	if (GPS.fix)
	{
		location = String(GPS.latitudeDegrees, 4) + "," + String(GPS.longitudeDegrees, 4);
	}
	else
	{
		location = "SHIT, SHIT";
	}
	sendTextMessage(location);
	timer = millis(); // reset the timer
}

void sendTextMessage(String text)
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
