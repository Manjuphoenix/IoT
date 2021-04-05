
/****************************************
 * Include Libraries
 ****************************************/
#include <WiFi.h>
#include <PubSubClient.h>




#define WIFISSID "sujay" // Put your Wifi SSID here
#define PASSWORD "12345678" // Put your WIFI password here
#define TOKEN "BBFF-NPfhxTDfEQ2q3DnKATI2P8UrvyRd5E" // Put your Ubidots' TOKEN
#define MQTT_CLIENT_NAME "1234a5d6798" // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 
                                           //it should be a random and unique ascii string and different from all other devices
 
/****************************************
 * Define Constants
 ****************************************/
#define VARIABLE_LABEL "sensor" // Assign the variable label
#define DEVICE_LABEL "esp32" // Assig the device label
 
#define SENSOR A0 // Set the A0 as SENSOR
 
char mqttBroker[]  = "industrial.api.ubidots.com";
char payload[100];
char topic[150];
// Space to store values to send
char str_sensor[10];
 
/****************************************
 * Auxiliar Functions
 ****************************************/
WiFiClient ubidots;
PubSubClient client(ubidots);


 
void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  Serial.write(payload, length);
  Serial.println(topic);
}
 
void reconnect() {
  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}
 
/****************************************
 * Main Functions
 ****************************************/
void setup() {
  Serial.begin(9600);
  pinMode(3, INPUT); // Setup for leads off detection LO +
  pinMode(2, INPUT); // Setup for leads off detection LO -
  WiFi.begin(WIFISSID, PASSWORD);
  // Assign the pin as INPUT 
  pinMode(34, INPUT);
 
  Serial.println();
  Serial.print("Waiting for WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);
  
  
}
 
void loop() {
    if((digitalRead(18) == 1)||(digitalRead(19) == 1)){
    Serial.println('!');
  }
  else{
    // send the value of analog input 0:
      Serial.println(analogRead(34));
  }
  //Wait for a bit to keep serial data from saturating
  delay(1);
  if (!client.connected()) {
    reconnect();
  }
 
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(payload, "%s", ""); // Cleans the payload
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL); // Adds the variable label
  
  /*float sensor = analogRead(SENSOR); */
  
  /* 4 is minimum width, 2 is precision; float value is copied onto str_sensor*/
  /*dtostrf(sensor, 4, 2, str_sensor);
  */
  sprintf(payload, "%s {\"value\": %s}}", payload, str_sensor); // Adds the value
  Serial.println("Publishing data to Ubidots Cloud");
  client.publish(topic, payload);
  client.loop();
  delay(500);
}
