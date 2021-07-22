// Código exemplo Sensor PIR de Moviemnto
// Projeto de Alarme / Automação Residencial

#include <SoftwareSerial.h>

SoftwareSerial mySerial(0, 1); // 0-RX, 1-TX

int lampada2 = 9; // Define a porta Digital 9 como lampada2.
int lampada1 = 8; // Define a porta Digital 8 como lampada1.
int buzzer = 7; // Define a porta Digital 7 como buzzer.
int ativa_sensor = 6; // Define a porta Digital 6 como ativa_sensor.
int le_sensor = 3; // Define a porta Digital 3 como le_sensor.

// Interrupt Service Routine (ISR)
void toca_alarme(){
  if(digitalRead(le_sensor) == HIGH) {
    digitalWrite(buzzer, LOW);
  }
  else{
    digitalWrite(buzzer, HIGH); 
  }
}


void setup() {

  Serial.begin(9600); // Declara o BaundRate em 9600
  Serial.println("testando sensor"); // Imprime a frase no monitor serial
  pinMode(lampada1, OUTPUT); // Declara pino 8 (lampada1) como um pino de saída.
  pinMode(lampada2, OUTPUT); // Declara pino 9 (lampada2) como um pino de saída.
  pinMode(buzzer, OUTPUT); // Declara pino 7 (buzzer) como um pino de saída.
  pinMode(ativa_sensor, OUTPUT); // Declara pino 6 (ativa_sensor)  como um pino de saída.
  pinMode(le_sensor, INPUT); // Declara pino 3 (le_sensor) como um pino de entrada.
  digitalWrite(ativa_sensor, LOW); // Põem o sensor em estado baixo = 0V
  attachInterrupt(digitalPinToInterrupt(le_sensor), toca_alarme, CHANGE);  //interrupção para tocar alarme quando detecta presença
}

void loop() {
  char caracter = mySerial.read(); // Faz a leitura da caractere
  
  if (caracter == 'A') { // Se identificada caractere 'A'.
   digitalWrite(lampada1, HIGH); // Liga làmpada 1.
   }
  if (caracter == 'a') { // Se identificada caractere 'a'.
   digitalWrite(lampada1, LOW); // Desliga lâmpada 1.
   }
  if (caracter == 'B') { // Se identificada caractere 'B'.
   digitalWrite(lampada2, HIGH); // Liga lâmpada 2.
   }
  if (caracter == 'b') { // Se identificada caractere 'b'.
   digitalWrite(lampada2, LOW); // Desliga lâmpada 2.
   }
  if (caracter == 'C') { // Se identificada caractere 'C'.
   digitalWrite(ativa_sensor, HIGH); // Liga sensor.
   }
  if (caracter == 'c') { // Se identificada caractere 'c'.
   digitalWrite(ativa_sensor, LOW); // Desliga sensor.
  }
    
}
