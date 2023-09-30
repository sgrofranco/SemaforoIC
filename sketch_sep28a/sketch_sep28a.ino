#include <WiFiClientSecure.h>

const char* ssid = "noteconectes";
const char* password = "12345678";

const char* host = "api.github.com";
const int httpPort = 80;

WiFiClientSecure client;

#define RED_LED_PIN 13
#define YELLOW_LED_PIN 12
#define GREEN_LED_PIN 14

void setup() {
  Serial.begin(9600);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  connectToWiFi();
}

void loop() {
  getRepoStatus(); // Obtiene el estado del repositorio

  // Analiza el estado y controla los LEDs
  // (por ejemplo, si el estado es "success", enciende el LED verde)
}

void connectToWiFi() {
  Serial.println("Conectando a WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando...");
  }

  Serial.println("Conexión WiFi establecida");
}

void getRepoStatus() {
  if (!client.connect(host, httpsPort)) {
    Serial.println("No se pudo conectar al servidor");
    return;
  }

  String url = "/repos/sgrofranco/SemaforoIC/commits/main/status";
  String request = "GET " + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Authorization: token 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\r\n" +
                   "User-Agent: ESP32\r\n" +
                   "Connection: close\r\n\r\n";

  client.print(request);
  delay(500);

  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  client.stop();
}

void controlarLEDs(String estado) {
  if (estado == "success") {
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
  } else if (estado == "failure") {
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);
  } else if (estado == "pending") {
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
  } else {
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
  }
}
