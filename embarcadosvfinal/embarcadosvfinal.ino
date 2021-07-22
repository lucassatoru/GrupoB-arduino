/*
    SISTEMA DE AUTOMAÇÃO RESIDENCIAL
*/

#include <SoftwareSerial.h>

/***********************************************************************
Define
 ***********************************************************************/
#define true  1
#define false 0


#define NUM_ESTADOS 3
#define NUM_EVENTOS 7

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
  int presenca = false; // Variável global que indica detecção de presença pelo sensor.
  int sensor_ativo = false; // Variável global que indica se o sensor está ativo ou não.
  char caracter = 'A'; // Variável global para leitura de caracteres por serial, começa como 'A'.

  SoftwareSerial mySerial(0, 1); // 0-RX, 1-TX

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
  acao_matrizTransicaoEstados[ALARME_DESLIGADO][LIGAR1] = A01;

  acao_matrizTransicaoEstados[ALARME_DESLIGADO][LIGAR2] = A02;

  acao_matrizTransicaoEstados[ALARME_DESLIGADO][DESLIGAR1] = A03;

  acao_matrizTransicaoEstados[ALARME_DESLIGADO][DESLIGAR2] = A04;
  
  proximo_estado_matrizTransicaoEstados[ALARME_DESLIGADO][LIGAR_S] = ALARME_LIGADO;
  acao_matrizTransicaoEstados[ALARME_DESLIGADO][LIGAR_S] = A05;

  acao_matrizTransicaoEstados[ALARME_LIGADO][LIGAR1] = A01;

  acao_matrizTransicaoEstados[ALARME_LIGADO][LIGAR2] = A02;

  acao_matrizTransicaoEstados[ALARME_LIGADO][DESLIGAR1] = A03;

  acao_matrizTransicaoEstados[ALARME_LIGADO][DESLIGAR2] = A04;

  proximo_estado_matrizTransicaoEstados[ALARME_LIGADO][DESLIGAR_S] = ALARME_DESLIGADO;
  acao_matrizTransicaoEstados[ALARME_LIGADO][DESLIGAR_S] = A06;

  proximo_estado_matrizTransicaoEstados[ALARME_LIGADO][DETECTAR] = ALARME_ACIONADO;
  acao_matrizTransicaoEstados[ALARME_LIGADO][DETECTAR] = A07;


  proximo_estado_matrizTransicaoEstados[ALARME_ACIONADO][DESLIGAR_S] = ALARME_DESLIGADO;
  acao_matrizTransicaoEstados[ALARME_ACIONADO][DESLIGAR_S] = A08;
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
 Parametros de entrada: tecla (char)
                        sensor (int)
                        detecta_presenca (int)
 Retorno: codigo do evento
*************************************************************************/

int obterEvento(char tecla, int sensor, int detecta_presenca)
{
  int retval = NENHUM_EVENTO;

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
  
// Desligamento do sensor
  else if (tecla == 'c' && sensor == true){
    return DESLIGAR_S;
  }

// Sensor ligado detectando presença
  else if (sensor == true && detecta_presenca == true){
    return DETECTAR;
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
  // Os pinos abaixo começam inicialmente em HIGH, pois o relé é ativo em LOW, ou seja, as lâmpadas e a sirene começam desligados.
  digitalWrite(lampada1, HIGH); // Desliga lâmpada 1.
  digitalWrite(lampada2, HIGH); // Desliga lâmpada 2.
  digitalWrite(buzzer, HIGH); // Desliga a sirene.
  attachInterrupt(digitalPinToInterrupt(le_sensor), detecta_presenca, CHANGE);  //interrupção para tocar alarme quando detectar presença

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
