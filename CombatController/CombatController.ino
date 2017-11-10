#include "StopWatch.h" // http://playground.arduino.cc/Code/StopWatchClass
#include <Servo.h>

Servo servoR,servoL;

StopWatch sw_millis(StopWatch::MILLIS);

int playButtonIN   = 2;
int pauseButtonIN  = 3;
int stopButtonIN   = 4;
int servoRpin      = 5;
int servoLpin      = 6;
//videoOUT           7
int player1LOUT    = 8;
//syncOUT            9
int player2IN      = 10;
int audioOUT       = 11;
int player1IN      = 12;
int player2LOUT    = 13;

int state = 0;
int prevState = 0;
boolean firstTime = true;
boolean player1Stop = false;
boolean player2Stop = false;
int triggerCount = 0;

int servoClosed = 0;
int servoOpened = 150;
boolean trapClosed = true;

long minutes = 0;
long seconds = 0;
long lastSecond = 0;
long miliseconds = 0;

long milisecondsStart = 0;
long milisecondsValue = 0;

#define INTRO     0
#define READY     1
#define COUNTING  2
#define PAUSED    3
#define STOPPED   4

void setup() {
  pinMode(playButtonIN, INPUT);
  pinMode(pauseButtonIN, INPUT);
  pinMode(stopButtonIN, INPUT);

  pinMode(player1IN, INPUT);
  pinMode(player2IN, INPUT);

  servoR.attach(servoRpin);
  servoL.attach(servoLpin);

  pinMode(player1LOUT, OUTPUT);
  pinMode(player2LOUT, OUTPUT);

  tone(audioOUT,800,200);
  tone(audioOUT,900,200);
  tone(audioOUT,1100,200);
  noTone(audioOUT);
  Serial.begin(115200);

  Serial.println("BricoLabs COMBAT controller");
  Serial.print("-Abriendo trampilla... ");
  servoR.write(servoOpened);
  servoL.write(servoOpened);
  delay(1000);
  trapClosed = false;
  Serial.println("Trampilla abierta!");
}

