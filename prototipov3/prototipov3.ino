#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ArduinoUnit.h>

const char* ssid = "ACA VA EL WIFI";
const char* password = "abcd1234";
const char* host = "api.github.com";
const int httpsPort = 443;

const int redLED = 4;
const int yellowLED = 2;
const int greenLED = 15;




void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    blinkYellowLED();
    Serial.println("Connecting to WiFi...");
  }
}

void makeHttpRequest() {
  HTTPClient http;
  http.begin("https://api.github.com/repos/sgrofranco/SemaforoIC/actions/runs?per_page=1&page=1");
  http.addHeader("Authorization", "token ACA VA EL TOKEN");

  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      parseJson(payload);
    } else {
      blinkYellowLED();
    }
  } else {
    Serial.printf("[HTTP] GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
    blinkYellowLED();
  }
  http.end();
}

void parseJson(String json) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json);

  JsonObject latestRun = doc["workflow_runs"][0];

  String conclusion = latestRun["conclusion"];
  Serial.println("Conclusión de la última ejecución: " + conclusion);

  if (conclusion == "success") {
    digitalWrite(greenLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
  } else if (conclusion == "failure") {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, HIGH);
  } else if (conclusion == "neutral" || conclusion == "null") {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, HIGH);
    digitalWrite(redLED, LOW);
  } else {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
  }
}

void blinkYellowLED() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(yellowLED, HIGH);
    delay(500);
    digitalWrite(yellowLED, LOW);
    delay(500);
  }
}

test(ConnectToWiFi) {
  // Simula la conexión WiFi
  // Verifica que se conecte correctamente
  connectToWiFi();
  assertEqual(WiFi.status(), WL_CONNECTED);
}

test (ParseJson) {
  // Simula un JSON de ejemplo
  String json = "{\"workflow_runs\":[{\"conclusion\":\"success\"}]}";
  parseJson(json);
  // Verifica que el LED verde se encienda y los demás se apaguen
  assertEqual(digitalRead(greenLED), HIGH);
  assertEqual(digitalRead(yellowLED), LOW);
  assertEqual(digitalRead(redLED), LOW);
}

test (BlinkYellowLED) {
  // Simula el parpadeo del LED amarillo
  blinkYellowLED();
  // Verifica que el LED amarillo parpadee 5 veces
  // (esto depende de la implementación exacta de tu función)
  // Puedes ajustar los valores si es necesario
  // También puedes considerar utilizar mocks o stubs para probar esto de manera más efectiva
  // en un entorno controlado.
  // assertEqual(...) para verificar el comportamiento esperado.
}


void setup() {
  Serial.begin(115200);

  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  connectToWiFi();


}

void loop() {
  Test::run();
  if (WiFi.status() == WL_CONNECTED) {
    makeHttpRequest();
  } else {
    Serial.println("WiFi Disconnected");
    blinkYellowLED();
  }

  delay(4000);
}
