// Função para processar mensagens MQTT recebidas
void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) {
    incommingMessage += (char)payload[i];
  }
  
  Serial.println("Mensagem chegando [" + String(topic) + "] " + incommingMessage);

  // Processamento das mensagens recebidas
  if (String(topic) == sub1) {
    if (incommingMessage == "1") {
      digitalWrite(RelayPin1, LOW);  // LOW = ligado
      estadoRele_1 = 0;
      Serial.println("Dispositivo 1 LIGADO");
    } else if (incommingMessage == "0") {
      digitalWrite(RelayPin1, HIGH);   // HIGH = desligado
      estadoRele_1 = 1;
      Serial.println("Dispositivo 1 DESLIGADO");
    }
  }

  else if (String(topic) == sub2) {
    if (incommingMessage == "1") {
      digitalWrite(RelayPin2, LOW);
      estadoRele_2 = 0;
      Serial.println("Dispositivo 2 LIGADO");
    } else if (incommingMessage == "0") {
      digitalWrite(RelayPin2, HIGH);
      estadoRele_2 = 1;
      Serial.println("Dispositivo 2 DESLIGADO");
    }
  }

  else if (String(topic) == sub3) {
    if (incommingMessage == "1") {
      digitalWrite(RelayPin3, LOW);
      estadoRele_3 = 0;
      Serial.println("Dispositivo 3 LIGADO");
    } else if (incommingMessage == "0") {
      digitalWrite(RelayPin3, HIGH);
      estadoRele_3 = 1;
      Serial.println("Dispositivo 3 DESLIGADO");
    }
  }

  else if (String(topic) == sub4) {
    if (incommingMessage == "1") {
      digitalWrite(RelayPin4, LOW);
      estadoRele_4 = 0;
      Serial.println("Dispositivo 4 LIGADO");
    } else if (incommingMessage == "0") {
      digitalWrite(RelayPin4, HIGH);
      estadoRele_4 = 1;
      Serial.println("Dispositivo 4 DESLIGADO");
    }
  }

  else if (String(topic) == sub5) {
    if (incommingMessage == "1") {
      digitalWrite(RelayPin5, LOW);
      estadoRele_5 = 0;
      Serial.println("Dispositivo 5 LIGADO");
    } else if (incommingMessage == "0") {
      digitalWrite(RelayPin5, HIGH);
      estadoRele_5 = 1;
      Serial.println("Dispositivo 5 DESLIGADO");
    }
  }

  else if (String(topic) == sub6) {
    if (incommingMessage == "1") {
      digitalWrite(RelayPin6, LOW);
      estadoRele_6 = 0;
      Serial.println("Dispositivo 6 LIGADO");
    } else if (incommingMessage == "0") {
      digitalWrite(RelayPin6, HIGH);
      estadoRele_6 = 1;
      Serial.println("Dispositivo 6 DESLIGADO");
    }
  }

  else if (String(topic) == sub7) {
    if (incommingMessage == "1") {
      digitalWrite(RelayPin7, LOW);
      estadoRele_7 = 0;
      Serial.println("Dispositivo 7 LIGADO");
    } else if (incommingMessage == "0") {
      digitalWrite(RelayPin7, HIGH);
      estadoRele_7 = 1;
      Serial.println("Dispositivo 7 DESLIGADO");
    }
  }

  else if (String(topic) == sub8) {
    if (incommingMessage == "1") {
      digitalWrite(RelayPin8, LOW);
      estadoRele_8 = 0;
      Serial.println("Dispositivo 8 LIGADO");
    } else if (incommingMessage == "0") {
      digitalWrite(RelayPin8, HIGH);
      estadoRele_8 = 1;
      Serial.println("Dispositivo 8 DESLIGADO");
    }
  }
}
