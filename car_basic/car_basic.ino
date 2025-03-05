#include <ESP8266WiFi.h>

const char* ssid = "NISHAT-2.4G";
const char* password = "nishat2020";

WiFiServer server(80);
WiFiClient client;

const byte L298N_A_pin = D1; // GPI05
const byte L298N_A_In1_pin = D2; // GPI04
const byte L298N_A_In2_pin = D3; // GPI0
const byte L298N_B_In3_pin = D4; // GPI02
const byte L298N_B_In4_pin = D5; // GPIO14
const byte L298N_B_pin = D6; // GPI12

const byte Led1_pin = D7; // GPIO13  // Left Turn Signal
const byte Led2_pin = D8; // GPIO15  // Right Turn Signal

// Set a static IP address
IPAddress staticIP(192, 168, 0, 100); // IP address
IPAddress gateway(192, 168, 0, 1);    // Gateway (router IP)
IPAddress subnet(255, 255, 255, 0);   // Subnet mask

void setup() {
  pinMode(L298N_A_In1_pin, OUTPUT);
  pinMode(L298N_A_In2_pin, OUTPUT);
  pinMode(L298N_B_In3_pin, OUTPUT);
  pinMode(L298N_B_In4_pin, OUTPUT);

  digitalWrite(L298N_A_In1_pin, LOW);
  digitalWrite(L298N_A_In2_pin, LOW);
  digitalWrite(L298N_B_In3_pin, LOW);
  digitalWrite(L298N_B_In4_pin, LOW);


  
  Serial.begin(115200);

  // Configure static IP
  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("Connected, IP address: " + WiFi.localIP().toString());

  server.begin();
  Serial.println("Setup Done");
}

void loop() {
  // Check if there are any new clients
  if (server.hasClient()) {
    // If we are already connected to another client, disconnect that client
    if (client.connected()) {
      Serial.println("Disconnecting previous client.");
      client.stop();
    }
    // Accept the new connection
    client = server.available();
    Serial.println("New client connected.");
  }

  if (!client.connected()) {
    // If the client has disconnected, stop the client and clean up
    Serial.println("Client disconnected.");
    client.stop();
    return; // Early return to skip processing when no client is connected
  }

  // Handle incoming data from the connected client
  if (client.available()) {
    String currentLine = ""; // To store the incoming data
    while (client.available()) { // While there is data to read
      char c = client.read();
      Serial.write(c);
      if (c == '\n') { // End of a command
        if (currentLine.length() == 0) {
          // A blank line means the end of the client's request, possibly a keep-alive
          continue;
        } else {
          handleCommand(currentLine); // Process the command
          currentLine = ""; // Clear the line after handling
        }
      } else if (c != '\r') {
        currentLine += c; // Add the character to the line
      }
    }
  }
}

void handleCommand(String command) {
  String response = "";

  Serial.println("Receive Command: " + command);
  if (command == "L") {
    motorSpeed(900, LOW, LOW, 1023, HIGH, LOW);
    
    response = "Turning left";
    Serial.println("900,1023"); // Output motor speeds to serial for plotting
    Serial.println("Turning left"); // Output motor speeds to serial for plotting
    
  } else if (command == "R") {
    motorSpeed(1023, HIGH, LOW, 900, LOW, LOW);
    
    response = "Turning RIGHT";
    Serial.println("1023,900"); // Output motor speeds to serial for plotting
    Serial.println("Turning Right"); // Output motor speeds to serial for plotting
    
  } else if (command == "S") {
    motorSpeed(1023, HIGH, LOW, 1023, HIGH, LOW);
    
    response = "MOVING FORWARD";
    Serial.println("1023,1023"); // Output motor speeds to serial for plotting
    Serial.println("Moving Forward"); // Output motor speeds to serial for plotting
    
  } else if (command == "O") {
    motorSpeed(0, LOW, LOW, 0, LOW, LOW);
    
    response = "stop";
    Serial.println("0,0"); // Output motor speeds to serial for plotting
    Serial.println("900,1023"); // Output motor speeds to serial for plotting
    
  } else {
    response = "wrong instruction";
    Serial.println("Error"); // Error message for unknown command
  }
  
  client.println(response);
}

void motorSpeed(int speedA, byte dirA1, byte dirA2, int speedB, byte dirB1, byte dirB2) {
  analogWrite(L298N_A_pin, speedA);
  analogWrite(L298N_B_pin, speedB);

  digitalWrite(L298N_A_In1_pin, dirA1);
  digitalWrite(L298N_A_In2_pin, dirA2);
  digitalWrite(L298N_B_In3_pin, dirB1);
  digitalWrite(L298N_B_In4_pin, dirB2);
}
