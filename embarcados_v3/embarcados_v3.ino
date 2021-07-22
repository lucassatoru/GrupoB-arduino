/*
    SISTEMA DE AUTOMAÇÃO RESIDENCIAL
*/

//include "definicoes_sistema.h"
#include <SoftwareSerial.h>

/***********************************************************************
Define
 ***********************************************************************/
#define true  1
#define false 0


#define NUM_ESTADOS 3
#define NUM_EVENTOS 8

// ESTADOS
#define ALARME_DESLIGADO   0
#define ALARME_LIGADO      1
#define ALARME_ACIONADO    2

// EVENTOS
#define NENHUM_EVENTO -1
#define LIGAR1         0
#define LIGAR2         1
#define DESLIGAR1      2
#define DESLIGAR2      3
#define LIGAR_S        4
#define DESLIGAR_S     5
#define DETECTAR       6
#define NAO_DETECTAR   7

// ACOES
#define NENHUMA_ACAO -1
#define A01           0
#define A02           1
#define A03           2
#define A04           3
#define A05           4
#define A06           5
#define A07           6
#define A08           7
#define A09           8

// PINOS
#define lampada1      8
#define lampada2      9
#define buzzer        7
#define ativa_sensor  6
#define le_sensor     3

/***********************************************************************
 Estaticos
 ***********************************************************************/
  int codigoEvento = NENHUM_EVENTO;
  int eventoInterno = NENHUM_EVENTO;
  int estado = ALARME_DESLIGADO;
  int codigoAcao;
  int acao_matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS];
  int proximo_estado_matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS];
  int presenca = false;
  int sensor_ativo = false;
  char caracter = 'A';

  SoftwareSerial mySerial(0, 1); // 0-RX, 1-TX

//  int lampada1 = 8; // Define a porta Digital 8 como lampada1.
//  int lampada2 = 9; // Define a porta Digital 9 como lampada2.
//  int buzzer = 7; // Define a porta Digital 8 como buzzer.
//  int ativa_sensor = 6; // Define a porta Digital 6 como ativa_sensor.
//  int le_sensor = 5; // Define a porta Digital 6 como le_sensor.

/************************************************************************
 detecta_presenca
 Função para tratamento de interrupção.
 Parametros de entrada: nenhum
 Retorno: nenhum
*************************************************************************/
// Interrupt Service Routine (ISR)
void detecta_presenca(){
  if(digitalRead(le_sensor) == HIGH) {
    presenca = true;
  }
  else{
    presenca = false; 
  }
}

/************************************************************************
 executarAcao
 Executa uma acao
 Parametros de entrada:
    (int) codigo da acao a ser executada
 Retorno: (int) codigo do evento interno ou NENHUM_EVENTO
*************************************************************************/
int executarAcao(int codigoAcao)
{
    int retval;

    retval = NENHUM_EVENTO;
    if (codigoAcao == NENHUMA_ACAO)
        return retval;

    switch(codigoAcao)
    {
    case A01:
        digitalWrite(lampada1, HIGH); // Liga lâmpada 1.
        break;
    case A02:
        digitalWrite(lampada2, HIGH); // Liga lâmpada 2.
        break;
    case A03:
        digitalWrite(lampada1, LOW); // Desliga lâmpada 1.
        break;
    case A04:
        digitalWrite(lampada2, LOW); // Desliga lâmpada 2.
        break;
    case A05:
        digitalWrite(ativa_sensor, HIGH); // Liga o sensor.
        sensor_ativo = true;
        break;
    case A06:
        digitalWrite(ativa_sensor, LOW); // Desliga sensor.
        sensor_ativo = false;
        break;
    case A07:
        digitalWrite(lampada1, LOW); // Liga lâmpada 1.
        digitalWrite(lampada2, LOW); // Liga lâmpada 2.
        digitalWrite(buzzer, LOW); // Liga sirene.
        break;
    case A08:
        digitalWrite(ativa_sensor, LOW); // Desliga sensor.
        sensor_ativo = false;
        digitalWrite(lampada1, HIGH); // Desliga lâmpada 1.
        digitalWrite(lampada2, HIGH); // Desliga lâmpada 2.
        digitalWrite(buzzer, HIGH); // Desliga sirene.
        break;
    case A09:
        digitalWrite(lampada1, HIGH); // Desliga lâmpada 1.
        digitalWrite(lampada2, HIGH); // Desliga lâmpada 2.
        digitalWrite(buzzer, HIGH); // Desliga sirene.
        break;
    } // switch

    return retval;
} // executarAcao

