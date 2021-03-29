#include <WiFi.h>
#include <DHT.h>
#include "secrets.h"


// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 25
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11

// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiServer server(80);


String header;

#define light_sensor A0
#define rain_sensor A1
// --- LIGHT SENSOR ---
const double k = 5.0 / 1024;
const double luxFactor = 500000;
const double R2 = 220;
const double LowLightLimit = 200;
const double B = 1.3 * pow(10.0, 7);
const double m = -1.4;
// --- END LIGHT SENSOR ---

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(SECRET_SSID);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  dht.begin();
}

String getLightIntensity() {
  double V2 = k * analogRead(light_sensor);
  double R1 = (5.0 / V2 - 1) * R2;
  double lux = B * pow(R1, m) + 50;
  Serial.print("Lux: ");
  Serial.println(lux);
  return (String)lux;
}

String getTemp() {
  float t = dht.readTemperature();
  Serial.print("Temp: ");
  Serial.println(t);
  return (String)t;
}

String getHum() {
  float h = dht.readHumidity();
  Serial.print("Hum: ");
  Serial.println(h);
  return (String)h;
}

String getRain() {
  int sensorData = analogRead(rain_sensor);
  Serial.print("Rain: ");
  Serial.println(sensorData);
  int range = map(sensorData, 0, 4095, 0, 3);
  switch (range) {
    case 0:    // Sensor getting wet
      return "Lloviendo";
    case 1:
      return "Llovizna";
    default:
      return "Sin lluvia";
  }
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html style=\"background: deepskyblue\">");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".card { box-shadow: 0 4px 8px 0 rgba(0,0,0,0.2); transition: 0.3s; width: 40%;}.card:hover { box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2);}.container { padding: 2px 16px;}");
            client.println("</style><meta http-equiv=\"refresh\" content=\"5\" /></head>");

            // Web Page Heading
            client.println("<body><h1 style=\"color: white\">Reto 2</h1>");

            // Display current state, and ON/OFF buttons for GPIO 5

            // TEMP
            client.println("<div class=\"card\" style=\"float:left; box-shadow: 0 4px 8px 0 rgba(0,0,0,0.2); width: 32%; background: white; margin-left: 25px\"> <img src=\"https://img.favpng.com/1/4/13/thermometer-temperature-computer-icons-symbol-png-favpng-nXxuwXmB4TBXtTy9tHS0Un4J8.jpg\" alt=\"Temp\" style=\"width:450px; height: 350px\"> <div class=\"container\" style=\" background: white\"> <h4><b>Temperature</b></h4> <p>" + getTemp() + " Celsius </p> </div></div>");

            // HUM
            client.println("<div class=\"card\" style=\"float:left; box-shadow: 0 4px 8px 0 rgba(0,0,0,0.2); width: 32%; background: white; margin-left: 10px\"> <img src=\"https://img.favpng.com/2/17/24/moisture-logo-png-favpng-wtAsi7hi39qU9KV6FRiXUmF1T.jpg\" alt=\"Hum\" style=\"width:400px; height: 350px\"> <div class=\"container\" style=\" background: white\"> <h4><b>Humedad</b></h4> <p>" + getHum() + " Humedad </p> </div></div>");

            // Light
            client.println("<div class=\"card\" style=\"float:left; box-shadow: 0 4px 8px 0 rgba(0,0,0,0.2); width: 32%; background: white; margin-left: 10px\"> <img src=\"https://img.flaticon.com/icons/png/512/1504/1504946.png?size=1200x630f&pad=10,10,10,10&ext=png&bg=FFFFFFFF\" alt=\"Co2\" style=\"width:450px; height: 350px\"> <div class=\"container\" style=\" background: white\"> <h4><b>Carbono</b></h4> <p>" + getLightIntensity() + " Lux </p> </div></div>");

            // Rain
            client.println("<div class=\"card\" style=\"float:left; box-shadow: 0 4px 8px 0 rgba(0,0,0,0.2); width: 32%; background: white; margin-left: 10px\"> <img src=\"https://img.flaticon.com/icons/png/512/1504/1504946.png?size=1200x630f&pad=10,10,10,10&ext=png&bg=FFFFFFFF\" alt=\"Co2\" style=\"width:450px; height: 350px\"> <div class=\"container\" style=\" background: white\"> <h4><b>Carbono</b></h4> <p>" + getRain() + "</p> </div></div>");

            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
