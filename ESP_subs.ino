#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

// ==== Konfigurasi WiFi ====
const char* ssid = "POCO M3 Pro 5G";
const char* password = "00000000";

// ==== Konfigurasi MQTT ====
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_topic_sub = "kelompok2/dht";     // menerima data dari publisher
const char* mqtt_topic_pub = "kelompok2/relay";   // mengirim kontrol relay

WiFiClient espClient;
PubSubClient client(espClient);

// ==== LCD ====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==== Touch Pin ====
#define TOUCH_PIN1 32
#define TOUCH_PIN2 33
int touchThreshold = 40;

// ==== Variabel ====
float suhu = 0, kelembapan = 0;
bool relay1 = false, relay2 = false;
bool prevTouch1 = false, prevTouch2 = false;

// ==== Setup WiFi ====
void setup_wifi() {
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
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) message += (char)payload[i];

  if (String(topic) == mqtt_topic_sub) {
    StaticJsonDocument<200> doc;
    if (deserializeJson(doc, message)) return;

    suhu = doc["temperature"];
    kelembapan = doc["humidity"];

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("T:%.1fC H:%.1f%%", suhu, kelembapan);
    lcd.setCursor(0, 1);
    lcd.printf("R1:%s R2:%s", relay1 ? "ON " : "OFF", relay2 ? "ON" : "OFF");
  }
}

// ==== Reconnect MQTT ====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    String clientId = "ESP32Subscriber-" + String(random(0xffff), HEX);
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

// ==== Setup ====
void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// ==== Loop ====
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  int t1 = touchRead(TOUCH_PIN1);
  int t2 = touchRead(TOUCH_PIN2);

  bool isTouched1 = (t1 < touchThreshold);
  bool isTouched2 = (t2 < touchThreshold);

  // === Relay 1 toggle ===
  if (isTouched1 && !prevTouch1) {
    relay1 = !relay1;
    StaticJsonDocument<100> doc;
    doc["relay1"] = relay1 ? 1 : 0;
    doc["relay2"] = relay2 ? 1 : 0;
    char buffer[100];
    serializeJson(doc, buffer);
    client.publish(mqtt_topic_pub, buffer);
    Serial.printf("Relay1 %s\n", relay1 ? "ON" : "OFF");
    delay(200);  // debounce
  }

  // === Relay 2 toggle ===
  if (isTouched2 && !prevTouch2) {
    relay2 = !relay2;
    StaticJsonDocument<100> doc;
    doc["relay1"] = relay1 ? 1 : 0;
    doc["relay2"] = relay2 ? 1 : 0;
    char buffer[100];
    serializeJson(doc, buffer);
    client.publish(mqtt_topic_pub, buffer);
    Serial.printf("Relay2 %s\n", relay2 ? "ON" : "OFF");
    delay(200);  // debounce
  }

  // Simpan status sentuhan sebelumnya
  prevTouch1 = isTouched1;
  prevTouch2 = isTouched2;
}
