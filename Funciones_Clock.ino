void fechahora(bool logtiempo)
{
  if(logtiempo == 1 && TimeVar.sec != debug_slower){
      debug_slower = TimeVar.sec;
      //fecha
      Serial.print(rtc.getDateStr());
      Serial.print(F(" -- "));

      //Hora
      Serial.println(rtc.getTimeStr());

      
      #ifdef DEBUG      
      Serial.print(F("Estado Actual: "));
      Serial.println(estado_txt);  
      Serial.print(F("Bateria: "));
      Serial.print(tb);
      Serial.print(F(" Linea: "));
      Serial.println(tl);      
      Serial.print(F("Sirena: "));
      Serial.println(sirena_flag);
      Serial.print(F("Temperatura Int: "));
      Serial.print(temp1);
      Serial.print(F(" Humedad Int: "));
      Serial.print(hum1);
      Serial.print(F(" %Luz: "));
      Serial.println(foto);
      Serial.print(F("Nivel Senial: "));
      Serial.println(nivel_senal);
      Serial.println();
      Serial.println(F(""));

      #endif
      
      
  }
  else{
      //Guardo los valores en la variable
      TimeVar = rtc.getTime();
  }
  
}

