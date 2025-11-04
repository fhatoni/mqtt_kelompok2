/*
  Sistem Monitoring Kualitas Air (Suhu, pH, TDS)
  Menggunakan ESP32 dengan WiFi, MQTT, Kontrol Relay, dan Web OTA Update
  
  Membaca data dari:
  - Sensor Suhu DS18B20 (Pin 26)
  - Sensor pH (Analog, Pin 35)
  - Sensor TDS (Analog, Pin 34)
  - Sensor DHT11 (Pin 27)
  
  Mengirim data ke MQTT:
  - Topic: hydro/sensors
  
  Menerima data dari MQTT:
  - Topic: hydro/relay (untuk kontrol manual Pin 23)
  - Topic: hydro/tds (untuk pengaturan min/max PPM & mode otomatis)
  
  Menyediakan Halaman Web:
  - http://hydromonitor.local/ (Halaman Login)
  - http://hydromonitor.local/update (Halaman upload firmware OTA)
  
  Menampilkan data ke Serial Monitor dan LCD I2C 16x2.
*/

// --- Library ---
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "GravityTDS.h" // Pastikan library ini terinstal
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include <WebServer.h>     
#include <Update.h>      
#include <ESPmDNS.h>    

#include "webpages.h"    

// --- Pengaturan WiFi ---
const char* WIFI_SSID = "ThinkpadP50";
const char* WIFI_PASSWORD = "thinkpad";
const char* host = "hydromonitor"; // Nama mDNS (akses di http://hydromonitor.local)

// --- Pengaturan MQTT ---
const char* MQTT_BROKER = "broker.emqx.io";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC_SENSORS = "hydro/sensors";
const char* MQTT_TOPIC_RELAY = "hydro/relay";
const char* MQTT_TOPIC_TDS = "hydro/tds"; // <-- TOPIK DIGANTI

WiFiClient espClient;
PubSubClient mqtt(espClient);
WebServer server(80); // Objek Web Server di port 80

// --- Pengaturan Relay ---
const int relayPin = 23; // Pin untuk pompa (Aktif HIGH)

// --- VARIABEL BARU UNTUK OTOMASI ---
bool autoMode = false;     // Mulai dalam mode otomatis
float ppmMin = 400.0; // Nilai default PPM minimum
float ppmMax = 600.0; // Nilai default PPM maksimum

// --- Pengaturan LCD ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Pengaturan Sensor Suhu (DS18B20) ---
const int oneWireBus = 26;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// --- Pengaturan Sensor TDS ---
#define TdsSensorPin 34
#define EEPROM_SIZE 512
GravityTDS gravityTds;
float temperature, tdsValue;

// --- Pengaturan Sensor pH (DIROMBAK NON-BLOCKING) ---
#define PhSensorPin 35
int buffer_arr[34], temp;
float ph;
int phBufferIndex = 0; // Index untuk buffer pH
unsigned long lastPhSampleTime = 0; // Timer untuk pembacaan pH

// --- Pengaturan Sensor DHT11 ---
#define DHTPIN 27
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float airHumidity, airTemp;

// --- Pengaturan Timer Non-Blocking ---
unsigned long lastSensorRead = 0;   // Timer untuk DS18B20, TDS, DHT
unsigned long lastMqttPublish = 0;
unsigned long lastLcdUpdate = 0;

const long sensorReadInterval = 3000;
const long mqttPublishInterval = 5000;
const long lcdUpdateInterval = 2500;
const long phSampleInterval = 30; // Baca 1 sampel pH setiap 30ms

int lcdScreenState = 0;
bool isOtaAuthenticated = false; // <-- VARIABEL BARU UNTUK STATUS LOGIN

// --- Halaman Web OTA Login & Update ---
// --- KODE HTML SEKARANG DIPINDAHKAN KE "webpages.h" ---
// Anda harus membuat file "webpages.h" di folder yang sama
// dan memasukkan variabel PAGE_Login dan PAGE_Update ke dalamnya.


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

// --- FUNGSI pH DIROMBAK ---
// Fungsi ini hanya memproses buffer setelah penuh
void processPhBuffer()
{
    unsigned long int avgval = 0; 

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
    for (int i = 2; i < 32; i++)
    {
        avgval += buffer_arr[i];
    }
    avgval = avgval / 30;
    
    // Konversi nilai analog ke pH
    ph = (avgval - 5201.9) / -276.04;
    
    Serial.println("Buffer pH selesai diproses.");
}
// --- AKHIR FUNGSI pH ---


// Fungsi membaca sensor TDS
void tdsSensor()
{
    // Sensor TDS memerlukan suhu untuk kompensasi
    gravityTds.setTemperature(temperature);
    gravityTds.update();
    tdsValue = gravityTds.getTdsValue();
}

