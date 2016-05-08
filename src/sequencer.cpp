/*
  Sequencer

  This program uses UART to read MIDI Clock
  sync information and play an 8 step sequencer.

  This is intenede to be used with a voltage
  controlled synthesizer.

*/

#include <Arduino.h>

void startStop();
void stepNext();
void stepBack();
void setPitch();
int getPitch(int);
void setButton(int, int, void (*f)(void));

int sequence[] = {
                        //  1/16  1/8   1/4   1/2   1/1
    1, 0, 0, 0, 0, 0,   //   1      1     1     1     1
    1, 0, 0, 0, 0, 0,   //   2
    1, 0, 0, 0, 0, 0,   //   3      2
    1, 0, 0, 0, 0, 0,   //   4
    1, 0, 0, 0, 0, 0,   //   5      3     2
    1, 0, 0, 0, 0, 0,   //   6
    1, 0, 0, 0, 0, 0,   //   7      4
    1, 0, 0, 0, 0, 0,   //   8
    1, 0, 0, 0, 0, 0,   //   9      5     3     2
    1, 0, 0, 0, 0, 0,   //  10
    1, 0, 0, 0, 0, 0,   //  11      6
    1, 0, 0, 0, 0, 0,   //  12
    1, 0, 0, 0, 0, 0,   //  13      7     4
    1, 0, 0, 0, 0, 0,   //  14
    1, 0, 0, 0, 0, 0,   //  15      8
    1, 0, 0, 0, 0, 0    //  16

};

int pitch[8] = { 0 };

int nextPin = 3;
int backPin = 2;
int pitchPin = 7;
int samplePin = 19;
int ledPin = 2;
int vcoPin = 10;
int gatePin = 20;
int runPin = 18;

int gate = 0;
int tick = 0;
int step = 0;

long lastTime[4] = { 0 };
long dlay[4] = { 50 };
int state[4];
int lastState[4] = { LOW };

enum Button { nextB, backB, pitchB, runB };
static bool isRunning = false;

void setup()
{
  Serial1.begin(31250);
  // for(int i = 0; i < 8; i++)
  //   pinMode(ledPin + i, OUTPUT);
  pinMode(samplePin, INPUT);
  pinMode(vcoPin, OUTPUT);
  pinMode(nextPin, INPUT);
  pinMode(backPin, INPUT);
  pinMode(gatePin, OUTPUT);
  pinMode(runPin, INPUT);
  // attachInterrupt(nextPin, stepNext, RISING);
  // attachInterrupt(backPin, stepBack, RISING);
  // attachInterrupt(samplePin, setPitch, RISING);
}

void loop()
{
  /*
  if (!isRunning)
  {
    setButton(nextB, nextPin, stepNext);
    setButton(backB, backPin, stepBack);
    setButton(pitchB, pitchPin, setPitch);
    for(int i = 0; i < 8; i++)
      digitalWrite(ledPin + i, LOW);
    digitalWrite(ledPin + step, HIGH);
  }
  setButton(runB, runPin, startStop);
  */
}

void setButton(int i, int pin, void (*fptr)(void))
{
  int reading = digitalRead(pin);
  if (reading != lastState[i])
    lastTime[i] = millis();
  if ((millis() - lastTime[i]) > dlay[i])
    if (reading != state[i])
    {
      state[i] = reading;
      if (state[i] == HIGH) fptr();
    }
  lastState[i] = reading;
}

void startStop()
{
  isRunning = !isRunning;
}

void setPitch()
{
  pitch[step] = analogRead(samplePin);
}

void stepNext()
{
  step = (step < 7) ? ++step : 0;
}

void stepBack()
{
  step = (step > 0) ? --step : 7;
}

int getPitch(int v)
{
  for (int i = 1; i < 14; i++)
    if (v < (79 * i)) return (i - 1);
}

void serialEvent1()
{
  //if (!isRunning) return;
  if (Serial1.available())
  {
    if (Serial1.read() == 0xF8)
    {
      int s = tick / 12;
      analogWrite(vcoPin, pitch[s]);
      gate ^= sequence[tick];
      digitalWrite(gatePin, gate);
      //digitalWrite(ledPin + s, gate);
      if (tick >= 96) tick = 0;
      else tick++;
    }
  }
}
