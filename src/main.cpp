#include <ESP8266WiFi.h> // Include the Wi-Fi library

// Set pin 2 for NodeMCU or specify your GPIO pin
const int ledPin = 2;
const int pwmPin = 12;
bool ledOn = false;

const char *ssid = "Wifiat 1";       // The SSID (name) of the Wi-Fi network you want to connect to
const char *password = "Fiat126bis"; // The password of the Wi-Fi network

IPAddress server(192, 168, 0, 230);
int port = 5001;
WiFiClient client;
int heartbeatTimer = 30;

void connect_to_wifi();

void setup()
{
  Serial.begin(115200); // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  // Initialize the LED pin as an output
  pinMode(pwmPin, OUTPUT);
  delay(100);
  analogWrite(pwmPin, 0);

  connect_to_wifi();
  while (!client.connect(server, port))
  {
    Serial.println("Connection to server failed, retrying...");
    delay(1000);
  }
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED){
    connect_to_wifi(); 
  }

  if (!client.connected()){
    Serial.println("Server disconnected");

    client.stop();

    while (!client.connect(server, port))
    {
      Serial.println("Connection to server failed, retrying...");
      delay(1000);
    }
  }

  // heartbeat ~30 sec
  if(heartbeatTimer <= 0){
    if (client.connected())
      client.println("PING");
  }

  while (client.available())
  {
    String cmd = client.readStringUntil('\n');

    Serial.println(cmd);

    if (cmd == "MOTORON")
      analogWrite(pwmPin, 255);

    else if (cmd == "MOTOROFF")
      analogWrite(pwmPin, 0);


    if (client.connected())
      client.println("ACK " + cmd);
  }

  heartbeatTimer--;
  delay(1000);

}

void connect_to_wifi()
{
  WiFi.begin(ssid, password); // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i);
    Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer
  Serial.println('\n');
}
