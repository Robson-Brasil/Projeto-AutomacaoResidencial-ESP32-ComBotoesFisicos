/******************************************************************************************************************************************
  Automação Residencial
  Autor : Robson Brasil

  Dispositivos : ESP32 WROOM32, DHT22, BMP180, Módulo Relé de 8 Canais, Botões Físicos
  Preferences--> URLs adicionais do Gerenciador de placas:
                                    ESP8266: http://arduino.esp8266.com/stable/package_esp8266com_index.json,
                                    ESP32  : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  Download Board ESP32 (x.x.x):
  Broker MQTT, Retain, QoS, KeepAlive
  Atualização OTA - Over-The -Air
  Node-Red / Google Assistant-Nora:  https://smart-nora.eu/
  Para Instalação do Node-Red:       https://nodered.org/docs/getting-started/
  Home Assistant
  Para Instalação do Home Assistant: https://www.home-assistant.io/installation/
  Versão : 1.0 - Release Candidate
  Última Modificação : 19/06/2025
******************************************************************************************************************************************/

#include "Bibliotecas.h"
#include <Wire.h>

// =============== ESTADOS DOS RELÉS (usando arrays) ===============
// Relay modules are active LOW: LOW = ON, HIGH = OFF
const int NUM_RELES = 8;
int estadoReles[NUM_RELES] = {1, 1, 1, 1, 1, 1, 1, 1}; // 1 = OFF, 0 = ON
const uint8_t relayPins[NUM_RELES] = {RelayPin1, RelayPin2, RelayPin3, RelayPin4, RelayPin5, RelayPin6, RelayPin7, RelayPin8};
const char* relayPubTopics[NUM_RELES] = {pub1, pub2, pub3, pub4, pub5, pub6, pub7, pub8};

// As credenciais de WiFi e MQTT estão agora em Credenciais.h

#include "ConfigMQTT.h"

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (80)
char msg[MSG_BUFFER_SIZE];
int value = 0;

// =============== VARIÁVEIS DHT ===============
DHT dht(DHTPIN, DHTTYPE);
unsigned long lastMsgDHT = 0;
#define SENSOR_INTERVAL_MIN 2
#define DHT_READ_INTERVAL (SENSOR_INTERVAL_MIN * 60000UL) // 2 minutos (ajustável)
char str_temp_data[10];
char str_hum_data[10];
char str_tempF_data[10];
char str_tempterm_data[10];

// =============== VARIÁVEIS BMP180 ===============
unsigned long lastMsgBMP180 = 0;
char str_bmp_temp[10];
char str_bmp_pressao[10];
char str_bmp_pressao_nivelmar[10];
char str_bmp_altitude_real[10];
char str_bmp_altitude_total[10];
float altitudeNivelMar = 0;

// =============== Função para ler e publicar dados dos sensores ===============
void readSensors() {
  unsigned long currentTimeDHT = millis();
  if (currentTimeDHT - lastMsgDHT > DHT_READ_INTERVAL) {
    lastMsgDHT = currentTimeDHT;
    float temp_data = dht.readTemperature();
    float hum_data = dht.readHumidity();
    if (isnan(temp_data) || isnan(hum_data)) {
      Serial.println("Falha na leitura do sensor DHT22!");
      return;
    }
    dtostrf(temp_data, 6, 2, str_temp_data);
    dtostrf(hum_data, 6, 2, str_hum_data);
    float tempF_data = dht.readTemperature(true);
    if (!isnan(tempF_data)) {
      dtostrf(tempF_data, 6, 2, str_tempF_data);
      float tempterm_data = dht.computeHeatIndex(tempF_data, hum_data);
      if (!isnan(tempterm_data)) {
        tempterm_data = dht.convertFtoC(tempterm_data);
        dtostrf(tempterm_data, 6, 2, str_tempterm_data);
        client.publish(pub11, str_tempterm_data, MQTT_RETAIN_SENSOR);
      }
    }
    client.publish(pub9, str_temp_data, MQTT_RETAIN_SENSOR);
    client.publish(pub10, str_hum_data, MQTT_RETAIN_SENSOR);
    Serial.println("\n=== Leitura DHT22 ===");
    Serial.print("Temperatura: "); Serial.print(str_temp_data); Serial.println(" °C");
    Serial.print("Umidade: "); Serial.print(str_hum_data); Serial.println(" %");
    Serial.print("Sensação Térmica: "); Serial.print(str_tempterm_data); Serial.println(" °C");
  }

  // =============== LEITURA E PUBLICAÇÃO BMP180 ===============
  unsigned long currentTimeBMP180 = millis();
  if (currentTimeBMP180 - lastMsgBMP180 > BMP_READ_INTERVAL) {
    lastMsgBMP180 = currentTimeBMP180;
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure();
    float seaLevelPressure = bmp.readSealevelPressure(PRESSAO_NIVEL_MAR * 100);
    float altitudeReal = bmp.readAltitude(PRESSAO_NIVEL_MAR * 100);
    float altitudeTotal = altitudeReal + altitudeNivelMar;

    dtostrf(temperature, 6, 2, str_bmp_temp);
    dtostrf(pressure / 100.0, 6, 2, str_bmp_pressao);
    dtostrf(seaLevelPressure / 100.0, 6, 2, str_bmp_pressao_nivelmar);
    dtostrf(altitudeReal, 6, 2, str_bmp_altitude_real);
    dtostrf(altitudeTotal, 6, 2, str_bmp_altitude_total);

    client.publish(pub12, str_bmp_temp, MQTT_RETAIN_SENSOR);
    client.publish(pub13, str_bmp_pressao, MQTT_RETAIN_SENSOR);
    client.publish(pub14, str_bmp_pressao_nivelmar, MQTT_RETAIN_SENSOR);
    client.publish(pub15, str_bmp_altitude_real, MQTT_RETAIN_SENSOR);
    client.publish(pub16, str_bmp_altitude_total, MQTT_RETAIN_SENSOR);

    Serial.println("\n=== Leitura BMP180 ===");
    Serial.print("Temperatura: "); Serial.print(str_bmp_temp); Serial.println(" °C");
    Serial.print("Pressão: "); Serial.print(str_bmp_pressao); Serial.println(" hPa");
    Serial.print("Pressão nível do mar: "); Serial.print(str_bmp_pressao_nivelmar); Serial.println(" hPa");
    Serial.print("Altitude real: "); Serial.print(str_bmp_altitude_real); Serial.println(" m");
    Serial.print("Altitude total: "); Serial.print(str_bmp_altitude_total); Serial.println(" m");
  }
}

