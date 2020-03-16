#include "config.h"
#include "main.h"

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>

#include <Wire.h>
#include "MPU9250.h"
#include <Adafruit_BMP280.h>

#include <ArduinoJson.h>

#include "setup.h"

#define dtMotion 5*1000
#define dtTemperature 1000
#define dtPressure 1000
#define dtStatus 10*1000

unsigned long timerMotion = 0;
unsigned long timerTemperature = 0;
unsigned long timerPressure = 0;
unsigned long timerStatus = 0;

bool mpu9255Status = false;
bool bmp280Status = false;

WiFiClient espClient;
PubSubClient client(espClient);

MPU9250 mpu(Wire,0x68);
Adafruit_BMP280 bmp;

void reconnect();
void callback(char * topic, byte* payload, unsigned int length);
void publishStatus(bool reset = false);

void reconnect()
{
	digitalWrite(1, LOW);

	//if (client.connect(controllerName, mqttLogin, mqttPasswd))
	if (client.connect(controllerName))
	{
		Serial.println("MQTT is connected");
		client.subscribe("drs/r1/reset");
	}
}

void callback(char * topic, byte* payload, unsigned int length)
{
	if(strcmp(topic,"drs/r1/reset")==0)
	{
		if((char)payload[0] == 'r')
		{
			publishStatus(true);
			delay(500);

			ESP.restart();
		}
	}
}

void publishStatus(bool reset)
{

	String status = "ok";

	if(reset == true)
	{
		status = "reset";
	}

	DynamicJsonDocument json(1024);

	json["main"] = status;

	if(mpu9255Status)
	{
		json["mpu9255"] = status;
	}

	if(bmp280Status)
	{
		json["bmp280"] = status;
	}

	String output;
	serializeJson(json, output);
	//Serial.println(output);

	client.publish("drs/r1/status", output.c_str());
}

void setup()
{
	Serial.begin(115200);

	WIFIsetup();
	OTAsetup();
	client.setServer(mqttIP, mqttPort);
	client.setCallback(callback);
	reconnect();

        Wire.begin(D1, D2);

        while(!Serial) {}

        if (mpu.begin() < 0)
        {
                Serial.println("MPU9255 initialization unsuccessful");
                Serial.println("Check IMU wiring or try cycling power");
                while(1) {}
        }
	else
	{
		mpu9255Status = true;
	}

        if (!bmp.begin())
        {
                Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
                while (1) {};
        }
	else
	{
		bmp280Status = true;
	}

        bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2, Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16, Adafruit_BMP280::STANDBY_MS_500);

	publishStatus();
}

void loop()
{
	if (!client.connected())
	{
		reconnect();
	}
	else
	{
		client.loop();
		ArduinoOTA.handle();
	}

	int dt = micros() - timerMotion;

	if(dt > dtMotion)
	{
		timerMotion = micros();

		DynamicJsonDocument json(1024);

	        mpu.readSensor();

	        JsonArray jsonAccelArray = json.createNestedArray("accel");
	        jsonAccelArray.add(mpu.getAccelX_mss());
		jsonAccelArray.add(mpu.getAccelY_mss());
		jsonAccelArray.add(mpu.getAccelZ_mss());

	        JsonArray jsonGyroArray = json.createNestedArray("gyro");
	        jsonGyroArray.add(mpu.getGyroX_rads());
	        jsonGyroArray.add(mpu.getGyroY_rads());
	        jsonGyroArray.add(mpu.getGyroZ_rads());

	        JsonArray jsonMagArray = json.createNestedArray("mag");
	        jsonMagArray.add(mpu.getMagX_uT());
	        jsonMagArray.add(mpu.getMagY_uT());
	        jsonMagArray.add(mpu.getMagZ_uT());

	        json["dt"] = dt;

	        String output;
	        serializeJson(json, output);
	        //Serial.println(output);

	        client.publish("drs/r1/sensors/motion", output.c_str());
	}

	dt = millis() - timerTemperature;

	if(dt > dtTemperature)
	{
		timerTemperature = millis();

		DynamicJsonDocument json(1024);

		json["tempMPU9255"] = mpu.getTemperature_C();
		json["tempBMP280"] = bmp.readTemperature();

	        json["dt"] = dt;

	        String output;
	        serializeJson(json, output);
	        //Serial.println(output);

	        client.publish("drs/r1/sensors/temperature", output.c_str());
	}

	dt = millis() - timerPressure;

	if(dt > dtPressure)
	{
		timerPressure = millis();

		DynamicJsonDocument json(1024);

		json["pressure"] = bmp.readPressure()/100;

	        json["dt"] = dt;

	        String output;
	        serializeJson(json, output);
	        //Serial.println(output);

	        client.publish("drs/r1/sensors/pressure", output.c_str());
	}

	dt = millis() - timerStatus;

	if(dt > dtStatus)
	{
		timerStatus = millis();

		publishStatus();
	}
}