/************************************************************************
 iniciaMaquina de Estados
 Carrega a maquina de estados
 Parametros de entrada: nenhum
 Retorno: nenhum
*************************************************************************/
void iniciaMaquinaEstados()
{
  int i;
  int j;

  for (i=0; i < NUM_ESTADOS; i++) {
    for (j=0; j < NUM_EVENTOS; j++) {
       acao_matrizTransicaoEstados[i][j] = NENHUMA_ACAO;
       proximo_estado_matrizTransicaoEstados[i][j] = i;
    }
  }
  //proximo_estado_matrizTransicaoEstados[ALARME_DESLIGADO][LIGAR1] = ALARME_DESLIGADO;
  acao_matrizTransicaoEstados[ALARME_DESLIGADO][LIGAR1] = A01;

  //proximo_estado_matrizTransicaoEstados[ALARME_DESLIGADO][LIGAR2] = ALARME_DESLIGADO;
  acao_matrizTransicaoEstados[ALARME_DESLIGADO][LIGAR2] = A02;

  //proximo_estado_matrizTransicaoEstados[ALARME_DESLIGADO][DESLIGAR1] = ALARME_DESLIGADO;
  acao_matrizTransicaoEstados[ALARME_DESLIGADO][DESLIGAR1] = A03;

  //proximo_estado_matrizTransicaoEstados[ALARME_DESLIGADO][DESLIGAR2] = ALARME_DESLIGADO;
  acao_matrizTransicaoEstados[ALARME_DESLIGADO][DESLIGAR2] = A04;
  
  proximo_estado_matrizTransicaoEstados[ALARME_DESLIGADO][LIGAR_S] = ALARME_LIGADO;
  acao_matrizTransicaoEstados[ALARME_DESLIGADO][LIGAR_S] = A05;

  //proximo_estado_matrizTransicaoEstados[ALARME_DESLIGADO][DESLIGAR_S] = ALARME_DESLIGADO;
  //acao_matrizTransicaoEstados[ALARME_DESLIGADO][DESLIGAR_S] = NENHUMA_ACAO;

  //proximo_estado_matrizTransicaoEstados[ALARME_DESLIGADO][DETECTAR] = ALARME_DESLIGADO;
  //acao_matrizTransicaoEstados[ALARME_DESLIGADO][DETECTAR] = NENHUMA_ACAO;

  //proximo_estado_matrizTransicaoEstados[ALARME_DESLIGADO][NAO_DETECTAR] = ALARME_DESLIGADO;
  //acao_matrizTransicaoEstados[ALARME_DESLIGADO][NAO_DETECTAR] = NENHUMA_ACAO;

  

  //proximo_estado_matrizTransicaoEstados[ALARME_LIGADO][LIGAR1] = ALARME_LIGADO;
  acao_matrizTransicaoEstados[ALARME_LIGADO][LIGAR1] = A01;

  //proximo_estado_matrizTransicaoEstados[ALARME_LIGADO][LIGAR2] = ALARME_LIGADO;
  acao_matrizTransicaoEstados[ALARME_LIGADO][LIGAR2] = A02;

  //proximo_estado_matrizTransicaoEstados[ALARME_LIGADO][DESLIGAR1] = ALARME_LIGADO;
  acao_matrizTransicaoEstados[ALARME_LIGADO][DESLIGAR1] = A03;

  //proximo_estado_matrizTransicaoEstados[ALARME_LIGADO][DESLIGAR2] = ALARME_LIGADO;
  acao_matrizTransicaoEstados[ALARME_LIGADO][DESLIGAR2] = A04;
  
  //proximo_estado_matrizTransicaoEstados[ALARME_LIGADO][LIGAR_S] = ALARME_LIGADO;
  //acao_matrizTransicaoEstados[ALARME_LIGADO][LIGAR_S] = NENHUMA_ACAO;

  proximo_estado_matrizTransicaoEstados[ALARME_LIGADO][DESLIGAR_S] = ALARME_DESLIGADO;
  acao_matrizTransicaoEstados[ALARME_LIGADO][DESLIGAR_S] = A06;

  proximo_estado_matrizTransicaoEstados[ALARME_LIGADO][DETECTAR] = ALARME_ACIONADO;
  acao_matrizTransicaoEstados[ALARME_LIGADO][DETECTAR] = A07;

  //proximo_estado_matrizTransicaoEstados[ALARME_LIGADO][NAO_DETECTAR] = ALARME_LIGADO;
  //acao_matrizTransicaoEstados[ALARME_LIGADO][NAO_DETECTAR] = NENHUMA_ACAO;




//  proximo_estado_matrizTransicaoEstados[ALARME_ACIONADO][LIGAR1] = ALARME_ACIONADO;
//  acao_matrizTransicaoEstados[ALARME_ACIONADO][LIGAR1] = NENHUMA_ACAO;
//
//  proximo_estado_matrizTransicaoEstados[ALARME_ACIONADO][LIGAR2] = ALARME_ACIONADO;
//  acao_matrizTransicaoEstados[ALARME_ACIONADO][LIGAR2] = NENHUMA_ACAO;
//
//  proximo_estado_matrizTransicaoEstados[ALARME_ACIONADO][DESLIGAR1] = ALARME_ACIONADO;
//  acao_matrizTransicaoEstados[ALARME_ACIONADO][DESLIGAR1] = NENHUMA_ACAO;
//
//  proximo_estado_matrizTransicaoEstados[ALARME_ACIONADO][DESLIGAR2] = ALARME_ACIONADO;
//  acao_matrizTransicaoEstados[ALARME_ACIONADO][DESLIGAR2] = NENHUMA_ACAO;
//  
//  proximo_estado_matrizTransicaoEstados[ALARME_ACIONADO][LIGAR_S] = ALARME_ACIONADO;
//  acao_matrizTransicaoEstados[ALARME_ACIONADO][LIGAR_S] = NENHUMA_ACAO;
//
  proximo_estado_matrizTransicaoEstados[ALARME_ACIONADO][DESLIGAR_S] = ALARME_DESLIGADO;
  acao_matrizTransicaoEstados[ALARME_ACIONADO][DESLIGAR_S] = A08;
//
//  proximo_estado_matrizTransicaoEstados[ALARME_ACIONADO][DETECTAR] = ALARME_ACIONADO;
//  acao_matrizTransicaoEstados[ALARME_ACIONADO][DETECTAR] = NENHUMA_ACAO;

  proximo_estado_matrizTransicaoEstados[ALARME_ACIONADO][NAO_DETECTAR] = ALARME_LIGADO;
  acao_matrizTransicaoEstados[ALARME_ACIONADO][NAO_DETECTAR] = A09;
} // initStateMachine


