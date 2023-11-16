#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ArduinoUnit.h>

const char* apSSID = "ESP32 IC-config";
const char* apPassword = "123456789";

AsyncWebServer server(80);

const int redLED = 4;
const int yellowLED = 2;
const int greenLED = 15;

int anterior = 0;

String githubToken;  // Variable para almacenar el token de GitHub
String wifiSSID;     // Variable para almacenar el SSID de la red Wi-Fi
String wifiPassword; // Variable para almacenar la contraseña de la red Wi-Fi
String repoOwner;
String repoName;
String ip;
bool wifiConnected = false;  // Indicador de conexión a Wi-Fi


void setup() {
  Serial.begin(115200);

  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);


  // Crea un Access Point
  WiFi.softAP(apSSID, apPassword);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(apIP);
  delay(1000);


   // Configura el servidor web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head><style>";
    html += "body { font-family: Arial, sans-serif; text-align: center; margin: 20px; }";
    html += "h1 { color: #333; }";
    html += "p { color: #666; }";
    html += "a { text-decoration: none; color: #4CAF50; font-weight: bold; }";
    html += "a:hover { text-decoration: underline; }";
    html += "</style></head><body>";
    html += "<h1>ESP32 CI Server</h1>";
    html += "<p>GitHub Token: " + githubToken + "</p>";
    html += "<p>Wi-Fi SSID: " + wifiSSID + "</p>";
    html += "<a href='/config'>Configurar</a>";
    html += "</body></html>";
    request->send(200, "text/html", html);
});

  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head><style>";
    html += "body { font-family: Arial, sans-serif; margin: 20px;}";
    html += "h2 { color: #333; }";
    html += "form { margin-top: 20px; }";
    html += "label { display: block; margin-bottom: 8px; }";
    html += "input[type='text'], input[type='password'] { width: 100%; padding: 8px; margin-bottom: 15px;}";
    html += "input[type='submit'] { background-color: #4CAF50; color: white; padding: 10px 15px; border: none; cursor: pointer; }";
    html += "</style></head><body>";
    html += "<h2>Configuracion</h2>";
    html += "<form action='/saveConfig' method='post'>";
    html += "<label for='token'>GitHub Token:</label><input type='text' name='token'><br>";
    html += "<label for='owner'>GitHub Repo Owner:</label><input type='text' name='owner'><br>";
    html += "<label for='repo'>GitHub Repo Name:</label><input type='text' name='repo'><br>";
    html += "<label for='ssid'>Wi-Fi SSID:</label><input type='text' name='ssid'><br>";
    html += "<label for='password'>Wi-Fi Password:</label><input type='password' name='password'><br>";
    html += "<input type='submit' value='Guardar'>";
    html += "</form></body></html>";
    request->send(200, "text/html", html);
});

  server.on("/saveConfig", HTTP_POST, [](AsyncWebServerRequest *request){
    githubToken = request->arg("token");
    repoOwner = request->arg("owner");
    repoName = request->arg("repo");
    wifiSSID = request->arg("ssid");
    wifiPassword = request->arg("password");
    wifiConnected = true;  // Indica que se han proporcionado las credenciales de Wi-Fi
    request->redirect("/");
  });


  server.begin();
}

void loop() {
 
  //Test::run();
  if (wifiConnected) {
    // Si se han proporcionado las credenciales de Wi-Fi, intenta conectarte a la red
    connectToWiFi();
    Serial.println("Conexion a Wifi Exitosa");

  }
  makeHttpRequest();
  delay(4000);
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


void blinkYellowLED() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(yellowLED, HIGH);
    delay(500);
    digitalWrite(yellowLED, LOW);
    delay(500);
  }
}

void parseJson(String json) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json);

  JsonObject latestRun = doc["workflow_runs"][0];

  String conclusion = latestRun["conclusion"];
  Serial.println("Conclusión de la última ejecución: " + conclusion);

  if (conclusion == "success") {
    if(anterior != 1){
      digitalWrite(greenLED, HIGH);
      digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, LOW);
      delay(400);
      digitalWrite(greenLED, LOW);
      digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, LOW);
      delay(400);
      anterior =1;
    }
    digitalWrite(greenLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
  } else if (conclusion == "failure") {
    if(anterior != 2){
      digitalWrite(greenLED, LOW);
      digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, HIGH);
      delay(400);
      digitalWrite(greenLED, LOW);
      digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, LOW);
      delay(400);
      anterior =2;

    }
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, HIGH);
  } else if (conclusion == "neutral" || conclusion == "null") {
    if(anterior != 3){
      digitalWrite(greenLED, LOW);
      digitalWrite(yellowLED, HIGH);
      digitalWrite(redLED, LOW);
      delay(400);
      digitalWrite(greenLED, LOW);
      digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, LOW);
      delay(400);
      anterior =3;
    }
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, HIGH);
    digitalWrite(redLED, LOW);
  } else {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
  }
}

void connectToWiFi() {
  WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    blinkYellowLED();
    Serial.println("Connecting to Wi-Fi...");
  }
  wifiConnected = false;  // Resetea el indicador después de la conexión exitosa
}

void makeHttpRequest() {
  if (WiFi.status() != WL_CONNECTED || githubToken.length() == 0) {
    // Si no se han proporcionado las credenciales de Wi-Fi o el token de GitHub, parpadea el LED amarillo
    blinkYellowLED();
    return;
  }

  HTTPClient http;
  http.begin("https://api.github.com/repos/"+repoOwner+"/"+repoName+"/actions/runs?per_page=1&page=1");
  http.addHeader("Authorization", "token " + githubToken);

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