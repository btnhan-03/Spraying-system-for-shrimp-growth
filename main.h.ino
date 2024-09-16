#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "uRTCLib.h"
// ================Phần khai báo của cảm biến PH ================
#define SensorPin_4 A6           
#define Offset 0.00            
#define LED 33
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40  
int pHArray[ArrayLenth];   
int pHArrayIndex=0;
#define quat A4
float do_PH = 0;
// ================TFT 2.8 inch ILI9341 SPI ================
// chân GND nối GND
// chân VCC nối VCC
// chân CLK nối chân 52 SCK
// chân MOSI nối chân 51 MOSI của mega
// chân RES nối chân 4
// chân DC nối chân 5
// chân BLK nối chân 3
// chân MISO nối chân 53 SS/SPI
#define TFT_CS    53 
#define TFT_DC    5
#define TFT_RST   4  
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
// ================khai báo các khối chấp hành  ================
#define van 27  
#define motor_tong 31
#define motor_phu 29
#define button_1  36
#define button_2  38
#define button_3  40
#define button_4  42 
// ================module RTC thời gian ================
uRTCLib rtc(0x68);
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
 int giay ;
 int gio;
 int phut; 
 int ngay;
 String real_time = "";
//================= Khai biến báo của 3 con lưu lượng==================
byte statusLed = 15;

byte sensorInterrupt_1 = 32; // digital pin nào còn trống
byte sensorInterrupt_2 = 34;
byte sensorInterrupt_3 = 35;

byte sensorPin_1 = 2;       //--------------------------------------------------chân tín hiệu
byte sensorPin_2 = 18;
byte sensorPin_3 = 19;

float calibrationFactor = 4.5;

volatile byte pulseCount_1;
volatile byte pulseCount_2;
volatile byte pulseCount_3;

float flowRate_1;
float flowRate_2;
float flowRate_3;

unsigned int flowMilliLitres_1;
unsigned int flowMilliLitres_2;
unsigned int flowMilliLitres_3;

unsigned long totalMilliLitres_1;
unsigned long totalMilliLitres_2;
unsigned long totalMilliLitres_3;

unsigned long oldTime_1;
unsigned long oldTime_2;
unsigned long oldTime_3;



float totalLitres_1;
float totalLitres_2;
float totalLitres_3;

int bien_dem =0;
bool delay_10s = false;
bool next_1 = false;
bool next_2 = false;
bool stopCounting_1 = false;
bool stopCounting_2 = false;
void pulseCounter_1()
{
    // Increment the pulse counter
    pulseCount_1++;
}
void pulseCounter_2()
{
    // Increment the pulse counter
    pulseCount_2++;
}
void pulseCounter_3()
{
    // Increment the pulse counter
    pulseCount_3++;
}

