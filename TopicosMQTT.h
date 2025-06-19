// TopicosMQTT.h
// Definição dos tópicos MQTT utilizados no projeto

#ifndef TOPICOS_MQTT_H
#define TOPICOS_MQTT_H

// Tópicos Subscribe (recebimento)
const char* sub0 = "ESP32/MinhaCasa/QuartoRobson/Ligar-DesligarTudo/Estado";
const char* sub1 = "ESP32/MinhaCasa/QuartoRobson/Interruptor1/Estado";
const char* sub2 = "ESP32/MinhaCasa/QuartoRobson/Interruptor2/Estado";
const char* sub3 = "ESP32/MinhaCasa/QuartoRobson/Interruptor3/Estado";
const char* sub4 = "ESP32/MinhaCasa/QuartoRobson/Interruptor4/Estado";
const char* sub5 = "ESP32/MinhaCasa/QuartoRobson/Interruptor5/Estado";
const char* sub6 = "ESP32/MinhaCasa/QuartoRobson/Interruptor6/Estado";
const char* sub7 = "ESP32/MinhaCasa/QuartoRobson/Interruptor7/Estado";
const char* sub8 = "ESP32/MinhaCasa/QuartoRobson/Interruptor8/Estado";
// Tópicos Publish (envio)
const char* pub1 = "ESP32/MinhaCasa/QuartoRobson/Interruptor1/Comando";
const char* pub2 = "ESP32/MinhaCasa/QuartoRobson/Interruptor2/Comando";
const char* pub3 = "ESP32/MinhaCasa/QuartoRobson/Interruptor3/Comando";
const char* pub4 = "ESP32/MinhaCasa/QuartoRobson/Interruptor4/Comando";
const char* pub5 = "ESP32/MinhaCasa/QuartoRobson/Interruptor5/Comando";
const char* pub6 = "ESP32/MinhaCasa/QuartoRobson/Interruptor6/Comando";
const char* pub7 = "ESP32/MinhaCasa/QuartoRobson/Interruptor7/Comando";
const char* pub8 = "ESP32/MinhaCasa/QuartoRobson/Interruptor8/Comando";
const char* pub9  = "ESP32/MinhaCasa/QuartoRobson/Temperatura";
const char* pub10 = "ESP32/MinhaCasa/QuartoRobson/Umidade";
const char* pub11 = "ESP32/MinhaCasa/QuartoRobson/SensacaoTermica";
// Sensor BMP180 (Temperatura, Pressão Atmosférica e Altitude)
const char* pub12 = "ESP32/MinhaCasa/QuartoRobson/BMP180/Temperatura";
const char* pub13 = "ESP32/MinhaCasa/QuartoRobson/BMP180/PressaoAtmosferica/Real";
const char* pub14 = "ESP32/MinhaCasa/QuartoRobson/BMP180/PressaoAtmosferica/NivelMar";
const char* pub15 = "ESP32/MinhaCasa/QuartoRobson/BMP180/AltitudeReal";
const char* pub16 = "ESP32/MinhaCasa/QuartoRobson/BMP180/AltitudeNivelMar";

#endif // TOPICOS_MQTT_H
