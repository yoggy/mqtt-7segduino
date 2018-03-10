/////////////////////////////////////////////////////////////////////////////

#define PIN_LED 2

void reboot() {
  Serial.println("REBOOT!!!!!");

  for (int i = 0; i < 5; ++i) {
    digitalWrite(PIN_LED, HIGH);
    delay(500);
    digitalWrite(PIN_LED, LOW);
    delay(500);
  }

  ESP.reset();
}

/////////////////////////////////////////////////////////////////////////////
#include <ESP8266WiFi.h>
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient/


// Wi-fi config (from config.ino)
extern char *wifi_ssid;
extern char *wifi_password;
extern char *mqtt_server;
extern int  mqtt_port;

extern char *mqtt_client_id;
extern bool mqtt_use_auth;
extern char *mqtt_username;
extern char *mqtt_password;

extern char *mqtt_subscribe_topic;

WiFiClient wifi_client;
PubSubClient mqtt_client(mqtt_server, mqtt_port, on_message_received, wifi_client);

void on_message_received(char* topic, byte* payload, unsigned int length) {
  Serial.print("segd");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup_mqtt()
{
  pinMode(PIN_LED, OUTPUT);

  WiFi.begin(wifi_ssid, wifi_password);
  WiFi.mode(WIFI_STA);

  int wifi_count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    wifi_count ++;
    if (wifi_count % 2) {
      digitalWrite(PIN_LED, HIGH);
    }
    else {
      digitalWrite(PIN_LED, LOW);
    }
    delay(1000);

    if (wifi_count > 100) reboot();
  }

  bool rv = false;
  if (mqtt_use_auth == true) {
    rv = mqtt_client.connect(mqtt_client_id, mqtt_username, mqtt_password);
  }
  else {
    rv = mqtt_client.connect(mqtt_client_id);
  }
  if (rv == false) {
    reboot();
  }

  mqtt_client.subscribe(mqtt_subscribe_topic);

  digitalWrite(PIN_LED, HIGH);
}

/////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  setup_mqtt();
}

void loop() {
  if (!mqtt_client.connected()) {
    reboot();
  }
  mqtt_client.loop();
}
