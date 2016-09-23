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