/************************************************************************
 iniciaSistema
 Inicia o sistema ...
 Parametros de entrada: nenhum
 Retorno: nenhum
*************************************************************************/
void iniciaSistema()
{
   iniciaMaquinaEstados();
} // initSystem


/************************************************************************
 obterEvento
 Obtem um evento, que pode ser da IHM ou do alarme
 Parametros de entrada: nenhum
 Retorno: codigo do evento
*************************************************************************/
//char* teclas;

//char caracter = mySerial.read(); // Faz a leitura da caractere
//if (caracter == 'A') { // Se identificada caractere 'A'.
// digitalWrite(rele, HIGH); // Liga làmpada 1.
// }
//if (caracter == 'a') { // Se identificada caractere 'a'.
// digitalWrite(rele, LOW); // Desliga lâmpada 1.
// }
//if (caracter == 'B') { // Se identificada caractere 'B'.
// digitalWrite(rele1, HIGH); // Liga lâmpada 2.
// }
//if (caracter == 'b') { // Se identificada caractere 'b'.
// digitalWrite(rele1, LOW); // Desliga lâmpada 2.
// }
//if (caracter == 'C') { // Se identificada caractere 'C'.
// digitalWrite(rele2, HIGH); // Liga sensor.
// }
//if (caracter == 'c') { // Se identificada caractere 'c'.
// digitalWrite(rele2, LOW); // Desliga sensor.
// }
//if (caracter == 'D') { // Se identificada caractere 'D'.
// digitalWrite(rele3, HIGH); // Liga sirene.
// }
//if (caracter == 'd') { // Se identificada caractere 'd'.
// digitalWrite(rele3, LOW); // Desliga sirene.
// }

