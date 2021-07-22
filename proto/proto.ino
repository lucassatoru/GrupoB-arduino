#include <SoftwareSerial.h>

SoftwareSerial mySerial(0, 1); // 0-RX, 1-TX

int lampada2 = 9; // Define a porta Digital 9 como rele.
int lampada1 = 8; // Define a porta Digital 8 como rele1.
int buzzer = 7; // Define a porta Digital 7 como rele2.
int ativa_sensor = 6; // Define a porta Digital 6 como rele3.
int le_sensor = 3;

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
 mySerial.begin(9600); // Define a comunicação serial em 9600.
 pinMode(lampada1, OUTPUT); // Declara rele como um pino de saida.
 pinMode(lampada2, OUTPUT); // Declara rele1 como um pino de saida.
 pinMode(buzzer, OUTPUT); // Declara rele2 como um pino de saida.
 pinMode(ativa_sensor, OUTPUT); // Declara rele3 como um pino de saida.
 pinMode(le_sensor, INPUT); // Declara pino 3 (le_sensor) como um pino de entrada.
 digitalWrite(buzzer, HIGH);
 digitalWrite(ativa_sensor, LOW); // Põem o sensor em estado baixo = 0V
 attachInterrupt(digitalPinToInterrupt(le_sensor), toca_alarme, CHANGE);  //interrupção para tocar alarme quando detecta presença
}

void loop() {
 char caracter = mySerial.read(); // Faz a leitura da caractere
 
 if (caracter == 'A') { // Se identificada caractere 'A'.
 digitalWrite(lampada1, HIGH); // Desliga lampada1.
 }
 if (caracter == 'a') { // Se identificada caractere 'a'.
 digitalWrite(lampada1, LOW); // Liga lampada1.
 }
 if (caracter == 'B') { // Se identificada caractere 'B'.
 digitalWrite(lampada2, HIGH); // Desliga lampada2.
 }
 if (caracter == 'b') { // Se identificada caractere 'b'.
 digitalWrite(lampada2, LOW); // Liga lampada2.
 }
 if (caracter == 'C') { // Se identificada caractere 'C'.
 digitalWrite(ativa_sensor, HIGH); // Liga sensor.
 }
 if (caracter == 'c') { // Se identificada caractere 'c'.
 digitalWrite(ativa_sensor, LOW); // Desliga sensor.
 }

}