void setup()
{
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
// các khối chấp hành
  pinMode(van, OUTPUT);
  pinMode(motor_tong, OUTPUT);
  pinMode(motor_phu, OUTPUT);
  pinMode(button_1, INPUT);
  pinMode(button_2, INPUT);
  pinMode(button_3, INPUT);
  pinMode(button_4, INPUT);
  pinMode(quat, OUTPUT);
// set của màn hình TFT
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
// thời gian
  URTCLIB_WIRE.begin();
  rtc.set(50,59,5,5,14,1,24);

/* =================== setup cảm biến lưu lượng ===================*/
    // Set up the status LED line as an output
    pinMode(statusLed, OUTPUT);
    digitalWrite(statusLed, HIGH); // We have an active-low LED attached

    pinMode(sensorPin_1, INPUT);
    digitalWrite(sensorPin_1, HIGH);
    pinMode(sensorPin_2, INPUT);
    digitalWrite(sensorPin_2, HIGH);
    pinMode(sensorPin_3, INPUT);
    digitalWrite(sensorPin_3, HIGH);

    pulseCount_1 = 0;
    flowRate_1 = 0.0;
    flowMilliLitres_1 = 0;
    totalMilliLitres_1 = 0;
    pulseCount_2 = 0;
    flowRate_2 = 0.0;
    flowMilliLitres_2 = 0;
    totalMilliLitres_2 = 0;
    pulseCount_3 = 0;
    flowRate_3 = 0.0;
    flowMilliLitres_3 = 0;
    totalMilliLitres_3 = 0;
    oldTime_1= 0;
      oldTime_2= 0;
        oldTime_3= 0;
    totalLitres_1 =0;
    totalLitres_2 =0;
    totalLitres_3 =0;
    attachInterrupt(digitalPinToInterrupt(sensorPin_1), pulseCounter_1, FALLING);
    attachInterrupt(digitalPinToInterrupt(sensorPin_2), pulseCounter_2, FALLING);
    attachInterrupt(digitalPinToInterrupt(sensorPin_3), pulseCounter_3, FALLING);

}
//================= Phần bơm nước của hệ thống ====================
void luuluong()
{
// lưu lượng 1
    if ((millis() - oldTime_1) > 1000) // Only process counters once per second
    {
        detachInterrupt(digitalPinToInterrupt(sensorInterrupt_1));
        flowRate_1 = ((1000.0 / (millis() - oldTime_1)) * pulseCount_1) / calibrationFactor;
        oldTime_1 = millis();
        flowMilliLitres_1 = (flowRate_1 / 60) * 1000.0;
        totalMilliLitres_1 += flowMilliLitres_1;
        totalLitres_1 = totalMilliLitres_1 / 10000.0 ;

        // Print the cumulative total of litres flowed since starting
        Serial.print("So lit nuoc bom duoc 1: ");
        Serial.print(totalLitres_1, 2);
        Serial.println("L");

        tft.fillRect(200, 70, 200, 15, ILI9341_BLACK);
        tft.setCursor(200,70);
        tft.setTextColor(ILI9341_YELLOW);
        tft.setTextSize(2);
        tft.print(totalLitres_1, 2);
        tft.println("L");  
        pulseCount_1 = 0;
    attachInterrupt(digitalPinToInterrupt(sensorPin_1), pulseCounter_1, FALLING);
    }
  
    //lưu lượng 2
    if ((millis() - oldTime_2) > 1000) // Only process counters once per second
    {
        detachInterrupt(digitalPinToInterrupt(sensorInterrupt_2));
        flowRate_2 = ((1000.0 / (millis() - oldTime_2)) * pulseCount_2) / calibrationFactor;
        oldTime_2 = millis();
        flowMilliLitres_2 = (flowRate_2 / 60) * 1000.0;
        totalMilliLitres_2 += flowMilliLitres_2;
        totalLitres_2 = totalMilliLitres_2 / 10000.0 ;

        // Print the cumulative total of litres flowed since starting
        Serial.print("So lit nuoc bom duoc 2: ");
        Serial.print(totalLitres_2,2);
        Serial.println("L");

        tft.fillRect(200, 90, 200, 15, ILI9341_BLACK);
        tft.setCursor(200,90);
        tft.setTextColor(ILI9341_YELLOW);
        tft.setTextSize(2);
        tft.print(totalLitres_2 ,2);
        tft.println("L");  
        pulseCount_2 = 0;
    attachInterrupt(digitalPinToInterrupt(sensorInterrupt_2), pulseCounter_2, FALLING);
    }
// lưu lượng 3
    if ((millis() - oldTime_3) > 1000) // Only process counters once per second
    {
        detachInterrupt(digitalPinToInterrupt(sensorInterrupt_3));
        flowRate_3 = ((1000.0 / (millis() - oldTime_3)) * pulseCount_3) / calibrationFactor;
        oldTime_3 = millis();
        flowMilliLitres_3 = (flowRate_3 / 60) * 1000.0;
        totalMilliLitres_3 += flowMilliLitres_3;
        totalLitres_3 = totalMilliLitres_3 / 10000.0 ;
        // Print the cumulative total of litres flowed since starting
        Serial.print("So lit nuoc bom duoc 3: ");
        Serial.print(totalLitres_3, 2);
        Serial.println("L");

        tft.fillRect(220, 110, 200, 15, ILI9341_BLACK);
        tft.setCursor(220,110);
        tft.setTextColor(ILI9341_YELLOW);
        tft.setTextSize(2);
        tft.print(totalLitres_3 ,2);
        tft.println("L");  
        // Reset the pulse counter so we can start incrementing again
        pulseCount_3 = 0;

        // Enable the interrupt again now that we've finished sending output
    attachInterrupt(digitalPinToInterrupt(sensorInterrupt_3), pulseCounter_3, FALLING);
    }
}
//================= Đo độ PH của hệ thống ====================
void sensor_PH(){
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue,voltage;
  if(millis()-samplingTime > samplingInterval)
  {
      pHArray[pHArrayIndex++]=analogRead(SensorPin_4);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset;
      samplingTime = millis();
      do_PH = pHValue ;
  }
  if(millis() - printTime > printInterval)  
  {
    // Serial.print("Voltage:");
    //     Serial.print(voltage,2);
    //     Serial.print("    pH value: ");
    // Serial.println(pHValue,2);
  tft.fillRect(40, 40, 85, 15, ILI9341_BLACK);
  tft.setCursor(40,40);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print(pHValue);
        digitalWrite(LED,digitalRead(LED)^1);
        printTime=millis();
  }
}
double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    // Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}
//================= hệ thống tự động bơm 1 lần====================
void che_do_tu_dong_1(){
  
  tft.fillRect(40, 55, 300, 15, ILI9341_BLACK);
  tft.setCursor(40,55);
  tft.setTextColor(ILI9341_GREENYELLOW);
  tft.setTextSize(2);
  tft.print("Che do bom 1Lan/1ngay");
// phần hoạt động phun thuốc của hệ thống
  if( gio == 6 && phut <3){  
 if (totalLitres_1 >= 2) {

  digitalWrite(motor_tong, LOW);
  tft.fillRect(140, 180, 200, 20, ILI9341_BLACK);
  tft.setCursor(140,180);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
 
  digitalWrite(van, LOW);
  tft.fillRect(120, 150, 200, 20, ILI9341_BLACK);
  tft.setCursor(120,150);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");  
} else {
  digitalWrite(motor_tong, HIGH);
  tft.fillRect(140, 180, 200, 20, ILI9341_BLACK);
  tft.setCursor(140,180);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");
    
  digitalWrite(van, HIGH);
  tft.fillRect(120, 150, 200, 20, ILI9341_BLACK);
  tft.setCursor(120,150);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");
}

  if (totalLitres_3 >= 2 ){

  tft.fillRect(200, 130, 300, 15, ILI9341_BLACK);
  tft.setCursor(200,130);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(1.75);
  tft.print("XU LY PHAN CAN");
  
  digitalWrite(motor_tong,HIGH);
  tft.fillRect(120, 150, 200, 20, ILI9341_BLACK);
  tft.setCursor(120,150);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");

  digitalWrite(van,HIGH);
  tft.fillRect(140, 180, 200, 20, ILI9341_BLACK);
  tft.setCursor(140,180);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");

  digitalWrite(motor_phu,HIGH);
  tft.fillRect(130, 210, 200, 20, ILI9341_BLACK);
  tft.setCursor(130,210);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");

  bien_dem++;
  Serial.print("Count: ");
  Serial.println(bien_dem);
  if(bien_dem >= 10){
     bien_dem = 0 ;
      flowRate_1 = 0;
      flowRate_2 = 0;
      flowRate_3 = 0;
      totalMilliLitres_1=0;
      totalMilliLitres_2=0;
      totalMilliLitres_3=0;
  }
  } else if (totalLitres_1 > 0.1 ){
  digitalWrite(motor_phu,HIGH);
  tft.fillRect(130, 210, 200, 20, ILI9341_BLACK);
  tft.setCursor(130,210);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");
  }
  } else{

      flowRate_1 = 0;
      flowRate_2 = 0;
      flowRate_3 = 0;
      totalMilliLitres_1=0;
      totalMilliLitres_2=0;
      totalMilliLitres_3=0;

  tft.fillRect(180, 130, 300, 15, ILI9341_BLACK);

  digitalWrite(motor_tong, LOW);
  tft.fillRect(140, 180, 200, 20, ILI9341_BLACK);
  tft.setCursor(140,180);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
 
  digitalWrite(van, LOW);
  tft.fillRect(120, 150, 200, 20, ILI9341_BLACK);
  tft.setCursor(120,150);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
  
  digitalWrite(motor_phu, LOW);
  tft.fillRect(130, 210, 200, 20, ILI9341_BLACK);
  tft.setCursor(130,210);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
}
}


