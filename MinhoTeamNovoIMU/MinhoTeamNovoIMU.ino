#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_10DOF.h>

#define OUTPUT__BAUD_RATE 9600

int addr = 0;
float angulo,anguloRaw;
float alfa = 0;

/* Assign a unique ID to the sensors */
Adafruit_10DOF                dof   = Adafruit_10DOF();
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);


void initSensors()
{
 
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
 
}

void setup(void)
{
  Serial.begin(OUTPUT__BAUD_RATE);
  
  alfa = EEPROMReadlong(addr);
  /* Initialise the sensors */
  initSensors();
}

void loop(void)
{
  sensors_event_t accel_event;
  sensors_event_t mag_event;
  sensors_event_t bmp_event;
  sensors_vec_t   orientation;

  mag.getEvent(&mag_event);
  if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation))
  {
    float angulo = orientation.heading;
    anguloRaw = angulo;
    if(angulo<alfa)
    {
      angulo = angulo + 360 - alfa;
    }
    else
    {
      angulo = angulo -alfa;
    }
    Serial.println(angulo);
  }
  
  if (Serial.available() >= 2)
  {
    int command = Serial.read();
    if (command == 'r') 
    {
        alfa = anguloRaw;
        EEPROMWritelong(addr,alfa);
    }
  }

  delay(20);
}
