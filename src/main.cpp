#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>



//************************************************************************** LAN Network definieren
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 22); //comment this line if you are using DHCP
IPAddress server(192, 168, 1, 5);
EthernetClient ethClient;

//************************************************************************** mqtt Referenz
PubSubClient client(ethClient);


//************************************************************************** Intervalle
long lastReconnectAttempt = 0;
const int reconnectInterval = 5000;

//************************************************************************** Variablen
long letzte_meldung = 0;
char publish_meldung[50];
int zaehler = 0;

//************************************************************************** Funktionsprototypen
void loop                       ();
void setup                      ();
boolean reconnect               ();


// ######################################################################## mqtt reconnect
boolean reconnect() {
  if (client.connect("GIMA_TEST2", "daten1", "qwerqwer")) {
    Serial.println("Connecting to MQTT broker...");

  } else {

    Serial.print("failed, rc=");
    Serial.println(client.state());
  }
  return client.connected();
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

}

//######################################################################## loop
void loop() {

  if (!client.connected()) {
    Serial.println("Client Not Connected...");
    long now = millis();
    if (now - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        Serial.println("Connected!");
        lastReconnectAttempt = 0;

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


      }
    }
  } else {

  }
}

