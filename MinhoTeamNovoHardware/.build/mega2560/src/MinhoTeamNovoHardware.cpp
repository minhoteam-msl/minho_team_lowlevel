#include <Arduino.h>
#include <Wire.h>    //required by Omni3MD.cpp
#include <Omni3MD.h>
#include <TimerOne.h>
#include <Servo.h> 
void setup();
void loop();
void timerIsr();
void resetEncoders();
void sendDados();
void readEncoders();
void setupMotorsBoard();
void setupOmni();
String getValue(String data, char separator, int index);
#line 1 "src/MinhoTeamNovoHardware.ino"
//#include <Wire.h>    //required by Omni3MD.cpp
//#include <Omni3MD.h>
//#include <TimerOne.h>
//#include <Servo.h> 

Servo myservo;  // create servo object to control a servo 

#define SENDDATA 
//#define KICKDEBUG
//#define BUZZERDEBUG
//#define LOOPDEBUG
//#define IMUDEBUG
//#define SONARDEBUG
//#define SERVODEBUG
//#define DEBUGPID

/////////////////////////////MOTORS VARS//////////////////////////
#define DRIBLLER1 5  
#define MM1 4 
#define DRIBLLER2 6                      
#define MM2 7 
#define KICKPIN 8
#define BALLPIN A3
#define SERRVOPIN 9
//////////////////////////////////////////////////////////////////
#define Buzzer 53
#define Red 23
#define Green 24
#define Blue 25
//////////////////////////////////////////////////////////////////
#define CamBattery A0
#define PcBattery A1
#define FreeWillButton A2
////////////////////////////OMNI3MD VARS//////////////////////////

//constants definitions
#define OMNI3MD_ADDRESS 0x30        //default factory address
#define BROADCAST_ADDRESS 0x00      //i2c broadcast address
#define M1  1            //Motor1
#define M2  2            //Motor2
#define M3  3            //Motor3


void readEncoders();
String getValue(String data, char separator, int index);
void setupOmni();
void setupMotorsBoard();

Omni3MD omni;                       //declaration of object variable to control the Omni3MD

//Variables to read from Omni3MD

int enc1=0;            // encoder1 reading, this is the encoder incremental count for the defined prescaler (positional control)
int enc2=0;            // encoder2 reading, this is the encoder incremental count for the defined prescaler (positional control)
int enc3=0;            // encoder3 reading, this is the encoder incremental count for the defined prescaler (positional control)
float battery=0;       // battery reading
float temperature=0;   // temperature reading
byte firm_int=0;       // the firmware version of your Omni3MD board (visit http://www.botnroll.com/omni3md/ to check if your version is updated)
byte firm_dec=0;       // the firmware version of your Omni3MD board (visit http://www.botnroll.com/omni3md/ to check if your version is updated)
byte firm_dev=0;       // the firmware version of your Omni3MD board (visit http://www.botnroll.com/omni3md/ to check if your version is updated)
byte ctrl_rate=0;      // the control rate for your motors defined at calibration (in times per second)
int enc1_max;          // maximum count for encoder 1 at calibration, for the defined control rate
int enc2_max;          // maximum count for encoder 2 at calibration, for the defined control rate
int enc3_max;          // maximum count for encoder 3 at calibration, for the defined control rate

int P = 851;
int I = 71;
int D = 101;

int B = 700;
int N = 1300;
int M = 351;

//400,1300,0

int enc1_old = 0;
int enc2_old = 0;
int enc3_old = 0;

int baterryLimitTime = 3000;
unsigned long baterryTimeStamp = 0;

boolean batteryLow = false;

int baterryLowLimitTime = 250;
unsigned long baterryLowTimeStamp = 0;
///////////////////////END OMNI3MD VARS///////////////////////////

float bussolaValorNew = 0,bussolaValor = 0,bussolaValorOld = 0;

int dataSendLimitTime = 20;
unsigned long dataSendTimeStamp = 0;

int encoderLimitTime = 20;
unsigned long encoderTimeStamp = 0;

int kickTime = 0;
int maxKick = 35;

unsigned long loopTime = 0;

short int have_ball = 0;

float batteryPC = 0,batteryCam = 0;
float maxVoltsBatteryPC = 13,maxVoltsBatteryCam = 8;
float maxBatteryPC = 567,maxBatteryCam = 537;

