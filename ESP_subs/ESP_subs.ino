#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

// ==== Konfigurasi WiFi ====
const char *ssid = "POCO M3 Pro 5G";
const char *password = "00000000";

// ==== Konfigurasi MQTT ====
const char *mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char *mqtt_topic_sub = "kelompok2/dht";   // Menerima data suhu/kelembapan
const char *mqtt_topic_pub = "kelompok2/relay"; // Mengirim status relay

WiFiClient espClient;
PubSubClient client(espClient);

// ==== LCD 16x2 ====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==== Touch pin ====
#define TOUCH_PIN1 32 // Toggle Relay 1
#define TOUCH_PIN2 33 // Toggle Relay 2
#define TOUCH_PIN3 13 // Toggle Relay 1 (threshold 50)
#define TOUCH_PIN4 14 // Toggle Relay 2 (threshold 50)
int touchThreshold = 40;
int touchThresholdExtra = 50;

// ==== Variabel Global ====
float suhu = 0, kelembapan = 0;
bool relay1 = false, relay2 = false;

// ==== Fungsi koneksi WiFi ====
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi terkoneksi!");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());
}

// ==== Callback MQTT ====
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("\nPesan diterima [");
  Serial.print(topic);
  Serial.print("] ");

  String message;
  for (int i = 0; i < length; i++)
    message += (char)payload[i];
  Serial.println(message);

  // --- Jika menerima data dari topik relay ---
  if (String(topic) == mqtt_topic_pub)
  {
    StaticJsonDocument<200> doc;
    if (deserializeJson(doc, message))
      return;

    relay1 = doc["relay1"];
    relay2 = doc["relay2"];

    // Update tampilan LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("T:%.1fC H:%.1f%%", suhu, kelembapan);
    lcd.setCursor(0, 1);
    lcd.printf("R1:%s R2:%s", relay1 ? "ON " : "OFF", relay2 ? "ON" : "OFF");

    Serial.printf("Relay sinkron -> R1:%s R2:%s\n", relay1 ? "ON" : "OFF", relay2 ? "ON" : "OFF");
  }

  // --- Jika menerima data dari topik sensor (publisher DHT) ---
  if (String(topic) == mqtt_topic_sub)
  {
    StaticJsonDocument<200> doc;
    if (deserializeJson(doc, message))
      return;

    suhu = doc["temperatureRtu"];
    kelembapan = doc["humidityRtu"];

    // Tampilkan di LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("T:%.1fC H:%.1f%%", suhu, kelembapan);
    lcd.setCursor(0, 1);
    lcd.printf("R1:%s R2:%s", relay1 ? "ON " : "OFF", relay2 ? "ON" : "OFF");

    Serial.printf("Suhu: %.1f°C | Kelembapan: %.1f%%\n", suhu, kelembapan);
  }
}

// ==== Reconnect MQTT ====
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Menghubungkan ke MQTT...");
    String clientId = "ESP32Device-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str()))
    {
      Serial.println("Terhubung ke broker!");
      client.subscribe(mqtt_topic_sub);
      client.subscribe(mqtt_topic_pub);
      Serial.println("Subscribed ke topik sensor dan relay");
    }
    else
    {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

// ==== Kirim status relay terbaru ke broker ====
void publishRelayStatus()
{
  StaticJsonDocument<100> doc;
  doc["relay1"] = relay1 ? 1 : 0;
  doc["relay2"] = relay2 ? 1 : 0;
  char buffer[100];
  serializeJson(doc, buffer);
  client.publish(mqtt_topic_pub, buffer);
}

// ==== Setup ====
void setup()
{
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// ==== Loop utama ====
void loop()
{
  if (!client.connected())
    reconnect();
  client.loop();

  int t1 = touchRead(TOUCH_PIN1);
  int t2 = touchRead(TOUCH_PIN2);
  int t3 = touchRead(TOUCH_PIN3);
  int t4 = touchRead(TOUCH_PIN4);

  // --- Toggle Relay 1 via TOUCH_PIN1 ---
  if (t1 < touchThreshold)
  {
    relay1 = !relay1;
    publishRelayStatus();
    Serial.println("Relay 1 diubah via TOUCH_PIN1");
    delay(500);
  }

  // --- Toggle Relay 2 via TOUCH_PIN2 ---
  if (t2 < touchThreshold)
  {
    relay2 = !relay2;
    publishRelayStatus();
    Serial.println("Relay 2 diubah via TOUCH_PIN2");
    delay(500);
  }

  // --- Toggle Relay 1 via TOUCH_PIN3 (threshold 50) ---
  if (t3 < touchThresholdExtra)
  {
    relay1 = !relay1;
    publishRelayStatus();
    Serial.println("Relay 1 diubah via TOUCH_PIN3");
    delay(500);
  }

  // --- Toggle Relay 2 via TOUCH_PIN4 (threshold 50) ---
  if (t4 < touchThresholdExtra)
  {
    relay2 = !relay2;
    publishRelayStatus();
    Serial.println("Relay 2 diubah via TOUCH_PIN4");
    delay(500);
  }
}
