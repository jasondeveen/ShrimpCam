// https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer

#include <ESP8266WiFi.h>        // Include the Wi-Fi library

// Set pin 2 for NodeMCU or specify your GPIO pin
const int ledPin = 2; 
bool ledOn = false;

const char* ssid     = "xxx";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "xxx";     // The password of the Wi-Fi network
WiFiServer server(80);

void connect_to_wifi();

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  
  // Initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  connect_to_wifi();
}

void loop() { 
  if (WiFi.status() != WL_CONNECTED){
    connect_to_wifi();
  }

  WiFiClient client = server.accept();
  // wait for a client (web browser) to connect
  if (client)
  {
    Serial.println("\n[Client connected]");
    while (client.connected())
    {
      // read line by line what the client (web browser) is requesting
      String line = "";
      if (client.available())
      {
        line = client.readStringUntil('\r');

        if (line == "LEDON"){
          ledOn = true;
          break;
        }
        else if (line == "LEDOFF"){
          ledOn = false;
          break;
        }
        Serial.print(line);
      }
    }

    while (client.available()) {
      // but first, let client finish its request
      // that's diplomatic compliance to protocols
      // (and otherwise some clients may complain, like curl)
      // (that is an example, prefer using a proper webserver library)
      client.read();
    }

    if (ledOn)
      digitalWrite(ledPin, LOW); 
    else 
      digitalWrite(ledPin, HIGH); 

    // close the connection:
    String htmlPage;
    htmlPage.reserve(1024);               // prevent ram fragmentation
    htmlPage = F("HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"  // the connection will be closed after completion of the response
                "\r\n");
    htmlPage += ledOn ? "Led on" : "Led off";
    htmlPage += "\r\n";
    
    client.println(htmlPage);
    client.stop();
    Serial.println("[Client disconnected]");

    
  }
  else {
    Serial.println("No client found");
    delay(1000);
  }
}

void connect_to_wifi() {
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  Serial.println('\n');

  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());

}