
#include <ESP8266WiFi.h>

const char* ssid     = "usuario";
const char* password = "contrasena";


WiFiServer server(80);


String header;

int temp = 0;
int hum = 0;
int co2 = 0;

unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

String getTemp(){
  temp+=1;
  return (String)temp; 
}


String getHum(){
  hum+=2;
  return (String)hum; 
}

String getCo2(){
  co2+=3;
  return (String)co2; 
}

void loop(){
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
            client.println("</style><meta http-equiv=\"refresh\" content=\"3\" /></head>");
              
            // Web Page Heading
            client.println("<body><h1 style=\"color: white\">Reto 2</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            //TEMP
            client.println("<div class=\"card\" style=\"float:left; box-shadow: 0 4px 8px 0 rgba(0,0,0,0.2); width: 32%; background: white; margin-left: 25px\"> <img src=\"https://img.favpng.com/1/4/13/thermometer-temperature-computer-icons-symbol-png-favpng-nXxuwXmB4TBXtTy9tHS0Un4J8.jpg\" alt=\"Temp\" style=\"width:450px; height: 350px\"> <div class=\"container\" style=\" background: white\"> <h4><b>Temperature</b></h4> <p>" + getTemp() + " Celsius </p> </div></div>");
  
            //HUM
            client.println("<div class=\"card\" style=\"float:left; box-shadow: 0 4px 8px 0 rgba(0,0,0,0.2); width: 32%; background: white; margin-left: 10px\"> <img src=\"https://img.favpng.com/2/17/24/moisture-logo-png-favpng-wtAsi7hi39qU9KV6FRiXUmF1T.jpg\" alt=\"Hum\" style=\"width:400px; height: 350px\"> <div class=\"container\" style=\" background: white\"> <h4><b>Humedad</b></h4> <p>" + getHum() + " Humedad </p> </div></div>");
  
            //CO2
            client.println("<div class=\"card\" style=\"float:left; box-shadow: 0 4px 8px 0 rgba(0,0,0,0.2); width: 32%; background: white; margin-left: 10px\"> <img src=\"https://img.flaticon.com/icons/png/512/1504/1504946.png?size=1200x630f&pad=10,10,10,10&ext=png&bg=FFFFFFFF\" alt=\"Co2\" style=\"width:450px; height: 350px\"> <div class=\"container\" style=\" background: white\"> <h4><b>Carbono</b></h4> <p>" + getCo2() + " Co2 </p> </div></div>");

            
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