// Fungsi SENSOR DHT11
void dhtSensor() {
  airHumidity = dht.readHumidity();
  airTemp = dht.readTemperature(); // Baca sebagai Celsius

  // Cek jika pembacaan gagal (sensor terputus / error)
  if (isnan(airHumidity) || isnan(airTemp)) {
    Serial.println("Error: Gagal membaca sensor DHT11!");
    airHumidity = 0;
    airTemp = 0;
  }
}

// --- FUNGSI BARU UNTUK WIFI & MQTT ---

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(WIFI_SSID);
  lcd.clear();
  lcd.print("WiFi Connect...");
  lcd.setCursor(0, 1);
  lcd.print(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi terhubung");
  Serial.println("Alamat IP: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.print("WiFi OK");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(1500);
}

// Fungsi callback saat ada pesan MQTT masuk
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan diterima di topic: ");
  Serial.println(topic);

  // Buat buffer untuk payload
  char payloadBuffer[length + 1];
  memcpy(payloadBuffer, payload, length);
  payloadBuffer[length] = '\0';
  Serial.println(payloadBuffer);

  // Proses jika topic adalah hydro/relay (KONTROL MANUAL)
  if (strcmp(topic, MQTT_TOPIC_RELAY) == 0) {
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, payloadBuffer);

    if (error) {
      Serial.print("deserializeJson() gagal: ");
      Serial.println(error.c_str());
      return;
    }

    if (doc.containsKey("relay_1")) {
      bool relay1_status = doc["relay_1"]; 
      
      // --- MODIFIKASI ---
      // Perintah manual menonaktifkan mode otomatis
      // autoMode = false;
      Serial.println("Mode Otomatis: OFF (Kontrol Manual Aktif)");
      // --- AKHIR MODIFIKASI ---
      
      if (relay1_status == true) {
        Serial.println("MQTT: Menyalakan Relay 1 (Pin 23)");
        digitalWrite(relayPin, HIGH); // Aktif HIGH
      } else {
        Serial.println("MQTT: Mematikan Relay 1 (Pin 23)");
        digitalWrite(relayPin, LOW); // Aktif HIGH
      }
    }

    if (doc.containsKey("relay_2")) {
      bool mode_status = doc["relay_2"]; 
      
      // --- MODIFIKASI ---
      // Perintah manual menonaktifkan mode otomatis
      autoMode = mode_status;
    }
  }
  
  // --- BLOK DIMODIFIKASI ---
  // Proses jika topic adalah hydro/tds (KONTROL OTOMATIS)
  else if (strcmp(topic, MQTT_TOPIC_TDS) == 0) {
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, payloadBuffer);

    if (error) {
      Serial.print("deserializeJson() gagal: ");
      Serial.println(error.c_str());
      return;
    }

    // Ambil nilai "min" dan "max" jika ada
    // Gunakan nilai lama (atau default) jika key tidak ada
    if (doc.containsKey("min")) {
      ppmMin = doc["min"];
    }
    if (doc.containsKey("max")) {
      ppmMax = doc["max"];
    }

    // Menerima pesan di topic ini otomatis mengaktifkan autoMode
    autoMode = true; 

    Serial.println("--- Pengaturan TDS Otomatis Diterima ---");
    Serial.println("Mode Otomatis: ON");
    Serial.print("PPM Min: "); Serial.println(ppmMin);
    Serial.print("PPM Max: "); Serial.println(ppmMax);
    Serial.println("---------------------------------");
  }
  // --- AKHIR BLOK MODIFIKASI ---
}

// Fungsi untuk koneksi ulang MQTT
void reconnect_mqtt() {
  while (!mqtt.connected()) {
    Serial.print("Mencoba koneksi MQTT...");
    lcd.clear();
    lcd.print("MQTT Connect...");
    
    // Buat client ID acak
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if (mqtt.connect(clientId.c_str())) {
      Serial.println("terhubung");
      lcd.clear();
      lcd.print("MQTT OK");
      delay(1000);
      
      // Subscribe ke topic relay
      mqtt.subscribe(MQTT_TOPIC_RELAY);
      Serial.print("Subscribe ke: ");
      Serial.println(MQTT_TOPIC_RELAY);
      
      // --- MODIFIKASI ---
      // Subscribe ke topic TDS BARU
      mqtt.subscribe(MQTT_TOPIC_TDS);
      Serial.print("Subscribe ke: ");
      Serial.println(MQTT_TOPIC_TDS);
      // --- AKHIR MODIFIKASI ---
      
    } else {
      Serial.print("gagal, rc=");
      Serial.print(mqtt.state());
      Serial.println(" coba lagi dalam 5 detik");
      lcd.clear();
      lcd.print("MQTT Gagal");
      lcd.setCursor(0, 1);
      lcd.print("Coba lagi...");
      delay(5000); // Tunggu 5 detik sebelum mencoba lagi
    }
  }
}