boolean batteryLowPC = false,batteryLowCam = false;

int baterryLowPCLimitTime = 450;
unsigned long baterryLowPCTimeStamp = 0;

int baterryLowCamLimitTime = 550;
unsigned long baterryLowCamTimeStamp = 0;

int comunicationTimeOutLimitTime = 200;
unsigned long comunicationTimeOutTimeStamp = 0;

boolean FreeWill = false;

int servo01 = 180,servo02 = 120;

void setup() {
  
  setupOmni();
  setupMotorsBoard();

  pinMode(KICKPIN, OUTPUT);
  digitalWrite(KICKPIN, LOW);
  
  Timer1.attachInterrupt( timerIsr ); // Timer to kick
  Timer1.stop();

  Serial.begin(115200);
  Serial.flush();
  
  Serial1.begin(9600);
  Serial1.flush();
  
  
  tone(Buzzer, 4000,50);
  delay(150);
  tone(Buzzer, 4000,50);
  delay(300);
  tone(Buzzer, 4000,100);
  delay(300);
  tone(Buzzer, 4000,100);
  
  resetEncoders();
  
  Serial.println("Setup Completed!");

}

void loop() {
  
  if(analogRead(FreeWillButton)>500 && FreeWill==false){
    omni.stop_motors();
    FreeWill = true;
  }
  else if(analogRead(FreeWillButton)<500 && FreeWill==true){
    FreeWill = false;
  }
  
  if(millis()-comunicationTimeOutTimeStamp>comunicationTimeOutLimitTime)
  {
    omni.stop_motors();
    comunicationTimeOutTimeStamp = millis();
  }
  
  //debug
  #if defined(LOOPDEBUG)
    loopTime = millis();
  #endif

  if(Serial1.available()>0)
  {
   String S1 = Serial1.readStringUntil('\n');
   /*bussolaValor = S1.toFloat();*/
   char buffer[10];
   S1.toCharArray(buffer, 10);
   bussolaValor = atof(buffer);
   
   //debug
   #if defined(IMUDEBUG)
    Serial.println(bussolaValor);
   #endif
  }
  
  if(millis()-baterryTimeStamp>baterryLimitTime)
  {
    batteryCam = analogRead(CamBattery) * maxVoltsBatteryCam / maxBatteryCam;
    
    batteryPC = analogRead(PcBattery) * maxVoltsBatteryPC / maxBatteryPC;
    
    battery = omni.read_battery();
    
    battery-=3;//Erro de leitura
    
    if(battery<20 && battery>4) batteryLow = true;
    else batteryLow = false;
    
    if(batteryPC<10.5 && batteryPC>5) batteryLowPC = true;
    else batteryLowPC = false;
    
    if(batteryCam<5.5 && batteryCam>3) batteryLowCam = true;
    else batteryLowCam = false;
    
    baterryTimeStamp = millis();
  }

  #if defined(SENDDATA)
    if(millis()-dataSendTimeStamp>dataSendLimitTime)
    {
      if(analogRead(BALLPIN)>500) have_ball = 1;
      else have_ball = 0;
      sendDados();
      dataSendTimeStamp = millis();
    }
  #endif
  
  #if defined(DEBUGPID)
    if(millis()-dataSendTimeStamp>dataSendLimitTime)
    {
      Serial.print("P: ");
      Serial.print(P);
      Serial.print(" I: ");
      Serial.print(I);
      Serial.print(" D: ");
      Serial.print(D);
      
      Serial.print(" Ramp_time(B): ");
      Serial.print(B);
      Serial.print(" Word slope(N): ");
      Serial.print(N);
      Serial.print(" Kl(M): ");
      Serial.println(M);
      dataSendTimeStamp = millis();
    }
  #endif
  
  if(millis()-encoderTimeStamp>encoderLimitTime)
  {
    readEncoders();
    encoderTimeStamp = millis();
  }

  if(batteryLow){
    
    if(millis()-baterryLowTimeStamp>baterryLowLimitTime)
    {
      tone(Buzzer, 4000,baterryLowLimitTime/2);
      baterryLowTimeStamp = millis();
    }
  }
  
  if(batteryLowPC){
    if(millis()-baterryLowPCTimeStamp>baterryLowPCLimitTime)
    {
      tone(Buzzer, 4500,baterryLowPCLimitTime/2);
      baterryLowPCTimeStamp = millis();
    }
  }
  
  if(batteryLowCam){
    if(millis()-baterryLowCamTimeStamp>baterryLowCamLimitTime)
    {
      tone(Buzzer, 5000,baterryLowCamLimitTime/2);
      baterryLowCamTimeStamp = millis();
    }
  }

  if(Serial.available()>0)
  {
        String lido = Serial.readStringUntil('\n');
        comunicationTimeOutTimeStamp = millis();
     
        if(lido[0]=='A')
        {
          String lido2 = lido.substring(2, lido.indexOf('!'));
          String lido3 = lido.substring(lido.indexOf('!')+1, lido.length());
          int val = lido2.toInt();
          int val2 = lido3.toInt();
          omni.mov_lin1m_nopid(M3,-val);
          delay(1);
          omni.mov_lin1m_nopid(M1,val);
          delay(1);
          omni.mov_lin1m_nopid(M2,val2);
          delay(1);
          Serial.flush();
        }
        else if(lido[0]=='S')
        {
           String Sservo = lido.substring(1, lido.indexOf('\n'));
           int servo = Sservo.toInt();
           myservo.write(servo);
           //debug
          #if defined(SERVODEBUG)
            Serial.print("Servo: ");
             Serial.println(servo);
          #endif
           
        }
        else if(lido[0]=='p'){
          P-=50;
          omni.set_PID(P,I,D);
          delay(10);
        }
        else if(lido[0]=='P'){
          P+=50;
          omni.set_PID(P,I,D);
          delay(10);
        }
        else if(lido[0]=='i'){
          I-=50;
          omni.set_PID(P,I,D);
          delay(10);
        }
        else if(lido[0]=='I'){
        I+=50;
        omni.set_PID(P,I,D);
          delay(10);
        }
        else if(lido[0]=='d'){
          D-=50;
          omni.set_PID(P,I,D);
          delay(10);
        }
        else if(lido[0]=='D'){
          D+=50;
          
          omni.set_PID(P,I,D);
          delay(10);
        }
        
        else if(lido[0]=='b'){
          B-=50;
          omni.set_ramp(B,N,M); 
          delay(15);
        }
        else if(lido[0]=='B'){
          B+=50;
          omni.set_ramp(B,N,M); 
          delay(15);
        }
        
        else if(lido[0]=='n'){
          N-=50;
          omni.set_ramp(B,N,M); 
          delay(15);
        }
        else if(lido[0]=='N'){
          N+=50;
          omni.set_ramp(B,N,M); 
          delay(15);
        }
        else if(lido[0]=='m'){
          M-=50;
          omni.set_ramp(B,N,M); 
          delay(15);
        }
        else if(lido[0]=='M'){
          M+=50;
          omni.set_ramp(B,N,M); 
          delay(15);
        }
        else if(lido[0]=='#')
        {
          Serial1.write("r");
        }
        else if(lido[0]=='x')
        {
            resetEncoders();
        }
        else if(lido[0]=='K')
        {
          String chuto = lido.substring(2, lido.indexOf('\n'));
          int kickTime = chuto.toInt();
          Serial.println("");
          Serial.println("");
          Serial.println(kickTime);
          Serial.println("");
          Serial.println("");
          if(kickTime>0)
          {
            if(kickTime>maxKick)kickTime = maxKick;
            if(kickTime < 0)kickTime = 0;
            digitalWrite(KICKPIN, HIGH);
            Timer1.initialize(kickTime*1000);
            
            //debug
            #if defined(KICKDEBUG)
              Serial.print("Kick: ");
              Serial.println(kickTime);
            #endif
          }
        }
        else
        {
          String linear = getValue(lido, ',', 0);
          String rotacao = getValue(lido, ',', 1);
          String direcao = getValue(lido, ',', 2);
          
          String tipoChuto = getValue(lido, ',', 3);
          String chuto = getValue(lido, ',', 4);
           
          String direct_dribler1 = getValue(lido, ',', 5);
          String vel_dribler1 = getValue(lido, ',', 6);
          String direct_dribler2 = getValue(lido, ',', 7);
          String vel_dribler2 = getValue(lido, ',', 8);
          
          String resetEnc1 = getValue(lido, ',', 9);
          String resetEnc2 = getValue(lido, ',', 10);
          String resetEnc3 = getValue(lido, ',', 11);
          
         
          /*String RedValue = getValue(lido, ',', 11);
          String GreenValue = getValue(lido, ',', 12);
          String BlueValue = getValue(lido, ',', 13);
          String Duration = getValue(lido, ',', 14);
          String Freq = getValue(lido, ',', 15);*/
          
          //Omni Move
          
          int  linear_speedInt = linear.toInt();
          int  rotational_speedInt = rotacao.toInt();
          int  directionsInt = direcao.toInt();
          
          directionsInt = 360 - directionsInt;
          
          //AQUI JOAO
 
         /*int vel1 = linear_speedInt / rotational_speedInt * directionsInt;
         int vel2 = rotational_speedInt / linear_speedInt * directionsInt;
         
         analogWrite(DRIBLLER1, vel1);   //PWM Speed Control
         analogWrite(DRIBLLER2, vel2);   //PWM Speed Control*/
         
 
         //         
         
          if(tipoChuto=="0"){
            myservo.write(servo01);
            delay(300);
            }
          else if(tipoChuto=="1"){
            myservo.write(servo02);
            delay(300);
            }
            
          int kickTime = chuto.toInt();
          
          if(linear_speedInt<0) linear_speedInt = 0;
          else if(linear_speedInt>100) linear_speedInt = 100;
          
          if(rotational_speedInt<-100) rotational_speedInt = -100;
          else if(rotational_speedInt>100) rotational_speedInt = 100;
          
          if(directionsInt<0) directionsInt = 0;
          else if(directionsInt>360) directionsInt = 360;
          
          if(!FreeWill) omni.mov_omni(linear_speedInt, rotational_speedInt, directionsInt);
          else omni.stop_motors();

          if(resetEnc1=="1")
          {
            omni.set_enc_value(M1,0); // resets to zero the encoder value [byte encoder, word encValue]
            delay(1); // waits 1ms for Omni3MD to process information
          }
          if(resetEnc2=="1")
          {
            omni.set_enc_value(M2,0);
            delay(1); // waits 1ms for Omni3MD to process information
          }
          if(resetEnc3=="1")
          {
            omni.set_enc_value(M3,0);
            delay(1); // waits 1ms for Omni3MD to process information
          }
          
          //dribler1
          if(direct_dribler1=="1") digitalWrite(MM1,HIGH);
          else if(direct_dribler1=="0") digitalWrite(MM1,LOW);
          if(direct_dribler1!="2") analogWrite(DRIBLLER1, vel_dribler1.toInt());     //PWM Speed Control  
          
          //dribler2
          if(direct_dribler2=="1") digitalWrite(MM2,HIGH);
          else if(direct_dribler2=="0") digitalWrite(MM2,LOW);
          if(direct_dribler2!="2") analogWrite(DRIBLLER2, vel_dribler2.toInt());     //PWM Speed Control  
          
          if(kickTime>0)
          {
            if(kickTime>maxKick)kickTime = maxKick;
            if(kickTime < 0)kickTime = 0;
            
            digitalWrite(KICKPIN, HIGH);
            Timer1.initialize(kickTime*1000);
            
            //debug
            #if defined(KICKDEBUG)
              Serial.print("Kick: ");
              Serial.println(kickTime);
            #endif
          }
          
        /*analogWrite(Red, RedValue.toInt());
        analogWrite(Green, GreenValue.toInt());
        analogWrite(Blue, BlueValue.toInt());
        if(Duration.toInt()>0)tone(Buzzer, Freq.toInt(),Duration.toInt());

        //debug
        #if defined(BUZZERDEBUG)
          Serial.print("BUZZER Freq: ");
          Serial.print(Freq.toInt());
          Serial.print("BUZZER Duration: ");
          Serial.println(Duration.toInt());
        #endif
        */
        
    }
  }

  //debug
  #if defined(LOOPDEBUG)
    Serial.print("Loop Time:");
    Serial.println(millis()-loopTime);
  #endif
  
}