// =============== DEBOUNCE DOS BOTÕES ===============
#define NUM_BOTOES 8
const uint8_t botoes[NUM_BOTOES] = {SwitchPin1, SwitchPin2, SwitchPin3, SwitchPin4, SwitchPin5, SwitchPin6, SwitchPin7, SwitchPin8};
unsigned long lastDebounceTime[NUM_BOTOES] = {0};
uint8_t lastButtonState[NUM_BOTOES] = {HIGH};
uint8_t buttonState[NUM_BOTOES] = {HIGH};
#define DEBOUNCE_DELAY 50UL

void controle_manual() {
  for (int i = 0; i < NUM_BOTOES; i++) {
    int leitura = digitalRead(botoes[i]);
    if (leitura != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
      lastButtonState[i] = leitura;
    }
    if ((millis() - lastDebounceTime[i]) > DEBOUNCE_DELAY) {
      if (leitura == LOW && buttonState[i] == HIGH) {
        // Toggle relay state (active LOW: LOW=ON, HIGH=OFF)
        if (estadoReles[i] == 1) {
          // Currently OFF, turn ON
          digitalWrite(relayPins[i], LOW);
          estadoReles[i] = 0;
          client.publish(relayPubTopics[i], "1", MQTT_RETAIN_RELE);
          Serial.printf("Dispositivo %d LIGADO\n", i + 1);
        } else {
          // Currently ON, turn OFF
          digitalWrite(relayPins[i], HIGH);
          estadoReles[i] = 1;
          client.publish(relayPubTopics[i], "0", MQTT_RETAIN_RELE);
          Serial.printf("Dispositivo %d DESLIGADO\n", i + 1);
        }
      }
      buttonState[i] = leitura;
    }
  }
}

// =============== TASK PARA O DHT22 NO CORE 0 ===============
void taskDHT(void *pvParameters) {
  for (;;) {
    readSensors(); // já publica no MQTT
    vTaskDelay(DHT_READ_INTERVAL / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Inicializa I2C e BMP180
  Wire.begin(BMP_SDA, BMP_SCL);
  if (!bmp.begin()) {
    Serial.println("BMP180 não encontrado! Verifique a conexão.");
  } else {
    Serial.println("BMP180 inicializado com sucesso.");
  }

  // Configura os pinos dos relés como saída
  for (int i = 0; i < NUM_RELES; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // HIGH = desligado (active LOW)
  }

  // Configura os pinos dos botões como entrada com pull-up
  for (int i = 0; i < NUM_BOTOES; i++) {
    pinMode(botoes[i], INPUT_PULLUP);
  }

  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH); // LED WiFi desligado
  conectar_wifi();
  
  // Inicializa MDNS
  if (!MDNS.begin("ESP32-OTA")) {
    Serial.println("Erro ao iniciar mDNS");
    return;
  }
  Serial.println("mDNS iniciado");

  client.setServer(mqttServer, 1883);
  client.setCallback(callback);

  // Inicializa OTA após WiFi estar conectado
  setupOTA("ESP32-OTA");

  xTaskCreatePinnedToCore(
    taskDHT,
    "TaskDHT",
    4096,
    NULL,
    1,
    NULL,
    0
  );
}

void loop() {
  if (!client.connected()) {
    digitalWrite(wifiLed, HIGH);
    reconectar_mqtt();
  } else {
    digitalWrite(wifiLed, LOW);
    controle_manual();
  }
  client.loop();
  ArduinoOTA.handle();
}