//================= hệ thống tự động bơm 2 lần====================
void che_do_tu_dong_2(){
  
  tft.fillRect(40, 55, 300, 15, ILI9341_BLACK);
  tft.setCursor(40,55);
  tft.setTextColor(ILI9341_GREENYELLOW);
  tft.setTextSize(2);
  tft.print("Che do bom 2Lan/1ngay");

  if( (gio==6||gio==16) && !next_2){
    if(!stopCounting_1){

  digitalWrite(motor_tong, HIGH);
  tft.fillRect(140, 180, 200, 20, ILI9341_BLACK);
  tft.setCursor(140,180);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");
  
  digitalWrite(van, HIGH);
  tft.fillRect(120, 150, 200, 20, ILI9341_BLACK);
  tft.setCursor(120,150);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");
  
    if(totalLitres_2>=0.3){
  digitalWrite(motor_phu, HIGH);
  tft.fillRect(130, 210, 200, 20, ILI9341_BLACK);
  tft.setCursor(130,210);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");
    }
    if(totalLitres_1>=2){
  digitalWrite(motor_tong, LOW);
  tft.fillRect(140, 180, 200, 20, ILI9341_BLACK);
  tft.setCursor(140,180);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
    }
    if(totalLitres_2>=2){
  digitalWrite(van, LOW);
  tft.fillRect(120, 150, 200, 20, ILI9341_BLACK);
  tft.setCursor(120,150);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");      
    }
    if(totalLitres_3>=2){
    //  next_2= true;
  digitalWrite(motor_phu, LOW);
  tft.fillRect(130, 210, 200, 20, ILI9341_BLACK);
  tft.setCursor(130,210);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");  
    }
    }     
  }else  if( totalLitres_3>=2 && !delay_10s){

  tft.fillRect(200, 130, 300, 15, ILI9341_BLACK);
  tft.setCursor(200,130);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(1.75);
  tft.print("XU LY PHAN CAN");
  

  digitalWrite(motor_tong,HIGH);
  tft.fillRect(120, 150, 200, 20, ILI9341_BLACK);
  tft.setCursor(120,150);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");

  digitalWrite(van,HIGH);
  tft.fillRect(140, 180, 200, 20, ILI9341_BLACK);
  tft.setCursor(140,180);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");

  digitalWrite(motor_phu,HIGH);
  tft.fillRect(130, 210, 200, 20, ILI9341_BLACK);
  tft.setCursor(130,210);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");

  bien_dem++;
  Serial.print("Count: ");
  Serial.println(bien_dem);
  if(bien_dem >= 10){
    // delay_10s = true;
    bien_dem = 0 ;
      flowRate_1 = 0;
      flowRate_2 = 0;
      flowRate_3 = 0;
      totalMilliLitres_1=0;
      totalMilliLitres_2=0;
      totalMilliLitres_3=0;
  }
  
  }else{

    tft.fillRect(180, 130, 300, 15, ILI9341_BLACK);
  digitalWrite(motor_tong, LOW);
  tft.fillRect(140, 180, 200, 20, ILI9341_BLACK);
  tft.setCursor(140,180);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
 
  digitalWrite(van, LOW);
  tft.fillRect(120, 150, 200, 20, ILI9341_BLACK);
  tft.setCursor(120,150);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
  
  digitalWrite(motor_phu, LOW);
  tft.fillRect(130, 210, 200, 20, ILI9341_BLACK);
  tft.setCursor(130,210);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
}
}

