// --- Library ---
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "GravityTDS.h" // Pastikan library ini terinstal
#include <OneWire.h>
#include <DallasTemperature.h>

// --- Pengaturan LCD ---
LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat 0x27, 16 kolom, 2 baris

// --- Pengaturan Sensor Suhu (DS18B20) ---
const int oneWireBus = 26;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// --- Pengaturan Sensor TDS ---
#define TdsSensorPin 34
#define EEPROM_SIZE 512
GravityTDS gravityTds;
float temperature, tdsValue;

// --- Pengaturan Sensor pH ---
#define PhSensorPin 35
unsigned long int avgval;
int buffer_arr[34], temp;
float ph;

// Fungsi kustom untuk menampilkan ke LCD
void led(String m1, String m2)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(m1);
    lcd.setCursor(0, 1);
    lcd.print(m2);
}

// Fungsi membaca sensor suhu
void tempSensor()
{
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);

    // Cek jika sensor gagal dibaca
    if (temperature == DEVICE_DISCONNECTED_C) {
      Serial.println("Error: Sensor DS18B20 terputus!");
      temperature = 25.0; // Gunakan nilai default 25 C jika sensor error
    }
}

// Fungsi membaca sensor pH
void phSensor()
{
    // Mengisi buffer
    for (int i = 0; i < 34; i++)
    {
        buffer_arr[i] = analogRead(PhSensorPin);
        delay(30);
    }
    
    // Mengurutkan buffer (bubble sort) untuk membuang noise
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
    
    // Mengambil rata-rata dari nilai tengah
    avgval = 0;
    for (int i = 2; i < 32; i++)
    {
        avgval += buffer_arr[i];
    }
    avgval = avgval / 30;
    
    // Konversi nilai analog ke pH
    // CATATAN: Nilai kalibrasi Anda (5201.9 dan -276.04) terlihat
    // tidak wajar untuk ADC ESP32 (0-4095). 
    // Pastikan nilai ini sudah benar.
    ph = (avgval - 5201.9) / -276.04;
}

// Fungsi membaca sensor TDS
void tdsSensor()
{
    // Sensor TDS memerlukan suhu untuk kompensasi
    gravityTds.setTemperature(temperature);
    gravityTds.update();
    tdsValue = gravityTds.getTdsValue();
}

void setup()
{
    Serial.begin(115200);
    
    // Inisialisasi Sensor Suhu
    sensors.begin();
    
    // Inisialisasi TDS
    EEPROM.begin(EEPROM_SIZE);
    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(3.3); // Aref ESP32 adalah 3.3V
    gravityTds.setAdcRange(4096); // ADC ESP32 adalah 12-bit
    gravityTds.begin();
    
    // Inisialisasi LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Sistem Monitor");
    lcd.setCursor(0, 1);
    lcd.print("Mulai...");
    delay(2000);
}

void loop()
{
    // 1. Baca semua sensor
    tempSensor();
    phSensor(); // pH dibaca sebelum TDS, tapi TDS butuh suhu
    tdsSensor(); // TDS dibaca setelah suhu
    
    // 2. Tampilkan ke Serial Monitor
    Serial.print("Suhu: ");
    Serial.print(temperature);
    Serial.println(" *C");
    
    Serial.print("Nilai mentah pH (avg): ");
    Serial.println(avgval);
    Serial.print("Ph: ");
    Serial.println(ph);
    
    Serial.print("Ppm (TDS): ");
    Serial.println(tdsValue);
    Serial.println("=================");
    
    // 3. Tampilkan ke LCD
    // Menggunakan fungsi led() asli Anda
    // led("Ph: " + String(ph), "Ppm: " + String(tdsValue));
    
    // Rekomendasi: Tampilkan Suhu, lalu ganti-ganti pH dan TDS
    lcd.clear();
    lcd.setCursor(0, 0);
    // Tampilkan suhu dengan 1 angka desimal
    lcd.print("Suhu: " + String(temperature, 1) + " C"); 
    
    // Tampilkan pH di baris kedua
    lcd.setCursor(0, 1);
    lcd.print("Ph: " + String(ph, 1)); // Tampilkan 1 angka desimal
    delay(2500); // Tahan selama 2.5 detik
    
    // Ganti baris kedua dengan TDS
    lcd.setCursor(0, 1);
    lcd.print("Ppm: " + String(tdsValue, 0)); // Tampilkan 0 angka desimal
    lcd.print("        "); // Hapus sisa baris
    delay(2500); // Tahan selama 2.5 detik
}

