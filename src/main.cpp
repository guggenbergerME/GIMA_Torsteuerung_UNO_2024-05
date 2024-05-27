#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

//************************************************************************** LAN Network definieren N
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

//************************************************************************** Pinout
const int RELAIS_AUF = 2; // Relais-Pin am Arduino

//************************************************************************** Funktionsprototypen
void loop                       ();
void setup                      ();
void reconnect                  ();
void callback(char* topic, byte* payload, unsigned int length);


//************************************************************************** Zeitschleifen
unsigned long previousMillis_mqtt = 0; // mqtt abrufen
unsigned long interval_mqtt = 50; 

//######################################################################## setup
void setup() {

  // Serielle Schnittstelle
  Serial.begin(115200);


  // Ethernet starten
  Ethernet.begin(mac, ip);
  // Pause Netzwerk Antwort
  delay(1500);

  // Client starten und Callback abrufen
  client.setServer(server, 1883);
  client.setCallback(callback);

  // Pins belegen
  pinMode(RELAIS_AUF, OUTPUT);
  // Ausgang nach Neustart setzen
  digitalWrite(RELAIS_AUF, !LOW);

}

// ######################################################################## mqtt callback
void callback(char* topic, byte* payload, unsigned int length) {


    if (strcmp(topic,"GIMA_Technik/TEST_ARDUINO_LAN/Steuerung")==0) {

        // Kanal A
        if ((char)payload[0] == 'o' && (char)payload[1] == 'n') {  
                 Serial.println("relais_A -> AN");
                  digitalWrite(RELAIS_AUF, !HIGH);
                 delay(10);
              }

        if ((char)payload[0] == 'o' && (char)payload[1] == 'f' && (char)payload[2] == 'f') {  
                 Serial.println("relais_A -> AUS");
                  digitalWrite(RELAIS_AUF, !LOW);
                 delay(10);
              }
      } 
}



// ######################################################################## mqtt reconnect
void reconnect() {
  // Loop until we're reconnected
   while (!client.connected()) {
    /*
    Serial.print("Verbindung zu mqtt IP: ");
    Serial.print(server);
    Serial.println("");
    */
    // Create a random client ID
    String clientId = "GIMA-Technik";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect("GIMA_TEST2", "daten1", "qwerqwer")) {
      Serial.println("mqtt lesen ....");
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
}

//######################################################################## loop
void loop() {

  // mqtt Daten senden     
  if (!client.connected()) {
      reconnect();
    }
    client.loop(); 


  // Nach einem Zeitintervall eine Nachricht publishen
  long now = millis();
  if (now - letzte_meldung > 6000) {
    letzte_meldung = now;
    ++zaehler;
    snprintf (publish_meldung, 75, "UNO Zähler #%ld", zaehler);
    Serial.print("## >> mqtt - Nachricht publish: ");
    Serial.println(publish_meldung);
    client.publish("GIMA_Technik/TEST_ARDUINO_LAN/Zaehler", publish_meldung);
  }

}