//================= chế độ thủ công của hệ thống =================
void che_do_thu_cong(){

  tft.fillRect(40, 55, 300, 15, ILI9341_BLACK);
  tft.setCursor(40,55);
  tft.setTextColor(ILI9341_GREENYELLOW);
  tft.setTextSize(2);
  tft.print(" Dieu khien thu cong");

  bool next_5 = false;
  int nutnhan_3 = digitalRead(button_3);
  int nutnhan_2 = digitalRead(button_2);
  int nutnhan_4 = digitalRead(button_4);
  
  if(nutnhan_2 == LOW){
// phần hoạt động phun thuốc của hệ thống 
  if(totalLitres_1 < 2  ){
  digitalWrite(motor_tong, HIGH);
  tft.fillRect(140, 180, 200, 20, ILI9341_BLACK);
  tft.setCursor(140,180);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");
    
  digitalWrite(van, HIGH);
  tft.fillRect(120, 150, 200, 20, ILI9341_BLACK);
  tft.setCursor(120,150);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");

  }else if (totalLitres_1 > 2 ) {
  digitalWrite(motor_tong, LOW);
  tft.fillRect(140, 180, 200, 20, ILI9341_BLACK);
  tft.setCursor(140,180);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");

    digitalWrite(van, LOW);
  tft.fillRect(120, 150, 200, 20, ILI9341_BLACK);
  tft.setCursor(120,150);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
  }

  if(totalLitres_1 > 0.2 && !next_5 ){
      digitalWrite(motor_phu, HIGH);
  tft.fillRect(130, 210, 200, 20, ILI9341_BLACK);
  tft.setCursor(130,210);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");

  }
  if(totalLitres_3 >= 2) {
  next_5 = true;
  digitalWrite(motor_phu, LOW);
  tft.fillRect(130, 210, 200, 20, ILI9341_BLACK);
  tft.setCursor(130,210);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
  }

  } else if(nutnhan_3 == LOW){   
// phần xả cặn của hệ thống 
  tft.fillRect(200, 130, 300, 15, ILI9341_BLACK);
  tft.setCursor(200,130);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(1.75);
  tft.print("XU LY PHAN CAN");
  
  digitalWrite(motor_tong,HIGH);
  tft.fillRect(120, 150, 200, 20, ILI9341_BLACK);
  tft.setCursor(120,150);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");

  digitalWrite(van,HIGH);
  tft.fillRect(140, 180, 200, 20, ILI9341_BLACK);
  tft.setCursor(140,180);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");

  digitalWrite(motor_phu,HIGH);
  tft.fillRect(130, 210, 200, 20, ILI9341_BLACK);
  tft.setCursor(130,210);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("ON");

      flowRate_1 = 0;
      flowRate_2 = 0;
      flowRate_3 = 0;
      totalMilliLitres_1=0;
      totalMilliLitres_2=0;
      totalMilliLitres_3=0;

  } else  if (nutnhan_4 == LOW ){

  digitalWrite(motor_tong, LOW);
  tft.fillRect(140, 180, 200, 20, ILI9341_BLACK);
  tft.setCursor(140,180);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");

  digitalWrite(motor_phu , LOW);
  tft.fillRect(130, 210, 200, 20, ILI9341_BLACK);
  tft.setCursor(130,210);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");

  digitalWrite(van, LOW);
  tft.fillRect(120, 150, 200, 20, ILI9341_BLACK);
  tft.setCursor(120,150);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
  
  } else {
      flowRate_1 = 0;
      flowRate_2 = 0;
      flowRate_3 = 0;
      totalMilliLitres_1=0;
      totalMilliLitres_2=0;
      totalMilliLitres_3=0;
  tft.fillRect(200, 130, 300, 15, ILI9341_BLACK);

  digitalWrite(motor_tong, LOW);
  tft.fillRect(140, 180, 200, 20, ILI9341_BLACK);
  tft.setCursor(140,180);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
 
  digitalWrite(van, LOW);
  tft.fillRect(120, 150, 200, 20, ILI9341_BLACK);
  tft.setCursor(120,150);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
  
  digitalWrite(motor_phu, LOW);
  tft.fillRect(130, 210, 200, 20, ILI9341_BLACK);
  tft.setCursor(130,210);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("OFF");
  } 
}
//================= phần main chính của hệ thống====================
void loop(){

  tft_print();
  luuluong();
  sensor_PH();
  rtc.refresh();
  giay = rtc.second();
  phut = rtc.minute();
  gio  = rtc.hour();
  ngay = rtc.day();
  real_time = String(gio)+":" + String(phut)+":"  + String(giay) +"/" +String(ngay);
  // Serial.println(real_time);
  tft.fillRect(185, 40, 200, 20, ILI9341_BLACK);
  tft.setCursor(185,40);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print(real_time);

  if( do_PH < 7){
    digitalWrite(quat, HIGH);
  }else {
    digitalWrite(quat, LOW);
  }

   int nutnhan_1 = digitalRead(button_1);
 if(nutnhan_1 == LOW){
   che_do_thu_cong();
 }else{
  if( ngay>=1 && ngay<=15){
    che_do_tu_dong_1();
  }else if(ngay>=16){
    che_do_tu_dong_2();
  }
 }
 delay(1000);
}
/* =================== Màn hình mặc định khi hệ thống hoạt động===================*/
void  tft_print(){
  tft.setCursor(90,0);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextSize(2);
  tft.println("DO AN KHOA HOC");

  tft.setCursor(80,20);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.print("WATER FLOW SENSOR");

  tft.setCursor(0,40);
  tft.setTextColor(ILI9341_ORANGE);
  tft.setTextSize(2);
  tft.print("PH: ");

  tft.setCursor(130,40);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("Time: ");
  
  tft.setCursor(10,70);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.print("May bom tong:");
  // tft.print(totalLitres_1);
  // tft.println("L");

  tft.setCursor(10,90);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.print("Binh chua ho 1:");
  // tft.print(totalLitres_2);
  // tft.println("L");

  tft.setCursor(10,110);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.print("May bom phu ho 1:" );
  // tft.print(totalLitres_3);
  // tft.println("L");

  tft.setCursor(10,130);
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.print("KHOI CHAP HANH");

  tft.setCursor(10,150);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("VAN HO 1:");
  
  tft.setCursor(10,180);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("MOTOR_TONG: ");
 
  tft.setCursor(10,210);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("MOTOR_PHU: ");
}