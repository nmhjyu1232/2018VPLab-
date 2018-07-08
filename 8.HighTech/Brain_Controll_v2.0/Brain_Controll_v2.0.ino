/*
 * 此程式為第8館大腦主控端之控制
 * 接線電路為大腦上連出去為4條燈條
 * input：   3 Buttons、Tower 4(pull up)
 * output：  4 led strip、TV、Tower 1~3、3 110 light
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
#define WaitColor strip.Color(0,0,127)
#define ModeColor strip.Color(0,0,127)
#define FinishColor strip.Color(127,0,0)

#define Mode1VedioTime  23000 
#define Mode2VedioTime  37000 
#define Mode4SoundTime  24000

#define MaxLedNum  400
#define scrollStripNum   5
#define Reset 12
#define stripNum 4
#define buttonNum 3
#define YES 1
#define NO  0

#define First_Relay  100
#define Second_Relay 200 

typedef  struct{
  int pin;
  int num;
  bool enable;
}var_strip;

typedef struct{
  int pin;
  bool triState;
  int mode;
  bool enable;
}var_button;

var_strip LedStrip[4];
var_button button[3];
const int relayPin[3] = {9,10,11};
int mode;
//int waitLastStrip;
bool inModeState;
unsigned long time1;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(MaxLedNum,2,NEO_GRB + NEO_KHZ800);

/*
 * 副程式頭
*/
void Reset_ISR(void);
void Timer_ISR(void);
void video(char* cmd);

void setup()
{
  Reset_ISR();
  
   //1.輸入按鈕
  for(int i = 0 ; i < buttonNum ; i++)
  {
    pinMode(button[i].pin,INPUT_PULLUP);
  }

  //2.Timer
  Timer1.initialize(2000);  //2ms
  Timer1.attachInterrupt(Timer_ISR);

  //3.燈條
  strip.begin();
  strip.show();
  
  //4.Tower
  pinMode(A0,OUTPUT);
  digitalWrite(A0,HIGH);
  pinMode(A1,OUTPUT);
  digitalWrite(A1,HIGH);
  pinMode(A2,OUTPUT);
  digitalWrite(A2,HIGH);
  pinMode(A3,INPUT_PULLUP);

  //5.Reset
  pinMode(Reset,INPUT_PULLUP);
  
  //9.繼電器腳位
  pinMode(relayPin[0],OUTPUT);
  pinMode(relayPin[1],OUTPUT);
  pinMode(relayPin[2],OUTPUT);
  digitalWrite(relayPin[0],HIGH);
  digitalWrite(relayPin[1],HIGH); 
  digitalWrite(relayPin[2],HIGH); 

  //模式狀態禁能
  inModeState = NO;
  
//  waitLastStrip = 0;

  //電腦通訊鮑率115200
  Serial.begin(115200);
}

