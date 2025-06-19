// ConfigBMP180.h
// Definições e configuração do sensor BMP180

#ifndef CONFIG_BMP180_H
#define CONFIG_BMP180_H

#include <Adafruit_BMP085.h>

#define BMP_SDA 21
#define BMP_SCL 22
#define BMP_READ_INTERVAL (2 * 60000UL) // 2 minutos

Adafruit_BMP085 bmp;

// Pressão ao nível do mar para cálculo de altitude (em hPa)
#define PRESSAO_NIVEL_MAR 1013.25

#endif // CONFIG_BMP180_H
