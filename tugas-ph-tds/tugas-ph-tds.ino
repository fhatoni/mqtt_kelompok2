#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "GravityTDS.h"
#include <OneWire.h>
#include <DallasTemperature.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int oneWireBus = 26;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

#define TdsSensorPin 34
#define EEPROM_SIZE 512
GravityTDS gravityTds;
float temperature, tdsValue;

#define PhSensorPin 35
unsigned long int avgval;
int buffer_arr[34], temp;
float ph;

void led(String m1, String m2)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(m1);
    lcd.setCursor(0, 1);
    lcd.print(m2);
}

void tempSensor()
{
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);
}

void phSensor()
{
    for (int i = 0; i < 34; i++)
    {
        buffer_arr[i] = analogRead(PhSensorPin);
        delay(30);
    }
    for (int i = 0; i < 33; i++)
    {
        for (int j = i + 1; j < 34; j++)
        {
            if (buffer_arr[i] > buffer_arr[j])
            {
                temp = buffer_arr[i];
                buffer_arr[i] = buffer_arr[j];
                buffer_arr[j] = temp;
            }
        }
    }
    avgval = 0;
    for (int i = 2; i < 32; i++)
    {
        avgval += buffer_arr[i];
    }
    avgval = avgval / 30;
    ph = (avgval - 5201.9) / -276.04;
}

void tdsSensor()
{
    gravityTds.setTemperature(temperature);
    gravityTds.update();
    tdsValue = gravityTds.getTdsValue();
}

void setup()
{
    Serial.begin(115200);
    sensors.begin();
    EEPROM.begin(EEPROM_SIZE);
    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(3.3);
    gravityTds.setAdcRange(4096);
    gravityTds.begin();
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("ESP32 Start");
    delay(1000);
    lcd.clear();
}

void loop()
{
    tempSensor();
    phSensor();
    tdsSensor();
    Serial.print("Temperature : ");
    Serial.println(temperature);
    Serial.print("Ph : ");
    Serial.println(ph);
    Serial.print("Ppm : ");
    Serial.println(tdsValue);
    Serial.println("=============");
    led("Ph: " + String(ph), "Ppm: " + String(tdsValue));
    delay(1000);
}
