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
double hPa_pressure, mmHg_pressure, temperature, humidity;

static int numOfCounting;
double sumHpaPressure, sumHgPressure, sumTemperature, sumHumidity, temp_avg;
unsigned long delayTime;
int resetPin=7;

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

    // Serial.println(numOfCounting);
    temp_avg = sumTemperature / numOfCounting;
    
    doc["Temperature"] = temp_avg; //round(temp_avg*10)/10;

    JsonArray data = doc.createNestedArray("Pressure");

    data.add(round(sumHpaPressure / numOfCounting));

    data.add(round(sumHgPressure / numOfCounting));
    doc["Humidity"] = round(sumHumidity / numOfCounting);
    serializeJsonPretty(doc, Serial);
    Serial.println();
    Serial.flush();
}
double roundValue(int value, int roundDigit = 1)
{
    return (int(value * pow(10,roundDigit))) / pow(10, roundDigit);
    // return 0;
}
void loop()
{
    // Serial.println(roundValue(14.35,1));
    printValues();
    delay(delayTime);
}