int obterEvento(char tecla, int sensor, int detecta_presenca)
{
  int retval = NENHUM_EVENTO;
//  int sensor = digitalRead(le_sensor);
//  char caracter = mySerial.read(); // Faz a leitura da caractere

// Sensor desligado
  if (caracter == 'A' && sensor == false){
    return LIGAR1;
  }
  else if (tecla == 'a' && sensor == false){
    return DESLIGAR1;
  }
  else if (tecla == 'B' && sensor == false){
    return LIGAR2;
  }
  else if (tecla == 'b' && sensor == false){
    return DESLIGAR2;
  }
  else if (tecla == 'C' && sensor == false){
    return LIGAR_S;
  }
  
// Sensor ligado sem detectar presença
  else if (caracter == 'A' && sensor == true && detecta_presenca == false){
    return LIGAR1;
  }
  else if (tecla == 'a' && sensor == true && detecta_presenca == false){
    return DESLIGAR1;
  }
  else if (tecla == 'B' && sensor == true && detecta_presenca == false){
    return LIGAR2;
  }
  else if (tecla == 'b' && sensor == true && detecta_presenca == false){
    return DESLIGAR2;
  }
  else if (tecla == 'c' && sensor == true && detecta_presenca == false){
    return DESLIGAR_S;
  }
  else if (tecla == 'c' && sensor == true){
    return DESLIGAR_S;
//  if (tecla == 'd'){
//    return DESATIVAR;
  }

// Sensor ligado detectando presença
  else if (sensor == true && detecta_presenca == true){
    return DETECTAR;
  }

// Sensor ligado sem detectar presença e sem receber comando serial
  else if (sensor == false && detecta_presenca == false) {
    return NAO_DETECTAR;
  }
  else {
    return retval;
  }
  return retval;

} // obterEvento


/************************************************************************
 obterAcao
 Obtem uma acao da Matriz de transicao de estados
 Parametros de entrada: estado (int)
                        evento (int)
 Retorno: codigo da acao
*************************************************************************/
int obterAcao(int estado, int codigoEvento) {
  return acao_matrizTransicaoEstados[estado][codigoEvento];
} // obterAcao


/************************************************************************
 obterProximoEstado
 Obtem o proximo estado da Matriz de transicao de estados
 Parametros de entrada: estado (int)
                        evento (int)
 Retorno: codigo do estado
*************************************************************************/
int obterProximoEstado(int estado, int codigoEvento) {
  return proximo_estado_matrizTransicaoEstados[estado][codigoEvento];
} // obterAcao


/************************************************************************
 Main
 Loop principal de controle que executa a maquina de estados
 Parametros de entrada: nenhum
 Retorno: nenhum
*************************************************************************/

void setup() {
  mySerial.begin(9600);
  pinMode(lampada1, OUTPUT); // Declara pino 8 (lampada1) como um pino de saída.
  pinMode(lampada2, OUTPUT); // Declara pino 9 (lampada2) como um pino de saída.
  pinMode(buzzer, OUTPUT); // Declara pino 7 (buzzer) como um pino de saída.
  pinMode(ativa_sensor, OUTPUT); // Declara pino 6 (ativa_sensor)  como um pino de saída.
  pinMode(le_sensor, INPUT); // Declara pino 3 (le_sensor) como um pino de entrada.
  attachInterrupt(digitalPinToInterrupt(le_sensor), detecta_presenca, CHANGE);  //interrupção para tocar alarme quando detecta presença

  iniciaSistema();
  Serial.println("Sistema iniciado");
} // setup

void loop() {
  caracter = mySerial.read(); // Faz a leitura da caractere
  if (eventoInterno == NENHUM_EVENTO) {
      codigoEvento = obterEvento(caracter, sensor_ativo, presenca);
  } else {
      codigoEvento = eventoInterno;
  }
  if (codigoEvento != NENHUM_EVENTO)
  {
      codigoAcao = obterAcao(estado, codigoEvento);
      estado = obterProximoEstado(estado, codigoEvento);
      eventoInterno = executarAcao(codigoAcao);
      Serial.print("Estado: ");
      Serial.print(estado);
      Serial.print(" Evento: ");
      Serial.print(codigoEvento);
      Serial.print(" Acao: ");
      Serial.println(codigoAcao);
  }
} // loop
