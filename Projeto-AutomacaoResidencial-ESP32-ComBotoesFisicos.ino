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

// Variáveis para armazenar o estado dos relés
int estadoRele_1 = 1;
int estadoRele_2 = 1;
int estadoRele_3 = 1;
int estadoRele_4 = 1;
int estadoRele_5 = 1;
int estadoRele_6 = 1;
int estadoRele_7 = 1;
int estadoRele_8 = 1;

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
    if (bmp.begin()) {
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
    } else {
      Serial.println("Erro na leitura do sensor BMP180");
    }
  }
}

// =============== DEBOUNCE DOS BOTÕES ===============
#define NUM_BOTOES 8
const uint8_t botoes[NUM_BOTOES] = {SwitchPin1, SwitchPin2, SwitchPin3, SwitchPin4, SwitchPin5, SwitchPin6, SwitchPin7, SwitchPin8};
unsigned long lastDebounceTime[NUM_BOTOES] = {0};
uint8_t lastButtonState[NUM_BOTOES] = {HIGH};
uint8_t buttonState[NUM_BOTOES] = {HIGH};
#define DEBOUNCE_DELAY 50UL

// Estados dos relés já definidos: estadoRele_1 ... estadoRele_8
// Tópicos já definidos: pub1 ... pub8

void controle_manual() {
  for (int i = 0; i < NUM_BOTOES; i++) {
    int leitura = digitalRead(botoes[i]);
    if (leitura != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
      lastButtonState[i] = leitura;
    }
    if ((millis() - lastDebounceTime[i]) > DEBOUNCE_DELAY) {
      if (leitura == LOW && buttonState[i] == HIGH) { // botão pressionado
        // Alterna o relé correspondente
        switch (i) {          case 0:
            if (estadoRele_1 == 0) {
              digitalWrite(RelayPin1, LOW);  // Inverte a lógica: LOW desliga
              estadoRele_1 = 1;
              client.publish(pub1, "0", MQTT_RETAIN_RELE);  // 0 = desligado
              Serial.println("Dispositivo 1 DESLIGADO");
            } else {
              digitalWrite(RelayPin1, HIGH);  // Inverte a lógica: HIGH liga
              estadoRele_1 = 0;
              client.publish(pub1, "1", MQTT_RETAIN_RELE);  // 1 = ligado
              Serial.println("Dispositivo 1 LIGADO");
            }
            break;          case 1:
            if (estadoRele_2 == 0) {
              digitalWrite(RelayPin2, LOW);  // LOW = desligado
              estadoRele_2 = 1;
              client.publish(pub2, "0", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 2 DESLIGADO");
            } else {
              digitalWrite(RelayPin2, HIGH);  // HIGH = ligado
              estadoRele_2 = 0;
              client.publish(pub2, "1", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 2 LIGADO");
            }
            break;
          case 2:
            if (estadoRele_3 == 0) {
              digitalWrite(RelayPin3, HIGH);
              estadoRele_3 = 1;
              client.publish(pub3, "1", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 3 DESLIGADO");
            } else {
              digitalWrite(RelayPin3, LOW);
              estadoRele_3 = 0;
              client.publish(pub3, "0", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 3 LIGADO");
            }
            break;
          case 3:
            if (estadoRele_4 == 0) {
              digitalWrite(RelayPin4, HIGH);
              estadoRele_4 = 1;
              client.publish(pub4, "1", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 4 DESLIGADO");
            } else {
              digitalWrite(RelayPin4, LOW);
              estadoRele_4 = 0;
              client.publish(pub4, "0", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 4 LIGADO");
            }
            break;
          case 4:
            if (estadoRele_5 == 0) {
              digitalWrite(RelayPin5, HIGH);
              estadoRele_5 = 1;
              client.publish(pub5, "1", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 5 DESLIGADO");
            } else {
              digitalWrite(RelayPin5, LOW);
              estadoRele_5 = 0;
              client.publish(pub5, "0", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 5 LIGADO");
            }
            break;
          case 5:
            if (estadoRele_6 == 0) {
              digitalWrite(RelayPin6, HIGH);
              estadoRele_6 = 1;
              client.publish(pub6, "1", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 6 DESLIGADO");
            } else {
              digitalWrite(RelayPin6, LOW);
              estadoRele_6 = 0;
              client.publish(pub6, "0", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 6 LIGADO");
            }
            break;
          case 6:
            if (estadoRele_7 == 0) {
              digitalWrite(RelayPin7, HIGH);
              estadoRele_7 = 1;
              client.publish(pub7, "1", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 7 DESLIGADO");
            } else {
              digitalWrite(RelayPin7, LOW);
              estadoRele_7 = 0;
              client.publish(pub7, "0", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 7 LIGADO");
            }
            break;
          case 7:
            if (estadoRele_8 == 0) {
              digitalWrite(RelayPin8, HIGH);
              estadoRele_8 = 1;
              client.publish(pub8, "1", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 8 DESLIGADO");
            } else {
              digitalWrite(RelayPin8, LOW);
              estadoRele_8 = 0;
              client.publish(pub8, "0", MQTT_RETAIN_RELE);
              Serial.println("Dispositivo 8 LIGADO");
            }
            break;
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

  // Inicializa BMP180 (I2C padrão)
  if (!bmp.begin()) {
    Serial.println("BMP180 não encontrado! Verifique a conexão.");
  } else {
    Serial.println("BMP180 inicializado com sucesso.");
  }

  // Configura os pinos dos relés como saída
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);
  pinMode(RelayPin5, OUTPUT);
  pinMode(RelayPin6, OUTPUT);
  pinMode(RelayPin7, OUTPUT);
  pinMode(RelayPin8, OUTPUT);

  // Configura os pinos dos botões como entrada com pull-up
  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);
  pinMode(SwitchPin5, INPUT_PULLUP);
  pinMode(SwitchPin6, INPUT_PULLUP);
  pinMode(SwitchPin7, INPUT_PULLUP);
  pinMode(SwitchPin8, INPUT_PULLUP);

  pinMode(wifiLed, OUTPUT);
  // Ao iniciar, todos os relés desligados
  digitalWrite(RelayPin1, HIGH);  // HIGH = desligado
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);
  digitalWrite(RelayPin4, HIGH);
  digitalWrite(RelayPin5, HIGH);
  digitalWrite(RelayPin6, HIGH);
  digitalWrite(RelayPin7, HIGH);
  digitalWrite(RelayPin8, HIGH);

  // LED WiFi desligado
  digitalWrite(wifiLed, HIGH);
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
    taskDHT,         // Função da task
    "TaskDHT",       // Nome
    4096,            // Stack size
    NULL,            // Param
    1,               // Prioridade
    NULL,            // Handle
    0                // Core 0
  );
}

void loop() {
  if (!client.connected()) {
    digitalWrite(wifiLed, HIGH);
    reconectar_mqtt();
  } else {
    digitalWrite(wifiLed, LOW);
    controle_manual();
    // readSensors(); // AGORA É FEITO NA TASK DO CORE 0
  }  client.loop();

  // Mantém OTA responsivo sem travar o loop principal
  ArduinoOTA.handle();
}