/*
 * Servidor web con ESP32
 * Librerias necesarias para conectarnos a un entorno Wifi y poder configurar un servidor WEB
 */

#include <WiFi.h>
#include <WebServer.h>

/* Añade tu SSID & Clave para acceder a tu Wifi */
const char* ssid = "PRUEBA32";  // Tu SSID
const char* password = "PRUEBA32";  //Tu Clave

WebServer server(80);

/* Declaramos los pines para el sensor ultrasónico */
uint8_t triggerPin = 12;
uint8_t echoPin = 13;

float distance = 0;

void handleOnConnect() {
  calcularDistancia(); // Calcula la distancia
  server.send(200, "text/html", SendHTML(distance));
}

void handleDistancia() {
  calcularDistancia();
  server.send(200, "text/plain", String(distance));
}

void handleNotFound() {
  server.send(404, "text/plain", "La pagina no existe");
}

String SendHTML(float dist) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Sensor Ultrasonico</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += "p {font-size: 24px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "<script>\n";
  ptr += "setInterval(function() {\n";
  ptr += "  getData();\n";
  ptr += "}, 1000);\n";
  ptr += "function getData() {\n";
  ptr += "  var xhttp = new XMLHttpRequest();\n";
  ptr += "  xhttp.onreadystatechange = function() {\n";
  ptr += "    if (this.readyState == 4 && this.status == 200) {\n";
  ptr += "      var distance = parseFloat(this.responseText);\n";
  ptr += "      document.getElementById('distanceValue').innerHTML = distance.toFixed(2);\n";
  ptr += "      if (distance < 7) {\n";
  ptr += "        alert('Cuidado! La distancia es menor a 7 centimetros');\n";
  ptr += "      }\n";
  ptr += "    }\n";
  ptr += "  };\n";
  ptr += "  xhttp.open('GET', '/distance', true);\n";
  ptr += "  xhttp.send();\n";
  ptr += "}\n";
  ptr += "</script>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>ESP32 Sensor Ultrasonico</h1>\n";
  ptr += "<p>Distancia: <span id='distanceValue'>";
  ptr += dist;
  ptr += "</span> cm</p>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

void calcularDistancia() {
  // Genera un pulso de disparo para el sensor ultrasónico
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Lee el pulso de eco (si hay un pulso de eco)
  float duration = pulseIn(echoPin, HIGH);

  // Calcula la distancia (en centímetros) basada en el tiempo de viaje del pulso de ida y vuelta
  distance = duration * 0.034 / 2;
}

void setup() {
  Serial.begin(115200);

  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Conectado a ");
  Serial.println(ssid);
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleOnConnect);
  server.on("/distance", handleDistancia);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}

