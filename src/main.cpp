#include <Arduino.h>
#include <stdint.h>
#include <artnet.h>



#define WIFI_SSID     "NailHWN"
#define WIFI_PW       "86288628sn"



#define ADRS_PIN_1    13
#define ADRS_PIN_2    14
#define ADRS_PIN_3    27
#define ADRS_PIN_4    35

#define ZX_PIN        34

#define TRIAC1_PIN    26
#define TRIAC2_PIN    25
#define TRIAC3_PIN    33
#define TRIAC4_PIN    32




volatile uint16_t   gDmxAddress;
volatile uint8_t    gDmxValue[4]{0,0,0,0};



uint16_t twinklePattern1OnTimes[8]{18,23,16,15,14,12,21,12};
uint16_t twinklePattern1OffTimes[8]{65,75,45,33,65,22,45,67};
uint8_t twinklePattern1Counter[4]{0,0,0,0};
unsigned long twinkle1LastMillis[4] {0,0,0,0};
boolean twinkle1On[4]{true,true,true,true};

uint16_t twinklePattern2OnTimes[8]{12,17,11,15,14,12,17,12};
uint16_t twinklePattern2OffTimes[8]{250,450,175,345,476,225,345,198};
uint8_t twinklePattern2Counter[4]{0,0,0,0};
unsigned long twinkle2LastMillis[4] {0,0,0,0};
boolean twinkle2On[4]{true,true,true,true};

uint16_t twinklePattern3OnTimes[8]{6,8,6,7,7,6,9,6};
uint16_t twinklePattern3OffTimes[8]{500,900,350,690,925,500,690,400};
uint8_t twinklePattern3Counter[4]{0,0,0,0};
unsigned long twinkle3LastMillis[4] {0,0,0,0};
boolean twinkle3On[4]{true,true,true,true};

int gTriacs[4]{TRIAC1_PIN,TRIAC2_PIN,TRIAC3_PIN,TRIAC4_PIN};

Artnet artnet;



void updateDMXAddress()
{
    gDmxAddress = 0;
    gDmxAddress += digitalRead(ADRS_PIN_1) == 0 ? 1 : 0;
    gDmxAddress += digitalRead(ADRS_PIN_2) == 0 ? 2 : 0;
    gDmxAddress += digitalRead(ADRS_PIN_3) == 0 ? 4 : 0;
    gDmxAddress += digitalRead(ADRS_PIN_4) == 0 ? 8 : 0;
    Serial.print("DMX Address: ");
    Serial.println(gDmxAddress);
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  // print out our data
  Serial.print("universe number = ");
  Serial.print(universe);
  Serial.print("\tdata length = ");
  Serial.print(length);
  Serial.print("\tsequence n0. = ");
  Serial.println(sequence);
  Serial.print("DMX data: ");
  for(int i = 0; i < 4; i++)
  {
    gDmxValue[i] = data[(gDmxAddress*4) + i];
    Serial.print(gDmxValue[i]);
    Serial.print(" ");
  }
  //for (int i = 0 ; i < length ; i++)
  //{
//    Serial.print(data[i]);
//    Serial.print("  ");
//  }
  Serial.println();
  Serial.println();

}

void setup() {


    delay(500);
    pinMode(ADRS_PIN_1,INPUT);
    pinMode(ADRS_PIN_2,INPUT);
    pinMode(ADRS_PIN_3,INPUT);
    pinMode(ADRS_PIN_4,INPUT);

    pinMode(ZX_PIN,INPUT);

    pinMode(TRIAC1_PIN,OUTPUT);
    pinMode(TRIAC2_PIN,OUTPUT);
    pinMode(TRIAC3_PIN,OUTPUT);
    pinMode(TRIAC4_PIN,OUTPUT);

    digitalWrite(TRIAC1_PIN,LOW);
    digitalWrite(TRIAC2_PIN,LOW);
    digitalWrite(TRIAC3_PIN,LOW);
    digitalWrite(TRIAC4_PIN,LOW);


    Serial.begin(115200);
    updateDMXAddress();
    artnet.begin(WIFI_SSID, WIFI_PW);
    artnet.setArtDmxCallback(onDmxFrame);

}

void updateLightStates()
{
  unsigned long currentMillis;
    for(int x = 0; x < 4; x ++)
    {
        switch(gDmxValue[x])
        {
          case 0:
            digitalWrite(gTriacs[x],LOW);
            break;
          case 1:
            digitalWrite(gTriacs[x],HIGH);
            break;
          case 2:
            currentMillis = millis();
            if(twinkle1On[0]){
              if(currentMillis-twinkle1LastMillis[0] >= twinklePattern1OnTimes[twinklePattern1Counter[0]]){
                  digitalWrite(gTriacs[x],LOW);
                  twinkle1On[0] = false;
                  twinkle1LastMillis[0] = currentMillis;
              }
            }else {
              if(currentMillis-twinkle1LastMillis[0] >= twinklePattern1OffTimes[twinklePattern1Counter[0]]){
                  digitalWrite(gTriacs[x],HIGH);
                  twinkle1On[0] = true;
                  twinkle1LastMillis[0] = currentMillis;
                  twinklePattern1Counter[0] ++;
                  if(twinklePattern1Counter[0] > 7){
                    twinklePattern1Counter[0] = 0;
                    }
                }
              }
              break;
            case 3:
              currentMillis = millis();
              if(twinkle2On[0]){
                if(currentMillis-twinkle2LastMillis[0] >= twinklePattern2OnTimes[twinklePattern2Counter[0]]){
                    digitalWrite(gTriacs[x],LOW);
                    twinkle2On[0] = false;
                    twinkle2LastMillis[0] = currentMillis;
                }
              }else {
                if(currentMillis-twinkle2LastMillis[0] >= twinklePattern2OffTimes[twinklePattern2Counter[0]]){
                    digitalWrite(gTriacs[x],HIGH);
                    twinkle2On[0] = true;
                    twinkle2LastMillis[0] = currentMillis;
                    twinklePattern2Counter[0] ++;
                    if(twinklePattern2Counter[0] > 7){
                      twinklePattern2Counter[0] = 0;
                    }
                }
              }
              break;
              case 4:
                currentMillis = millis();
                if(twinkle3On[0]){
                  if(currentMillis-twinkle3LastMillis[0] >= twinklePattern3OnTimes[twinklePattern3Counter[0]]){
                      digitalWrite(gTriacs[x],LOW);
                      twinkle3On[0] = false;
                      twinkle3LastMillis[0] = currentMillis;
                  }
                }else {
                  if(currentMillis-twinkle3LastMillis[0] >= twinklePattern3OffTimes[twinklePattern3Counter[0]]){
                      digitalWrite(gTriacs[x],HIGH);
                      twinkle3On[0] = true;
                      twinkle3LastMillis[0] = currentMillis;
                      twinklePattern3Counter[0] ++;
                      if(twinklePattern3Counter[0] > 7){
                        twinklePattern3Counter[0] = 0;
                      }
                  }
                }
                break;
        }
    }
}



void loop() {
    artnet.read();
    updateLightStates();
}