void loop()
{
  switch(mode)
  {
    //模式未啟動 
    case 0 :
    //PIN腳切換 
    for(int i = 0; i <stripNum; i++)
    {
          //燈條使能，執行待機動作
          if(LedStrip[i].enable == YES)//
          {
//            waitLastStrip = i;
            //設定輸出腳位
            strip.setPin(LedStrip[i].pin);
            //燈條暗
            for(int j = 0; j < LedStrip[i].num; j++)
            {
                strip.setPixelColor(j,strip.Color(0,0,0));
            }
            //執行待機動作
            for(int j = 0; j < LedStrip[i].num && inModeState == NO; j++)
            {
                
              for(int k = j; k < j+scrollStripNum; k++)
                if(k < LedStrip[i].num)
                  strip.setPixelColor(k,WaitColor);
              strip.show();
              
              delay(5);
              
              for(int k = j; k < j+scrollStripNum; k++)
                  if(k < LedStrip[i].num)
                    strip.setPixelColor(k,strip.Color(0,0,0));
            }
          }
    }
    break;

    //模式1
    case 1 : 
    //清掉全部尚未禁能燈條顏色 
    for(int j = 0 ;j < stripNum ; j++)
    {
      if(LedStrip[j].enable == YES)
      {
        strip.setPin(LedStrip[j].pin);
        for(int i=0; i < MaxLedNum ; i++)
              strip.setPixelColor(i,strip.Color(0,0,0));
        strip.show();
      }
    }
        //燈條觸發動作 
    strip.setPin(LedStrip[0].pin);
    for(int i = 0;i < LedStrip[0].num ; i++)
    {
      strip.setPixelColor(i,ModeColor);
      strip.show();
      delay(5);
    }
    //燈條禁能 
    LedStrip[0].enable = NO;
    
    // 影片撥放
    video("1");
      
    //等待影片播完 
    time1 = millis();
    while(millis() - time1 < Mode1VedioTime){};
       
    //mode 清零或完成三模式
    mode = 0 ;
    for(int i = 0 ; i < stripNum ; i++)
    {
      if(LedStrip[i].enable == NO)
      {
        mode++;
      }
    }
    if(mode != 4)
      mode = 0;
      
    //Tower 1 訊號
    digitalWrite(A0,LOW);
     
    //inModeState 旗標清除
    inModeState = NO;
      
    break; 

  //模式2
  case 2 :
    //清掉先前的燈條顏色 
    for(int j = 0 ;j < stripNum ; j++)
    {
      if(LedStrip[j].enable == YES)
      {
        strip.setPin(LedStrip[j].pin);
        for(int i=0; i < MaxLedNum ; i++)
              strip.setPixelColor(i,strip.Color(0,0,0));
            strip.show();
          }
    }
  //燈條觸發動作 
      strip.setPin(LedStrip[1].pin);
      for(int i = 0;i < LedStrip[1].num ; i++)
      {
        strip.setPixelColor(i,ModeColor);
        strip.show();
        delay(5);
      }
    
    //燈條禁能 
      LedStrip[1].enable = NO;
      
      // 影片撥放
      video("2");
      
      //等待影片播完 
      time1 = millis();
      while(millis() - time1 < Mode2VedioTime){};
      
      
    //mode 清零或完成三模式
    mode = 0 ;
    for(int i = 0 ; i < stripNum ; i++)
    {
      if(LedStrip[i].enable == NO)
      {
        mode++;
      }
    }
    if(mode != 4)
      mode = 0;
      
    //Tower 2 訊號
    digitalWrite(A1,LOW);
     
    
    //inModeState 旗標清除
    inModeState = NO;
    
    break;

  //模式3
  case 3 :
    //清掉先前的燈條顏色 
    for(int j = 0 ;j < stripNum ; j++)
    {
      if(LedStrip[j].enable == YES)
      {
        strip.setPin(LedStrip[j].pin);
        for(int i=0; i < MaxLedNum ; i++)
              strip.setPixelColor(i,strip.Color(0,0,0));
            strip.show();
          }
    }
    //燈條觸發動作 
      for(int i = 0;i < LedStrip[3].num ; i++)
      {
      strip.setPixelColor(i,ModeColor);
      //切換順序對調第3條會不穩定 
      strip.setPin(LedStrip[3].pin);
        strip.show();
        strip.setPin(LedStrip[2].pin);
        strip.show();
        if(i == First_Relay)
          digitalWrite(relayPin[0],LOW);
        if(i == Second_Relay)
          digitalWrite(relayPin[1],LOW);
        delay(5);
      }
      
    //燈條禁能 
      LedStrip[2].enable = NO;
      LedStrip[3].enable = NO;
        
    //mode 清零或完成三模式
    mode = 0 ;
    for(int i = 0 ; i < stripNum ; i++)
    {
      if(LedStrip[i].enable == NO)
      {
        mode++;
      }
    }
    if(mode != 4)
      mode = 0;
      
    //Tower 3 訊號
    digitalWrite(A2,LOW);
    
    
      //inModeState 旗標清除
      inModeState = NO;
      
    break;
    
  case 4 :
    //等待輸入腳位完成 
    while(digitalRead(A3)){}
    
    //完成觸發
      //使燈條都是藍色
    for(int i = 0 ; i < MaxLedNum ; i++)
      strip.setPixelColor(i,ModeColor);
    for(int i = 0 ; i < stripNum ; i++)
    {
      strip.setPin(LedStrip[i].pin);
      strip.show();
    }
      //紅色跑回來 
    for(int i = MaxLedNum-1 ; i >= 0 ; i--)
    {
      strip.setPixelColor(i,FinishColor);
      for(int j = 0 ; j < stripNum ; j++)
      {
        strip.setPin(LedStrip[j].pin);
        strip.show();
      }
      delay(1);
    }
    //眼睛亮
    digitalWrite(relayPin[2],LOW);

    //人類聲音
    video("H");
    time1 = millis();
    while(millis() - time1 < Mode4SoundTime){}
    
    mode = 5;

    //關閉計時器
    Timer1.stop();
    
    break;

    //重置模式
  case 5 :
    while(digitalRead(button[0].pin) == HIGH){}
    while(digitalRead(button[1].pin) == HIGH){}
    while(digitalRead(button[2].pin) == HIGH){}
    //燈條全暗
    for(int j = 0; j < MaxLedNum; j++)
    {
      strip.setPixelColor(j,strip.Color(0,0,0));
    }
    for(int i = 0 ; i < 4 ; i++)
    {
      strip.setPin(LedStrip[i].pin);
      strip.show();
    }
    //重置
    Reset_ISR();
    delay(1000);
    Timer1.start();
    break;
  }
}

/*
 * 初始化
 * 
 * 4.Tower 
 * 6.LedStrip 初始化
 * 7.button 初始化 
 * 7.模式進行旗標
 * 8.完成此板模式等待外部模式完成旗標 
 * 9.繼電器腳位 
*/
void Reset_ISR(void)
{
  
  //4.Tower
  digitalWrite(A0,HIGH);
  digitalWrite(A1,HIGH);
  digitalWrite(A2,HIGH);


  //6.stripEnable
  for(int i = 0; i < stripNum ; i++)
  {
    LedStrip[i].enable = YES ;
    LedStrip[i].pin = i + 2; 
  }
  LedStrip[0].num = 300;
  LedStrip[1].num = 392;
  LedStrip[2].num = 200;
  LedStrip[3].num = 350;
  
  //button 初始化
 for(int i = 0;i < buttonNum ; i++)
 {
  button[i].enable = YES;
  button[i].pin = i+ 6;
  button[i].triState = LOW;
  button[i].mode = i+1;
  } 
  
  //9.繼電器腳位
  digitalWrite(relayPin[0],HIGH);
  digitalWrite(relayPin[1],HIGH); 
  digitalWrite(relayPin[2],HIGH); 

  inModeState = NO;
  
  mode = 0;
  
//  waitLastStrip = 0;
}

/*
 * Timer中斷處理函式
 * 
 * 1.不在模式中
 *  1-1.polling每個腳位，Set腳位模式
 * 2.重置按鈕 
*/
void Timer_ISR(void)
{
  //  1
  if(inModeState == NO)
  {
    //1-1
    for(int i=0 ; i < buttonNum && mode != 4; i++)
    {
      if(digitalRead(button[i].pin) == button[i].triState && LedStrip[i].enable == YES)
      {
        inModeState = YES;
        mode = i+1;
      }
    }
  }
  //2
  if(digitalRead(Reset) == LOW)
  {
    Reset_ISR();
  }
}

void video(char* cmd) {
  Serial.print(cmd);  
}
