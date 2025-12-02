#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const int analogInPin = A0; 
const int Relay = D2; 

int sensorValue = 0;
int outputValue = 0;

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

String webhook_url = "";

bool notified = false;  

void setup() {
  Serial.begin(9600);
  pinMode(Relay, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void sendDiscord(String msg) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(webhook_url);

    http.addHeader("Content-Type", "application/json");

    String payload = "{\"content\":\"" + msg + "\"}";
    http.POST(payload);
    http.end();
  }
}

void loop() {
  sensorValue = analogRead(analogInPin);
  outputValue = map(sensorValue, 0, 1023, 100, 0);

  Serial.print(outputValue);
  Serial.println(" %");

  if (outputValue <= 40) {
    digitalWrite(Relay, HIGH);

    if (!notified) {
      sendDiscord("⚠️ ความชื้นต่ำกว่า 40% — กำลังเปิดปั๊มน้ำ 💧");
      notified = true;
    }
  }
  else {
    digitalWrite(Relay, LOW);

    if (notified) {
      sendDiscord("✅ ความชื้นกลับมาปกติ — ปิดปั๊มน้ำแล้ว 🌿");
      notified = false;
    }
  }

  delay(500);
}