void loop() {
   switch(state)
  {
   case INTRO:
     if(firstTime)
     {
       while(digitalRead(playButtonIN)){}
       Serial.println("Cierra manualmente la trampilla y pulsa Play para cerrar");
       tone(audioOUT,800,200);
       noTone(audioOUT);
       firstTime = false;
     }

     if(digitalRead(playButtonIN))
     {
       tone(audioOUT,800,200);
       noTone(audioOUT);
       Serial.print("-Cerrando trampilla... ");
       servoR.write(servoClosed);
       servoL.write(servoClosed);
       delay(1000);
       trapClosed = true;
       Serial.println("Trampilla cerrada!");
       firstTime = true;
       prevState = state;
       state = READY;
       break;
     }
     break;
   case READY:
     Serial.println("JUGADOR 1 Pulsa para comenzar");
     while(!digitalRead(player1IN))
     {
       digitalWrite(player1LOUT, LOW);
       delay(50);
       digitalWrite(player1LOUT, HIGH);
       delay(50);
     }
     tone(audioOUT,300,200);
     tone(audioOUT,600,300);
     tone(audioOUT,1200,1000);
     noTone(audioOUT);
     Serial.println("JUGADOR 2 Pulsa para comenzar");
     while(!digitalRead(player2IN))
     {
       digitalWrite(player2LOUT, LOW);
       delay(50);
       digitalWrite(player2LOUT, HIGH);
       delay(50);
     }
     tone(audioOUT,300,200);
     tone(audioOUT,600,300);
     tone(audioOUT,1200,1000);
     noTone(audioOUT);
     Serial.println("JUEZ Pulsa para comenzar");
     while(!digitalRead(playButtonIN)){}
     tone(audioOUT,800,500);
     noTone(audioOUT);
     firstTime = true;
     prevState = state;
     state = COUNTING;
     break;
   case COUNTING:
     if(firstTime)
     {
       digitalWrite(player1LOUT, HIGH);
       digitalWrite(player2LOUT, HIGH);
       Serial.println("TIEMPO:");
       firstTime = false;
       if(prevState != PAUSED)sw_millis.reset();
       sw_millis.start();
     }
     lastSecond = seconds;
     miliseconds = (sw_millis.elapsed()) % 1000;
     seconds = ((sw_millis.elapsed()) / 1000) % 60;
     minutes = ((sw_millis.elapsed()) / 1000) / 60;

     if(seconds!= lastSecond)
     {
       Serial.print(minutes);
       Serial.print(":");
       Serial.print(seconds);
       Serial.println("   ");
     }


     if(minutes==0 && seconds==30 && trapClosed==true)
     {
       Serial.print("-Abriendo trampilla...");
       servoR.write(servoOpened);
       servoL.write(servoOpened);
       delay(1000);
       trapClosed = false;
       Serial.println("Trampilla abierta!");
     }

     if(minutes>=2)
     {
       sw_millis.stop();
       firstTime = true;
       prevState = state;
       state = STOPPED;
       Serial.println();
       Serial.println("FIN COMBATE!");
     }

     if(digitalRead(pauseButtonIN))
     {
       sw_millis.stop();
       firstTime = true;
       prevState = state;
       state = PAUSED;
     }
     if (digitalRead(player1IN))
     {
       sw_millis.stop();
       firstTime = true;
       prevState = state;
       player1Stop = true;
       state = PAUSED;
     }
     if (digitalRead(player2IN))
     {
       sw_millis.stop();
       firstTime = true;
       prevState = state;
       player2Stop = true;
       state = PAUSED;
     }

     if(digitalRead(stopButtonIN))
     {
       sw_millis.stop();
       firstTime = true;
       prevState = state;
       state = STOPPED;
       Serial.println("FIN COMBATE! (Parado por Juez)");
     }
     break;
   case PAUSED:
     if(firstTime)
     {
       {
         tone(audioOUT,800,500);
         noTone(audioOUT);
       }
       Serial.println("PAUSA Pulsa Play para continuar");
       if(player1Stop)
       {
         tone(audioOUT,1100,200);
         tone(audioOUT,900,200);
         tone(audioOUT,200,2000);
         noTone(audioOUT);
         Serial.println("Jugador 1 abandona!");
         digitalWrite(player1LOUT, LOW);
         delay(100);
         digitalWrite(player1LOUT, HIGH);
         delay(100);
         digitalWrite(player2LOUT, LOW);
       }
       if(player2Stop)
       {
         tone(audioOUT,1100,200);
         tone(audioOUT,900,200);
         tone(audioOUT,200,2000);
         noTone(audioOUT);
         Serial.println("Jugador 2 abandona!");
         digitalWrite(player1LOUT, LOW);
         delay(100);
         digitalWrite(player2LOUT, HIGH);
         delay(100);
         digitalWrite(player1LOUT, LOW);
       }
       firstTime = false;
     }
     if(player1Stop)
       {
         digitalWrite(player1LOUT, LOW);
         delay(100);
         digitalWrite(player1LOUT, HIGH);
         delay(100);
         digitalWrite(player2LOUT, LOW);
       }
       if(player2Stop)
       {
         digitalWrite(player2LOUT, LOW);
         delay(100);
         digitalWrite(player2LOUT, HIGH);
         delay(100);
         digitalWrite(player1LOUT, LOW);
       }
     if(!player1Stop & !player2Stop)
     {
       digitalWrite(player1LOUT, LOW);
       digitalWrite(player2LOUT, LOW);
     }
     if(digitalRead(playButtonIN))
     {
       firstTime = true;
       prevState = state;
       player1Stop = false;
       player2Stop = false;
       state = COUNTING;
     }
     if(digitalRead(stopButtonIN))
     {
       firstTime = true;
       prevState = state;
       player1Stop = false;
       player2Stop = false;
       state = STOPPED;
       Serial.println("FIN COMBATE! (Parado por Juez)");
     }
     break;
   case STOPPED:
     if(firstTime)
     {
       tone(audioOUT,300,200);
       tone(audioOUT,600,300);
       tone(audioOUT,1200,1000);
       noTone(audioOUT);
       digitalWrite(player1LOUT, LOW);
       digitalWrite(player2LOUT, LOW);
       Serial.print("TIEMPO TOTAL: ");
       Serial.print(minutes);
       Serial.print(":");
       Serial.print(seconds);
       Serial.print(":");
       Serial.println(miliseconds);
       Serial.println("Pulsa Play para comenzar de nuevo");
       firstTime = false;
     }
     if(digitalRead(playButtonIN))
     {
       firstTime = true;
       prevState = state;
       state = INTRO;
     }
     break;
  }
}