// Fungsi untuk publish data sensor ke MQTT
void publishSensorData() {
  StaticJsonDocument<256> doc; // Buat JSON dokumen

  // Masukkan data sensor
  doc["ph_water"] = ph;
  doc["tds_water"] = tdsValue;
  doc["water_temp"] = temperature;

  // Menggunakan data dari sensor DHT11
  doc["air_temp"] = airTemp;
  doc["air_humidity"] = airHumidity;

  char buffer[256];
  serializeJson(doc, buffer); // Ubah JSON ke String

  Serial.print("Mengirim data ke MQTT: ");
  Serial.println(buffer);
  
  mqtt.publish(MQTT_TOPIC_SENSORS, buffer);
}

// Fungsi untuk update LCD secara non-blocking
void updateLcdDisplay() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Suhu: " + String(temperature, 1) + " C"); 
    
    if (lcdScreenState == 0) {
      lcd.setCursor(0, 1);
      lcd.print("Ph: " + String(ph, 1));
      lcdScreenState = 1; // Ganti state
    } else if (lcdScreenState == 1) { 
      lcd.setCursor(0, 1);
      lcd.print("TDS: " + String(tdsValue, 0));
      lcdScreenState = 2; // Ganti state
    } else if (lcdScreenState == 2) { 
      lcd.setCursor(0, 1);
      lcd.print(String(airTemp,0) + "C H:" + String(airHumidity,0) + "%");
      lcdScreenState = 3; 
    
    } else { // Ini adalah state 3
      lcd.setCursor(0, 1);
      lcd.print("Mode: ");
      // Tampilkan "Otomatis" atau "Manual" (tambah spasi agar 16 char)
      lcd.print(autoMode ? "Otomatis" : "Manual  "); 
      lcdScreenState = 0; // Kembali ke state 0
    }
}


// --- SETUP ---
void setup()
{
    Serial.begin(115200);
    
    // Inisialisasi Relay Pin
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW); // Mulai dengan relay MATI (asumsi active-HIGH)
    
    // Inisialisasi Sensor Suhu
    sensors.begin();
    
    // Inisialisasi Sensor DHT11
    dht.begin();
    
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
    delay(1000);

    // Hubungkan WiFi
    setup_wifi();

    // Hubungkan MQTT
    mqtt.setServer(MQTT_BROKER, MQTT_PORT);
    mqtt.setCallback(mqtt_callback);

    // --- SETUP WEB SERVER OTA BARU ---
    if (MDNS.begin(host)) {
      Serial.print("mDNS responder dimulai. Akses di: http://");
      Serial.print(host);
      Serial.println(".local");
    }

    // Arahkan halaman utama (/) ke halaman login
    server.on("/", HTTP_GET, []() {
      if (isOtaAuthenticated) {
        // Jika sudah login, alihkan ke halaman update
        server.sendHeader("Location", "/update");
        server.send(302, "text/plain", "Redirecting to /update");
      } else {
        // Kirim halaman login
        String page = PAGE_Login;
        // Cek jika ada query error
        if (server.hasArg("error")) {
          page.replace("%LOGIN_ERROR_CLASS%", "status-error");
          page.replace("%LOGIN_ERROR_STYLE%", "display: block;");
          page.replace("%LOGIN_ERROR_MSG%", "Username atau Password salah!");
        } else {
          page.replace("%LOGIN_ERROR_CLASS%", "");
          page.replace("%LOGIN_ERROR_STYLE%", "display: none;");
          page.replace("%LOGIN_ERROR_MSG%", "");
        }
        server.send(200, "text/html", page);
      }
    });

    // --- HANDLER BARU UNTUK PROSES LOGIN ---
    server.on("/login", HTTP_POST, []() {
      if (server.hasArg("username") && server.hasArg("password") &&
          server.arg("username") == "admin" && server.arg("password") == "admin") {
        isOtaAuthenticated = true;
        server.sendHeader("Location", "/update"); // Sukses, ke halaman update
        server.send(302, "text/plain", "Login OK. Redirecting...");
      } else {
        isOtaAuthenticated = false;
        server.sendHeader("Location", "/?error=1"); // Gagal, kembali ke login dengan pesan error
        server.send(302, "text/plain", "Login Gagal. Redirecting...");
      }
    });


    // Halaman untuk menampilkan form upload (SEKARANG DIPROTEKSI)
    server.on("/update", HTTP_GET, []() {
      if (!isOtaAuthenticated) {
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", "Not Authenticated. Redirecting...");
        return;
      }
      // Kirim halaman update, ganti placeholder
      String page = PAGE_Update;
      page.replace("%FREE_HEAP%", String(ESP.getFreeHeap()));
      server.send(200, "text/html", page);
    });

    // Halaman untuk menangani proses upload (SEKARANG DIPROTEKSI)
    server.on("/update", HTTP_POST, []() {
      if (!isOtaAuthenticated) {
        server.send(401, "text/plain", "NOT AUTHORIZED");
        return;
      }
      // Tampilkan pesan sukses dan restart
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "UPDATE GAGAL!" : "UPDATE BERHASIL! Restart...");
      ESP.restart();
    }, []() {
      // Fungsi ini menangani data file yang diupload
      if (!isOtaAuthenticated) { // Proteksi di setiap chunk upload
        return;
      }
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { // Mulai update
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { // Selesaikan update
          Serial.printf("Update Sukses: %u bytes\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
      }
    });
    
    server.begin(); // Mulai web server
    Serial.println("Server OTA HTTP dimulai.");
    lcd.clear();
    lcd.print("OTA Aktif");
    lcd.setCursor(0,1);
    lcd.print(WiFi.localIP());
    delay(2000);
}

