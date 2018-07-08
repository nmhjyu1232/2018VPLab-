
/*
 * 此程式為第8館巴別塔之控制
 * 接線電路為大腦上連出去為4條燈條
 * input：   1 Buttons、Tower 1 & Tower 2 & Tower 3(pull up)
 * output：  1 led strip、Tower 4
 * 
 * 所有觸發  LOW啟動  HIGH禁能 
*/

/*標頭檔
 * 使用燈條及timer
*/
#include <Adafruit_NeoPixel.h>
#include <TimerOne.h>

/*
 * 定義
*/
#define MaxLedNum      210
#define Mode3VideoTime  210000
#define Mode4VedioTime   12000
#define StripTimeOver 5
#define Mode4Button   7
#define StripPin    6 
#define TowerLayer    4
#define Mode1       A0
#define Mode2       A1
#define Mode3       A2
#define Mode4       A3
#define YES       1
#define NO        0

unsigned long time1;
unsigned long delayTime;
int towerPin[] = {2,3,4,5};
bool towerEnable[4];
int mode;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(MaxLedNum,StripPin,NEO_GRB + NEO_KHZ800);

/*
 * 副程式頭
*/

void setup()
{
  //腳位模式設定
  pinMode(Mode4Button, INPUT_PULLUP);
  pinMode(Mode1,INPUT_PULLUP);
  pinMode(Mode2,INPUT_PULLUP);
  pinMode(Mode3,INPUT_PULLUP);
  pinMode(Mode4,OUTPUT);
  for(int i = 0 ;i < TowerLayer ; i++)
  {
    pinMode(towerPin[i],OUTPUT);
  }
  
  //輸出狀態設置
  digitalWrite(Mode4 , HIGH);
  for(int i = 0 ;i < TowerLayer ; i++)
  {
    digitalWrite(towerPin[i],HIGH);
  }
  //燈條啟動
  strip.begin();
  strip.show(); 

  //燈塔致能
  for(int i = 0 ; i < 4 ; i++)
  {
    towerEnable[i] = 1;
  }
  
  Serial.begin(115200);
}

void loop()
{
  mode = 0;
  for(int i = 0 ; i< 4 ; i++)
    if(towerEnable[i] == NO)
      mode++;
      
  //Tower 1 燈亮 
  if(!digitalRead(Mode1) && towerEnable[0])
  {
    Serial.print("T");
    delay(3000);
    digitalWrite(towerPin[0],LOW);
    delay(5000);
    towerEnable[0] = 0;
  }
  //Tower 2 燈亮 
  if(!digitalRead(Mode2) && towerEnable[1]) 
  {
    Serial.print("T");
    delay(3000);
    digitalWrite(towerPin[1],LOW);
    delay(5000);
    towerEnable[1] = 0;
  }
  //Tower 3 燈亮
  if(!digitalRead(Mode3) && towerEnable[2])
  {
    //影片撥放
    Serial.print("3");
    //等待影片結束
    time1 = millis();
    while(millis() - time1 < Mode3VideoTime){}
    //巴別塔音效
    Serial.print("T");
    digitalWrite(towerPin[2],LOW);
    towerEnable[2] = 0;
  }
  
  //Mode 4
  if(mode == 3 && !digitalRead(Mode1)& !digitalRead(Mode2) & !digitalRead(Mode3) & towerEnable[3])
  {
    while(digitalRead(Mode4Button)){}
    //撥放音效
    Serial.print("A");
    //紀錄現在時間
    time1 = millis();
    //燈條呼吸閃爍
    while(millis() - time1 < Mode4VedioTime)
    {
      for(int i = 0 ; i < 256 && millis() - time1 < Mode4VedioTime ;i++)
      {
        for(int j = 0 ; j < MaxLedNum && millis() - time1 < Mode4VedioTime ; j++)
        {
          if(i<128)
            strip.setPixelColor(j,strip.Color(0,0,i));
          else
            strip.setPixelColor(j,strip.Color(0,0,255-i));
        }
        strip.show();
        delayTime = millis();
        while(millis() - delayTime < StripTimeOver && millis() - time1 < Mode4VedioTime){}        
      }
    }
    //燈條變紅
    for(int i = 0;i < MaxLedNum ; i++)
      strip.setPixelColor(i,strip.Color(127,0,0));

    strip.show();
    //巴別塔音效
    Serial.print("T");
    digitalWrite(Mode4,LOW);
    
    digitalWrite(towerPin[3],LOW);
    towerEnable[3] = 0;
  }
  //重置迴圈
  while(!(towerEnable[0] | towerEnable[1] | towerEnable[2] | towerEnable[3]))
  {
    //等待開關關閉
    while(!digitalRead(Mode4Button)){}

  //重置參數
    //輸出狀態設置
    digitalWrite(Mode4 , HIGH);
    for(int i = 0 ;i < TowerLayer ; i++)
    {
      digitalWrite(towerPin[i],HIGH);
    }
    //燈塔致能
   for(int i = 0 ; i < 4 ; i++)
    {
      towerEnable[i] = 1;
    }

    //燈條暗
    for(int i = 0 ; i < MaxLedNum ; i++)
      strip.setPixelColor(i,strip.Color(0,0,0));
    strip.show();
  }
} 

