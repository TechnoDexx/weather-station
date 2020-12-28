#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ArduinoJson.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

//Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI
double hPa_pressure{0}, mmHg_pressure{0}, temperature{0}, humidity{0};

static int numOfCounting{0};
double sumHpaPressure{0}, sumHgPressure{0}, sumTemperature{0}, sumHumidity{0}, temp_avg{0};
unsigned long delayTime;
int resetPin{7};

double roundValue(int,int);

void setup()
{
    pinMode(resetPin,OUTPUT);
    digitalWrite(resetPin,HIGH);
    
    Serial.begin(9600);
    // Serial.println(F("BME280 test"));

    bool status;

    // default settings
    status = bme.begin();
    if (!status)
    {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        Serial.println();
        while (1);
    }

    // Serial.println("-- Default Test --");
    delayTime = 5000;

    Serial.println();

    delay(100); // let sensor boot up
}

void printValues()
{
    StaticJsonDocument<256> doc;
    while (numOfCounting < 10){
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    hPa_pressure = bme.readPressure() / 100.0F;
    mmHg_pressure = hPa_pressure * 0.750064;
    sumTemperature += temperature;
    sumHumidity += humidity;
    sumHgPressure += mmHg_pressure;
    sumHpaPressure += hPa_pressure;

    ++numOfCounting;
    }

    
    temp_avg = sumTemperature / numOfCounting;
    
    doc["Temperature"] = temp_avg; 
    doc["Pressure"] = round(sumHgPressure / numOfCounting);
    doc["Humidity"] = round(sumHumidity / numOfCounting);
    serializeJson(doc, Serial);
    Serial.println();
    Serial.flush();
}

void loop()
{
    // Serial.println(roundValue(14.35,1));
    printValues();
    delay(delayTime);
}