#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "noteconectes";
const char* password = "12345678";
const char* host = "api.github.com";
const int httpsPort = 443;

const int redLED = 4;
const int yellowLED = 5;
const int greenLED = 19;

void setup() {
  Serial.begin(115200);

  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.github.com/repos/sgrofranco/SemaforoIC/commits/main/status");
    http.addHeader("Authorization", "token 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");

    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        // Procesa el JSON
        parseJson(payload);
      }
    } else {
      Serial.printf("[HTTP] GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }

  delay(5000); // Espera 5 segundos antes de realizar la siguiente solicitud
}

void parseJson(String json) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json);

  String status = doc["state"];
  Serial.println("Estado de integración continua: " + status);

  if (status == "success") {
    // Encender LED verde y apagar los demás
    digitalWrite(greenLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
  } else if (status == "failure") {
    // Encender LED rojo y apagar los demás
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, HIGH);
  } else if (status == "pending") {
    // Encender LED amarillo y apagar los demás
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, HIGH);
    digitalWrite(redLED, LOW);
  } else {
    // Apagar todos los LEDs
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
  }
}
