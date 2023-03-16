/*
*    AUTOR:		  BrincandoComIdeias
*    APRENDA: 	https://cursodearduino.net/
*    SKETCH:    Controle de Umidade do Ar
*    DATA:		  27/02/2023
*/

// INCLUSÃO DE BIBLIOTECAS
#include "DHT.h"
#include <MicroLCD.h>

// INCLUSÃO DE ARQUIVOS
#include "icones.h"

// DEFINIÇÕES DE PINOS
#define pinDHT 7
#define pinUmid 8
#define pinDesumid 9

// DEFINIÇÕES
#define UR_MAX 60    // Umidade Relativa Maxima
#define UR_IDEAL 55  // Umidade Alvo
#define UR_MIN 50    // Umidade Relativa Minima

#define INTERVALO_LEITURA 10000  // Intervalo minimo de leitura do DHT22 ~2s \
                                // Intervalo minimo do AdafruitIO ~7s

// INSTANCIANDO OBJETOS
DHT dht(pinDHT, DHT22);
LCD_SH1106 lcd;  //LCD_SSD1306   //LCD_SH1106   //LCD_PCD8544

// DECLARAÇÃO DE VARIÁVEIS
float umidade = 55.0;
float umidadeAnt = 55.0;

int estado = 0;  // 0 = Ar ideal | -1 = Umidificador ligado | 1 = Ar condicionado Ligado
unsigned long controleLeitura;

void setup() {
  Serial.begin(115200);

  // Configura os pinos como OUTPUT
  pinMode(pinDesumid, OUTPUT);
  pinMode(pinUmid, OUTPUT);

  digitalWrite(pinUmid, LOW);
  digitalWrite(pinDesumid, LOW);

  // Inicializa o Sensor
  dht.begin();

  // Inicializa o Display e limpa a tela
  lcd.begin();
  lcd.clear();

  // Posiciona o Cursos e Imprime o Logo
  lcd.setCursor(39, 1);
  lcd.draw(logo, 48, 48);

  // Espera alguns segundos com o Logo na tela
  delay(3000);

  // Remove o Logo da tela
  lcd.clear();

  // Posiciona o Cursos e Imprime o Icone de Umidade Relativa
  lcd.setCursor(95, 0);
  lcd.draw(humidity, 32, 32);
}

void loop() {

  if (millis() - controleLeitura > INTERVALO_LEITURA) {
    float leitura = dht.readHumidity();
    controleLeitura = millis();

    lcd.setCursor(45, 1);

    if (!isnan(leitura)) {  // Confere se recebeu algum valor do sensor
      umidade = leitura;

      lcd.setFontSize(FONT_SIZE_XLARGE);
      lcd.print(umidade);
      lcd.setFontSize(FONT_SIZE_SMALL);

    } else {
      lcd.setCursor(0, 6);
      lcd.println(F("                  "));
      lcd.println(F("-->Falha no DHT<--"));
      delay(1000);
    }

    switch (estado) {
      case 0:                    // Leitura Anterior = Indicando Umidade ideal | Umidificador e Ar desligados
        if (umidade < UR_MIN) {  // Se Leitura Atual = Indicando Ar muito Seco
          estado = -1;

          digitalWrite(pinUmid, HIGH);

          lcd.setCursor(0, 6);
          lcd.println(F("Umidif.: Ligado   "));
          lcd.println(F("Desumi.: Desligado"));

        } else if (umidade > UR_MAX) {  // Se Leitura Atual = Indicando Ar muito umido
          estado = 1;

          digitalWrite(pinDesumid, HIGH);

          lcd.setCursor(0, 6);
          lcd.println(F("Umidif.: Desligado"));
          lcd.println(F("Desumi.: ligado   "));
        }
        break;

      case -1:                      // Leitura Anterior = Indicando Umidade baixa | Umidificador ligado
        if (umidade >= UR_IDEAL) {  // Se Leitura Atual = Indicando Umidade subiu para o nível ideal
          estado = 0;

          digitalWrite(pinUmid, LOW);

          lcd.setCursor(0, 6);
          lcd.println(F("Umidif.: Desligado"));
          lcd.println(F("Desumi.: Desligado"));
        }
        break;

      case 1:                       // Leitura Anterior = Indicando Umidade alta | Ar ligado
        if (umidade <= UR_IDEAL) {  // Se Leitura Atual = Indicando Umidade abaixou para o nível ideal
          estado = 0;

          digitalWrite(pinDesumid, LOW);

          lcd.setCursor(0, 6);
          lcd.println(F("Umidif.: Desligado"));
          lcd.println(F("Desumi.: Desligado"));
        }
        break;
    }
  }

  // VERIFICA SE HOUVE VARIAÇÃO SIGNIFICATIVA NA LEITURA
  if (umidade != umidadeAnt) {
    // ATUALIZA O ÍCONE NO DISPLAY
    if (umidade >= UR_MIN && umidade <= UR_MAX) {
      lcd.setCursor(0, 0);
      lcd.draw(good, 32, 32);
    } else {
      lcd.setCursor(0, 0);
      lcd.draw(bad, 32, 32);
    }

    // ENVIA PARA O ESP AS INFORMAÇÕES ATUAIS
    Serial.print(umidade);
    Serial.print(';');
    Serial.print(digitalRead(pinUmid));
    Serial.print(';');
    Serial.print(digitalRead(pinDesumid));
    Serial.print(';');
  }

  umidadeAnt = umidade;
}