void timerIsr()
{
    digitalWrite(KICKPIN, LOW);
    Timer1.stop();
}

void resetEncoders()
{
   omni.set_enc_value(M1,0); // resets to zero the encoder value [byte encoder, word encValue]
   delay(1); // waits 1ms for Omni3MD to process information
   omni.set_enc_value(M2,0);
   delay(1); // waits 1ms for Omni3MD to process information
   omni.set_enc_value(M3,0);
   delay(1); // waits 1ms for Omni3MD to process information
}

// DataSend.ino

void sendDados()
{
   Serial.print((int)bussolaValor);
   Serial.print(",");
   Serial.print(FreeWill);
   Serial.print(",");
   Serial.print(have_ball);
   Serial.print(",");
   
   Serial.print(enc1);
   Serial.print(",");
   Serial.print(enc2);
   Serial.print(",");
   Serial.print(enc3);
   
   Serial.print(",");
   Serial.print(batteryPC);
   Serial.print(",");
   Serial.print(batteryCam);
   Serial.print(",");
   Serial.println(battery);
   Serial.flush();
   enc1_old = enc1;
   enc2_old = enc2;
   enc3_old = enc3;
}

// Encoders.ino

void readEncoders()
{
  omni.read_encoders(&enc1,&enc2,&enc3);
}


