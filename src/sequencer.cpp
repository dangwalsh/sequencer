/*
  Sequencer

  This program uses UART to read MIDI Clock
  sync information and play an 8 step sequencer.

  This is intenede to be used with a voltage
  controlled synthesizer.

*/

#include <Arduino.h>

int sequence[96] = {
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

int gatePin = 2;
int potPin = A0;
int pitchPin = 10;
int gate = 0;
int tick = 0;


void setup()
{
  Serial1.begin(31250);
  for(int i=0; i<8; i++)
    pinMode(gatePin+i, OUTPUT);
  pinMode(potPin, INPUT);
  pinMode(pitchPin, OUTPUT);
}

void loop()
{

}

void serialEvent1()
{
  if (Serial1.available())
  {
    if (Serial1.read() == 0xF8)
    {
      gate ^= sequence[tick];
      digitalWrite(gatePin + tick / 12, gate);
      if (tick >= 96) tick = 0;
      else tick++;
    }
  }
}