// --- LOOP ---
void loop()
{
    // Cek koneksi WiFi
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Koneksi WiFi terputus!");
      setup_wifi(); // Coba hubungkan ulang
    }

    // Cek koneksi MQTT
    if (!mqtt.connected()) {
      reconnect_mqtt();
    }
    
    mqtt.loop(); // Wajib untuk MQTT
    server.handleClient(); // Wajib untuk Web Server

    // Dapatkan waktu saat ini
    unsigned long now = millis();

    // --- Timer 1: Baca Sensor (DS18B20, TDS, DHT) ---
    if (now - lastSensorRead > sensorReadInterval) {
      lastSensorRead = now;
      
      Serial.println("Membaca sensor (Suhu, TDS, DHT)...");
      tempSensor();
      tdsSensor(); // <-- tdsValue diperbarui di sini
      dhtSensor();

      // --- LOGIKA OTOMASI BARU ---
      if (autoMode) {
        Serial.print("Mode Otomatis AKTIF. Cek TDS: ");
        Serial.print(tdsValue);
        Serial.print(" (Range: ");
        Serial.print(ppmMin);
        Serial.print("-");
        Serial.print(ppmMax);
        Serial.println(")");

        // Logika Hysteresis (Deadband)
        // Pompa (relayPin) diasumsikan MENAMBAH PPM (nutrisi)

        // Jika TDS jatuh DI BAWAH minimum, nyalakan pompa
        if (tdsValue < ppmMin) {
          Serial.println("TDS di bawah minimum. Menyalakan Pompa.");
          digitalWrite(relayPin, HIGH);
          delay(1000);
          digitalWrite(relayPin, LOW);
        } 
        // Jika TDS naik DI ATAS maksimum, matikan pompa
        else if (tdsValue > ppmMax) {
          Serial.println("TDS di atas maksimum. Mematikan Pompa.");
          digitalWrite(relayPin, LOW);
        }
        // Jika TDS berada DI ANTARA min dan max, JANGAN LAKUKAN APA-APA.
        // Biarkan pompa dalam status terakhirnya (jika sedang ON, biarkan ON
        // sampai mencapai max. Jika sedang OFF, biarkan OFF sampai jatuh di bawah min)
        else {
          Serial.println("TDS dalam rentang. Tidak ada perubahan status pompa.");
        }
      }
      // --- AKHIR LOGIKA OTOMASI ---
    }

    // --- Timer 2: Kirim Data MQTT ---
    if (now - lastMqttPublish > mqttPublishInterval) {
      lastMqttPublish = now;
      
      Serial.println("Mengirim data MQTT...");
      publishSensorData();
    }

    // --- Timer 3: Update Tampilan LCD ---
    if (now - lastLcdUpdate > lcdUpdateInterval) {
      lastLcdUpdate = now;
      updateLcdDisplay();
    }

    // --- Timer 4: Pembacaan Sensor pH (Non-Blocking) ---
    // Cek apakah sudah waktunya mengambil sampel pH BERIKUTNYA
    if (now - lastPhSampleTime > phSampleInterval) {
      if (phBufferIndex < 34) { // Cek jika buffer belum penuh
        lastPhSampleTime = now; // Setel ulang timer sampel
        buffer_arr[phBufferIndex] = analogRead(PhSensorPin); // Baca 1 sampel
        phBufferIndex++; // Pindahkan index
        
        // Jika buffer SEKARANG sudah penuh
        if (phBufferIndex == 34) {
          processPhBuffer(); // Olah datanya
          phBufferIndex = 0; // Siapkan untuk pengisian buffer berikutnya
        }
      }
    }
}

