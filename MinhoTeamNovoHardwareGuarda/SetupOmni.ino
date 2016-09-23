
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
