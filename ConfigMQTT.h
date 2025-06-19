// ConfigMQTT.h
// Configurações gerais do MQTT para o projeto

#ifndef CONFIG_MQTT_H
#define CONFIG_MQTT_H

#define MQTT_KEEPALIVE 30 // segundos
#define MQTT_QOS_RELE 1    // QoS para relés (comando e status)
#define MQTT_QOS_SENSOR 0  // QoS para sensores (temperatura, umidade, sensação)
#define MQTT_RETAIN_RELE true    // Retain para relés
#define MQTT_RETAIN_SENSOR false // Retain para sensores

// Instância do cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Função para conectar ao WiFi
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

// Função para reconectar ao MQTT
void reconectar_mqtt() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    client.setKeepAlive(MQTT_KEEPALIVE);
    if (client.connect(clientID, mqttUserName, mqttPwd)) {
      Serial.println("Conectado ao MQTT!");
      // Inscreve nos tópicos de relé com QOS_RELE
      client.subscribe(sub1, MQTT_QOS_RELE);
      client.subscribe(sub2, MQTT_QOS_RELE);
      client.subscribe(sub3, MQTT_QOS_RELE);
      client.subscribe(sub4, MQTT_QOS_RELE);
      client.subscribe(sub5, MQTT_QOS_RELE);
      client.subscribe(sub6, MQTT_QOS_RELE);
      client.subscribe(sub7, MQTT_QOS_RELE);
      client.subscribe(sub8, MQTT_QOS_RELE);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println("; tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

// Função de callback do MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");

  if (strstr(topic, sub1)) {
    for (int i = 0; i < length; i++) Serial.print((char)payload[i]);
    Serial.println();
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin1, LOW);
      estadoRele_1 = 0;
      client.publish(pub1, "0", MQTT_RETAIN_RELE);
    } else {
      digitalWrite(RelayPin1, HIGH);
      estadoRele_1 = 1;
      client.publish(pub1, "1", MQTT_RETAIN_RELE);
    }
  }
  else if (strstr(topic, sub2)) {
    for (int i = 0; i < length; i++) Serial.print((char)payload[i]);
    Serial.println();
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin2, LOW);
      estadoRele_2 = 0;
      client.publish(pub2, "0", MQTT_RETAIN_RELE);
    } else {
      digitalWrite(RelayPin2, HIGH);
      estadoRele_2 = 1;
      client.publish(pub2, "1", MQTT_RETAIN_RELE);
    }
  }
  else if (strstr(topic, sub3)) {
    for (int i = 0; i < length; i++) Serial.print((char)payload[i]);
    Serial.println();
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin3, LOW);
      estadoRele_3 = 0;
      client.publish(pub3, "0", MQTT_RETAIN_RELE);
    } else {
      digitalWrite(RelayPin3, HIGH);
      estadoRele_3 = 1;
      client.publish(pub3, "1", MQTT_RETAIN_RELE);
    }
  }
  else if (strstr(topic, sub4)) {
    for (int i = 0; i < length; i++) Serial.print((char)payload[i]);
    Serial.println();
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin4, LOW);
      estadoRele_4 = 0;
      client.publish(pub4, "0", MQTT_RETAIN_RELE);
    } else {
      digitalWrite(RelayPin4, HIGH);
      estadoRele_4 = 1;
      client.publish(pub4, "1", MQTT_RETAIN_RELE);
    }
  }
  else if (strstr(topic, sub5)) {
    for (int i = 0; i < length; i++) Serial.print((char)payload[i]);
    Serial.println();
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin5, LOW);
      estadoRele_5 = 0;
      client.publish(pub5, "0", MQTT_RETAIN_RELE);
    } else {
      digitalWrite(RelayPin5, HIGH);
      estadoRele_5 = 1;
      client.publish(pub5, "1", MQTT_RETAIN_RELE);
    }
  }
  else if (strstr(topic, sub6)) {
    for (int i = 0; i < length; i++) Serial.print((char)payload[i]);
    Serial.println();
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin6, LOW);
      estadoRele_6 = 0;
      client.publish(pub6, "0", MQTT_RETAIN_RELE);
    } else {
      digitalWrite(RelayPin6, HIGH);
      estadoRele_6 = 1;
      client.publish(pub6, "1", MQTT_RETAIN_RELE);
    }
  }
  else if (strstr(topic, sub7)) {
    for (int i = 0; i < length; i++) Serial.print((char)payload[i]);
    Serial.println();
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin7, LOW);
      estadoRele_7 = 0;
      client.publish(pub7, "0", MQTT_RETAIN_RELE);
    } else {
      digitalWrite(RelayPin7, HIGH);
      estadoRele_7 = 1;
      client.publish(pub7, "1", MQTT_RETAIN_RELE);
    }
  }
  else if (strstr(topic, sub8)) {
    for (int i = 0; i < length; i++) Serial.print((char)payload[i]);
    Serial.println();
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin8, LOW);
      estadoRele_8 = 0;
      client.publish(pub8, "0", MQTT_RETAIN_RELE);
    } else {
      digitalWrite(RelayPin8, HIGH);
      estadoRele_8 = 1;
      client.publish(pub8, "1", MQTT_RETAIN_RELE);
    }
  }
  else {
    Serial.println("tópico não inscrito");
  }
}

#endif // CONFIG_MQTT_H
