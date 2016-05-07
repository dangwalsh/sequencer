/*
  Sequencer

  This program uses UART to read MIDI Clock
  sync information and play an 8 step sequencer.

  This is intenede to be used with a voltage
  controlled synthesizer.

*/

#include <Arduino.h>

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

int pitch[8] = { 500 };

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

long lastTime[3] = { 0 };
long dlay[3] = { 50 };
int state[3];
int lastState[3] = { LOW };

enum Button { nextB, backB, pitchB };

void setup()
{
  Serial.begin(9600);
  Serial1.begin(31250);
  for(int i = 0; i < 8; i++)
    pinMode(ledPin + i, OUTPUT);
  pinMode(samplePin, INPUT);
  pinMode(vcoPin, OUTPUT);
  pinMode(nextPin, INPUT);
  pinMode(backPin, INPUT);
  pinMode(gatePin, OUTPUT);
}

void loop()
{
  if (!Serial1.available()){
    setButton(nextB, nextPin, stepNext);
    setButton(backB, backPin, stepBack);
    setButton(pitchB, pitchPin, setPitch);

    for(int i = 0; i < 8; i++)
      digitalWrite(ledPin + i, LOW);
    digitalWrite(ledPin + step, HIGH);
  }
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

void setPitch()
{
  pitch[step] = analogRead(samplePin);
  Serial.println(step);
  Serial.println(pitch[step]);
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
  if (Serial1.available())
  {
    if (Serial1.read() == 0xF8)
    {
      int s = tick / 12;
      analogWrite(vcoPin, pitch[s]);
      gate ^= sequence[tick];
      digitalWrite(gatePin, gate);
      digitalWrite(ledPin + s, gate);
      if (tick >= 96) tick = 0;
      else tick++;
    }
  }
}
