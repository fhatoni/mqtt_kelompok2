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
const char* mqtt_topic_sub = "kelompok2/dht";     // Menerima data dari publisher
const char* mqtt_topic_pub = "kelompok2/relay";   // Mengirim kontrol relay

WiFiClient espClient;
PubSubClient client(espClient);

// ==== LCD 16x2 (alamat I2C bisa 0x27 atau 0x3F tergantung modul) ====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==== Touch pin (sentuh jari) ====
#define TOUCH_PIN1 32  // Kontrol relay 1
#define TOUCH_PIN2 33  // Kontrol relay 2
int touchThreshold = 40;

// ==== Variabel Global ====
float suhu = 0, kelembapan = 0;
bool relay1 = false, relay2 = false;

// ==== Koneksi WiFi ====
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

// ==== Callback (menerima pesan dari publisher) ====
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("\nPesan diterima [");
  Serial.print(topic);
  Serial.print("] ");

  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (String(topic) == mqtt_topic_sub) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
      Serial.print("Gagal parsing JSON: ");
      Serial.println(error.c_str());
      return;
    }

    suhu = doc["temperature"];
    kelembapan = doc["humidity"];

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
void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    String clientId = "ESP32Subscriber-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Terhubung ke broker!");
      client.subscribe(mqtt_topic_sub);
      Serial.print("Subscribed ke topik: ");
      Serial.println(mqtt_topic_sub);
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

// ==== Loop utama ====
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // ==== Baca touch sensor ====
  int touch1 = touchRead(TOUCH_PIN1);
  int touch2 = touchRead(TOUCH_PIN2);

  // Sentuh untuk toggle Relay 1
  if (touch1 < touchThreshold) {
    relay1 = !relay1;
    StaticJsonDocument<100> doc;
    doc["relay1"] = relay1 ? 1 : 0;
    doc["relay2"] = relay2 ? 1 : 0;
    char buffer[100];
    serializeJson(doc, buffer);
    client.publish(mqtt_topic_pub, buffer);
    Serial.println("Relay 1 diubah lewat touch");
    delay(500);  // debounce
  }

  // Sentuh untuk toggle Relay 2
  if (touch2 < touchThreshold) {
    relay2 = !relay2;
    StaticJsonDocument<100> doc;
    doc["relay1"] = relay1 ? 1 : 0;
    doc["relay2"] = relay2 ? 1 : 0;
    char buffer[100];
    serializeJson(doc, buffer);
    client.publish(mqtt_topic_pub, buffer);
    Serial.println("Relay 2 diubah lewat touch");
    delay(500);  // debounce
  }
}
