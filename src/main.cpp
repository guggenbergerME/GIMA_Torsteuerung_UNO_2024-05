#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

//************************************************************************** LAN Network definieren
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 22); //comment this line if you are using DHCP
IPAddress server(192, 168, 1, 5); // mqtt Server
EthernetClient ethClient;

//************************************************************************** mqtt Referenz
PubSubClient client(ethClient);


//************************************************************************** Intervalle
long lastReconnectAttempt = 0;
const int reconnectInterval = 5000;

unsigned long previousMillis_mqttCHECK = 0; // Windstärke prüfen
unsigned long interval_mqttCHECK = 900; 

//************************************************************************** Variablen
long letzte_meldung = 0;
char publish_meldung[50];
int zaehler = 0;

//************************************************************************** Funktionsprototypen
void loop                       ();
void setup                      ();
void reconnect                  ();
void callback(char* topic, byte* payload, unsigned int length);
void connect_mqtt               ();


//************************************************************************** Zeitschleifen
unsigned long previousMillis_mqtt = 0; // mqtt abrufen
unsigned long interval_mqtt = 500; 



// ######################################################################## mqtt callback
void callback(char* topic, byte* payload, unsigned int length) {


    if (strcmp(topic,"GIMA_Technik/TEST_ARDUINO_LAN/Steuerung")==0) {

        // Kanal A
        if ((char)payload[0] == 'o' && (char)payload[1] == 'n') {  
                 Serial.println("relais_A -> AN");

                 //client.publish("RK_WiFi_003/OUT/A","on");
                delay(100);
              }

        if ((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') {  
                 Serial.println("relais_A -> AUS");

                 //client.publish("RK_WiFi_003/OUT/A","off");
                delay(100);
              }
      } 
}


// ######################################################################## mqtt reconnect
void reconnect() {
  // Loop until we're reconnected
   while (!client.connected()) {
    Serial.print("Verbindung zu mqtt IP: ");
    Serial.print(server);
    Serial.println("");
    // Create a random client ID
    String clientId = "GIMA-Technik";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect("GIMA_TEST2", "daten1", "qwerqwer")) {
      Serial.println("Verbunden ....");
      // ... and resubscribe
      client.subscribe("GIMA_Technik/TEST_ARDUINO_LAN/Steuerung");
        
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }


  /*
  if (client.connect("GIMA_TEST2", "daten1", "qwerqwer")) {
    Serial.println("Connecting to MQTT broker...");

    // mqtt Meldungen auslesen 
    client.subscribe("GIMA_Technik/TEST_ARDUINO_LAN/Steuerung/");

  } else {
    Serial.print("failed, rc=");
    Serial.println(client.state());
  }
    return client.connected();
    */

}


//######################################################################## setup
void setup() {

  // Serielle Schnittstelle
  Serial.begin(115200);


  // Ethernet starten
  Ethernet.begin(mac, ip);
  // Pause Netzwerk Antwort
  delay(1500);

  // Client starten
  client.setServer(server, 1883);
  client.setCallback(callback);


}

//######################################################################## mqtt connect
void connect_mqtt(){

  // mqtt Daten senden     
  if (!client.connected()) {
      reconnect();
    }
    client.loop(); 

}



//######################################################################## loop
void loop() {


  if (!client.connected()) {
    reconnect();
  }
  client.loop();

/*
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ mqtt Checken
  if (millis() - previousMillis_mqttCHECK > interval_mqttCHECK) {
      previousMillis_mqttCHECK = millis(); 
      connect_mqtt();
    }  
*/

/*
  // Nach einem Zeitintervall eine Nachricht publishen
  long now = millis();
  if (now - letzte_meldung > 6000) {
    letzte_meldung = now;
    ++zaehler;
    snprintf (publish_meldung, 75, "Testzähler #%ld", zaehler);
    Serial.print("## >> mqtt - Nachricht publish: ");
    Serial.println(publish_meldung);
    client.publish("GIMA_Technik/TEST_ARDUINO_LAN/zaehler", publish_meldung);
  }
*/

}