// setupMotorsBoard.ino

void setupMotorsBoard()
{
  //Setup Hardware control Pins
  pinMode(MM1, OUTPUT);   
  pinMode(MM2, OUTPUT); 
  pinMode(DRIBLLER1,OUTPUT);
  pinMode(DRIBLLER2,OUTPUT);

  pinMode(BALLPIN,INPUT);
  pinMode(PcBattery,INPUT);
  pinMode(CamBattery,INPUT);
  pinMode(FreeWillButton,INPUT);
  
  digitalWrite(MM1,LOW);   
  digitalWrite(MM2, LOW);  
  
  analogWrite(DRIBLLER1, 0);   //PWM Speed Control
  analogWrite(DRIBLLER2, 0);   //PWM Speed Control

  myservo.attach(SERRVOPIN);  // attaches the servo on pin 22 to the servo object
  myservo.write(180);

  Serial.println("End Motors Setup");
  
}

// SetupOmni.ino


void setupOmni()
{
    omni.i2c_connect(OMNI3MD_ADDRESS);  // set i2c connection
    delay(10);                          // pause 10 milliseconds
    omni.stop_motors();                 // stops all motors
    delay(10);
    omni.read_firmware(&firm_int,&firm_dec,&firm_dev); // read firmware version value
    Serial.print("Firmware:");  
    Serial.print(firm_int);             // prints firmware value
    Serial.print(".");  
    Serial.print(firm_dec);             // prints firmware value
    Serial.print(".");  
    Serial.println(firm_dev);             // prints firmware value
      
    delay(5);
    ctrl_rate=omni.read_control_rate();   // read the control rate value
    Serial.print("Control_Rate:");
    Serial.println(ctrl_rate);            // prints control rate value
    delay(5);
    enc1_max=omni.read_enc1_max();        // read encoder1 maximum value at calibration (usefull for detecting a faulty encoder)
    Serial.print("Encoder1_max:");
    Serial.println(enc1_max);              // prints encoder1 maximum calibration value
    delay(5);
    enc2_max=omni.read_enc2_max();        // read encoder1 maximum value at calibration (usefull for detecting a faulty encoder)
    Serial.print("Encoder2_max:");
    Serial.println(enc2_max);              // prints encoder2 maximum calibration value 
    delay(5);
    enc3_max=omni.read_enc3_max();        // read encoder1 maximum value at calibration (usefull for detecting a faulty encoder)
    Serial.print("Encoder3_max:");
    Serial.println(enc3_max);              // prints encoder3 maximum calibration value
    delay(5);
    
    omni.set_PID(P,I,D); // Adjust paramenters for PID control [word Kp, word Ki, word Kd]
    delay(15);                 // 15ms pause required for Omni3MD eeprom writing

    omni.set_ramp(B,N,M);   // set acceleration ramp and limiar take off parameter gain[word ramp_time, word slope, word Kl] 
    delay(15);                 // 10ms pause required for Omni3MD eeprom writing
    
    /*omni.set_i2c_timeout(20); // 200ms
    delay(15); */
    
    Serial.println("End SPI Setup");

    delay(1000);
 
}

// StringSplit.ino

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

