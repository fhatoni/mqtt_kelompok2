#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <ModbusMaster.h>

// ==== WiFi ====
const char *ssid = "POCO M3 Pro 5G";
const char *password = "00000000";

// ==== MQTT ====
const char *mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char *mqtt_topic_sub = "kelompok2/relay"; // menerima perintah relay
const char *mqtt_topic_pub = "kelompok2/dht";   // kirim data suhu & kelembapan (biar kompatibel nama topiknya)

WiFiClient espClient;
PubSubClient client(espClient);

// ==== LCD 16x2 ====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==== Pin Push Button ====
#define BUTTON1 13
#define BUTTON2 14

// ==== Pin Relay ====
#define RELAY1 25
#define RELAY2 26

// ==== Variabel Global ====
float suhu = 0, kelembapan = 0;
bool relay1 = false, relay2 = false;
bool lastButton1 = HIGH, lastButton2 = HIGH;

// ==== Konfigurasi MODBUS (XY-MD02) ====
#define MODBUS_EN_PIN  4
#define MODBUS_RO_PIN 18
#define MODBUS_DI_PIN 19
#define MODBUS_SERIAL_BAUD 9600
#define MODBUS_PARITY SERIAL_8N1

#define MODBUS_SLAVE_ID 2
#define MODBUS_ADDRESS 0x0001
#define MODBUS_QUANTITY 2

ModbusMaster modbus;

void modbusPreTransmission() {
  digitalWrite(MODBUS_EN_PIN, HIGH);
  delay(2);
}

void modbusPostTransmission() {
  digitalWrite(MODBUS_EN_PIN, LOW);
  delay(2);
}

// ==== Fungsi koneksi WiFi ====
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi terkoneksi!");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());
}

// ==== Callback MQTT ====
void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("\nPesan MQTT diterima [");
  Serial.print(topic);
  Serial.print("] ");

  String message;
  for (int i = 0; i < length; i++)
    message += (char)payload[i];
  Serial.println(message);

  StaticJsonDocument<200> doc;
  if (deserializeJson(doc, message))
    return;

  if (String(topic) == mqtt_topic_sub) {
    relay1 = doc["relay1"];
    relay2 = doc["relay2"];
    digitalWrite(RELAY1, relay1);
    digitalWrite(RELAY2, relay2);
    publishRelayStatus(); // update LCD
  }
}

// ==== Reconnect MQTT ====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    String clientId = "ESP32Device-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Terhubung ke broker!");
      client.subscribe(mqtt_topic_sub);
    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

// ==== Kirim data suhu & kelembapan ====
void publishSensorData() {
  StaticJsonDocument<100> doc;
  doc["temperature"] = suhu;
  doc["humidity"] = kelembapan;

  char buffer[100];
  serializeJson(doc, buffer);
  client.publish(mqtt_topic_pub, buffer);

  Serial.printf("Data dikirim -> Suhu: %.1f°C, Kelembapan: %.1f%%\n", suhu, kelembapan);
}

// ==== Kirim status relay ke broker ====
void publishRelayStatus() {
  StaticJsonDocument<100> doc;
  doc["relay1"] = relay1 ? 1 : 0;
  doc["relay2"] = relay2 ? 1 : 0;
  char buffer[100];
  serializeJson(doc, buffer);
  client.publish("kelompok2/relayStatus", buffer); // topik status relay (boleh beda)

  // Update LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printf("T:%.1fC H:%.1f%%", suhu, kelembapan);
  lcd.setCursor(0, 1);
  lcd.printf("R1:%s R2:%s", relay1 ? "ON " : "OFF", relay2 ? "ON" : "OFF");
}

// ==== Baca data dari sensor Modbus ====
void readModbusSensor() {
  uint8_t result = modbus.readInputRegisters(MODBUS_ADDRESS, MODBUS_QUANTITY);

  if (result == modbus.ku8MBSuccess) {
    uint16_t rawTemp = modbus.getResponseBuffer(0x00);
    uint16_t rawHum  = modbus.getResponseBuffer(0x01);

    suhu = rawTemp / 10.0;
    kelembapan = rawHum / 10.0;

    Serial.printf("Suhu: %.1f°C | Kelembapan: %.1f%%\n", suhu, kelembapan);
  } else {
    Serial.println("❌ Gagal baca data Modbus!");
  }
}

// ==== Setup ====
void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);

  pinMode(MODBUS_EN_PIN, OUTPUT);
  digitalWrite(MODBUS_EN_PIN, LOW);

  // Setup Modbus
  Serial2.begin(MODBUS_SERIAL_BAUD, MODBUS_PARITY, MODBUS_RO_PIN, MODBUS_DI_PIN);
  modbus.begin(MODBUS_SLAVE_ID, Serial2);
  modbus.preTransmission(modbusPreTransmission);
  modbus.postTransmission(modbusPostTransmission);

  // Setup WiFi & MQTT
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  lcd.setCursor(0, 0);
  lcd.print("System Ready...");
  delay(1000);
}

// ==== Loop utama ====
void loop() {
  if (!client.connected())
    reconnect();
  client.loop();

  // Baca tombol
  bool currentButton1 = digitalRead(BUTTON1);
  bool currentButton2 = digitalRead(BUTTON2);

  if (lastButton1 == HIGH && currentButton1 == LOW) {
    relay1 = !relay1;
    digitalWrite(RELAY1, relay1);
    publishRelayStatus();
    Serial.println("Relay 1 diubah via BUTTON1");
    delay(300);
  }

  if (lastButton2 == HIGH && currentButton2 == LOW) {
    relay2 = !relay2;
    digitalWrite(RELAY2, relay2);
    publishRelayStatus();
    Serial.println("Relay 2 diubah via BUTTON2");
    delay(300);
  }

  lastButton1 = currentButton1;
  lastButton2 = currentButton2;

  // Baca sensor tiap 2 detik
  static unsigned long lastRead = 0;
  if (millis() - lastRead > 2000) {
    readModbusSensor();
    publishSensorData();
    publishRelayStatus();
    lastRead = millis();
  }
}
