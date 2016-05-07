/*
  Sequencer

  This program uses UART to read MIDI Clock
  sync information and play an 8 step sequencer.

  This is intenede to be used with a voltage
  controlled synthesizer.

*/

#include <Arduino.h>

void nextStep();
void backStep();
void setPitch(int);
int getPitch(int);

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

int nextPin = 14;
int backPin = 15;
int pitchPin = 16;
int samplePin = 18;
int ledPin = 2;
int vcoPin = 10;
int gatePin = 19;

int gate = 0;
int tick = 0;
int step = 0;

void setup()
{
  Serial1.begin(31250);
  for(int i = 0; i < 8; i++)
    pinMode(ledPin + i, OUTPUT);
  pinMode(samplePin, INPUT);
  pinMode(vcoPin, OUTPUT);
  pinMode(nextPin, INPUT);
  pinMode(backPin, INPUT);
}

void loop()
{
  if (digitalRead(nextPin) == HIGH) nextStep();
  if (digitalRead(backPin) == HIGH) backStep();
  if (digitalRead(pitchPin) == HIGH) setPitch(step);
  for(int i = 0; i < 8; i++)
    digitalWrite(ledPin + i, LOW);
  digitalWrite(ledPin + step, HIGH);
}

void setPitch(int s)
{
  pitch[s] = analogRead(samplePin);
}

void nextStep()
{
  step = (step < 7) ? ++step : 0;
}

void backStep()
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
  if (Serial1.available())
  {
    if (Serial1.read() == 0xF8)
    {
      int s = tick / 12;
      analogWrite(vcoPin, pitch[s]);
      gate ^= sequence[tick];
      digitalWrite(ledPin + s, gate);
      if (tick >= 96) tick = 0;
      else tick++;
    }
  }
}
