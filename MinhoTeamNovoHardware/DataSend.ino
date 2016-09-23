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
