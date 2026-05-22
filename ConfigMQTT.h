// ConfigMQTT.h
// Configurações gerais do MQTT para o projeto

#ifndef CONFIG_MQTT_H
#define CONFIG_MQTT_H

#define MQTT_KEEPALIVE 30
#define MQTT_QOS_RELE 1
#define MQTT_QOS_SENSOR 0
#define MQTT_RETAIN_RELE true
#define MQTT_RETAIN_SENSOR false

// Instância do cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Subscribe topics array (must match sub1..sub8 order)
const char* subTopics[NUM_RELES] = {sub1, sub2, sub3, sub4, sub5, sub6, sub7, sub8};

void conectar_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se ao WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Non-blocking MQTT reconnect using millis()
unsigned long lastMqttReconnectAttempt = 0;
const unsigned long MQTT_RECONNECT_INTERVAL = 5000;

void reconectar_mqtt() {
  unsigned long now = millis();
  if (now - lastMqttReconnectAttempt < MQTT_RECONNECT_INTERVAL) {
    return; // Wait before next attempt
  }
  lastMqttReconnectAttempt = now;

  Serial.print("Tentando conectar ao MQTT...");
  client.setKeepAlive(MQTT_KEEPALIVE);
  if (client.connect(clientID, mqttUserName, mqttPwd)) {
    Serial.println("Conectado ao MQTT!");
    for (int i = 0; i < NUM_RELES; i++) {
      client.subscribe(subTopics[i], MQTT_QOS_RELE);
    }
  } else {
    Serial.print("Falha, rc=");
    Serial.print(client.state());
    Serial.println(". Nova tentativa em 5s.");
  }
}

// MQTT callback - refatorada com arrays
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");

  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Find which relay topic this message belongs to
  for (int i = 0; i < NUM_RELES; i++) {
    if (strcmp(topic, subTopics[i]) == 0) {
      char msg = (char)payload[0];
      if (msg == '1') {
        // Turn ON (active LOW)
        digitalWrite(relayPins[i], LOW);
        estadoReles[i] = 0;
        client.publish(relayPubTopics[i], "1", MQTT_RETAIN_RELE);
        Serial.printf("Dispositivo %d LIGADO via MQTT\n", i + 1);
      } else if (msg == '0') {
        // Turn OFF (active LOW)
        digitalWrite(relayPins[i], HIGH);
        estadoReles[i] = 1;
        client.publish(relayPubTopics[i], "0", MQTT_RETAIN_RELE);
        Serial.printf("Dispositivo %d DESLIGADO via MQTT\n", i + 1);
      }
      return;
    }
  }
  Serial.println("Tópico não inscrito");
}

#endif // CONFIG_MQTT_H
