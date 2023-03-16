/*
*    AUTOR:		  BrincandoComIdeias
*    APRENDA: 	https://cursodearduino.net/
*    SKETCH:    Controle de Umidade do Ar
*    DATA:		  27/02/2023
*/

// Placa Generic ESP8266 v3.0.2
#include "config.h"

// INSTANCIANDO OBJETOS
AdafruitIO_Feed *feedUmidade = io.feed("umidade");
AdafruitIO_Feed *feedUmidificador = io.feed("umidificador");
AdafruitIO_Feed *feedDesumidificador = io.feed("desumidificador");

// DECLARAÇÃO DE VARIÁVEIS
float umidade;
bool desumidificador, umidificador;

void setup() {
  Serial.begin(115200);
  delay(2000);

  // CONECTA NO WIFI + ADAFRUITIO
  io.connect();

  // PISCA O LED ENQUANTO TENTA CONECTAR
  while (io.status() < AIO_CONNECTED) {
    delay(500);
  }

}

void loop() {
  io.run();

  // VERIFICA SE TEM ALGUMA INFORMAÇÃO NO SERIAL
  if (Serial.available()) {
    // INCIA A STRING VAZIA
    String recebido = "";

    // LÊ O BUFFER ATÉ O PRÓXIMO ';'
    recebido = Serial.readStringUntil(';');
    umidade = recebido.toFloat();

    // LÊ O BUFFER ATÉ O PRÓXIMO ';'
    recebido = Serial.readStringUntil(';');
    umidificador = recebido.toInt();

    // LÊ O BUFFER ATÉ O PRÓXIMO';'
    recebido = Serial.readStringUntil(';');
    desumidificador = recebido.toInt();

    // ENVIA PARA O ADAFRUIT IO
    feedUmidade->save(umidade);
    feedUmidificador->save(umidificador);
    feedDesumidificador->save(desumidificador);
  }
}
