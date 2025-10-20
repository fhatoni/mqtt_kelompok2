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

    StaticJsonDocument<200> doc;
    if (deserializeJson(doc, message))
        return;

    if (String(topic) == mqtt_topic_pub)
    {
        relay1 = doc["relay1"];
        relay2 = doc["relay2"];
        digitalWrite(RELAY1, relay1);
        digitalWrite(RELAY2, relay2);
    }

    if (String(topic) == mqtt_topic_sub)
    {
        suhu = doc["temperature"];
        kelembapan = doc["humidity"];
    }

    // Update tampilan LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("T:%.1fC H:%.1f%%", suhu, kelembapan);
    lcd.setCursor(0, 1);
    lcd.printf("R1:%s R2:%s", relay1 ? "ON " : "OFF", relay2 ? "ON" : "OFF");
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

// ==== Kirim status relay ke broker ====
void publishRelayStatus()
{
    StaticJsonDocument<100> doc;
    doc["relay1"] = relay1 ? 1 : 0;
    doc["relay2"] = relay2 ? 1 : 0;
    char buffer[100];
    serializeJson(doc, buffer);
    client.publish(mqtt_topic_pub, buffer);

    // Update LCD juga
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("T:%.1fC H:%.1f%%", suhu, kelembapan);
    lcd.setCursor(0, 1);
    lcd.printf("R1:%s R2:%s", relay1 ? "ON " : "OFF", relay2 ? "ON" : "OFF");
}

// ==== Setup ====
void setup()
{
    Serial.begin(115200);
    lcd.init();
    lcd.backlight();

    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);
    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);

    digitalWrite(RELAY1, relay1);
    digitalWrite(RELAY2, relay2);

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

    bool currentButton1 = digitalRead(BUTTON1);
    bool currentButton2 = digitalRead(BUTTON2);

    // --- Tombol 1 ditekan ---
    if (lastButton1 == HIGH && currentButton1 == LOW)
    {
        relay1 = !relay1;
        digitalWrite(RELAY1, relay1);
        publishRelayStatus();
        Serial.println("Relay 1 diubah via BUTTON1");
        delay(300); // debouncing sederhana
    }

    // --- Tombol 2 ditekan ---
    if (lastButton2 == HIGH && currentButton2 == LOW)
    {
        relay2 = !relay2;
        digitalWrite(RELAY2, relay2);
        publishRelayStatus();
        Serial.println("Relay 2 diubah via BUTTON2");
        delay(300); // debouncing sederhana
    }

    lastButton1 = currentButton1;
    lastButton2 = currentButton2;
}
