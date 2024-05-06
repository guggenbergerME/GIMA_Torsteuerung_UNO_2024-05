#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>



//DEFINE NETWORK
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 22); //comment this line if you are using DHCP
IPAddress server(192, 168, 1, 5);
EthernetClient ethClient;

//MQTT Parameters
String topicBase = "location/room/sub-room/";
const char id[] = "GardenCenter";
PubSubClient client(ethClient);

long lastReconnectAttempt = 0;
const int reconnectInterval = 5000;


long letzte_meldung = 0;
char publish_meldung[50];
int zaehler = 0;

boolean reconnect() {
  if (client.connect("GIMA_TEST2", "daten1", "qwerqwer")) {
    Serial.println("Connecting to MQTT broker...");

  } else {

    Serial.print("failed, rc=");
    Serial.println(client.state());
  }
  return client.connected();
}


void setup() {

  Serial.begin(9600);


  // Ethernet.begin(mac); //Use this if you want to use DHCP
  Ethernet.begin(mac, ip); //Use this if you want a static iP.
  delay(1500); //Used to allow time for the network to connect.

  client.setServer(server, 1883);

}

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

