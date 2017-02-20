
void alimentacion(void)
{
 
  tlTotal = tlTotal - tlReadings[readIndex];
  tlReadings[readIndex] = analogRead(linea);
  tlTotal = tlTotal + tlReadings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) readIndex = 0;  

  tl = map(tlTotal/numReadings,0,1023,160,225);   //Linea 220v
  //tb = map(analogRead(batt),0,1023,9,14);      //Bateria
  
  
  if (tl < 215 && aviso_220 == 0){
    envia_SMS(NUMERO1,7); 
    aviso_220 = 1;
  }
  if (tl > 216 && aviso_220 == 1){
    envia_SMS(NUMERO1,8); 
    aviso_220 = 0;
  }
    
  if(TimeVar.min == 5 && TimeVar.sec <= 1 && aviso_220 == 0){ //Tomo lectura de la bateria bajo carga cada 1h siempre que no este cortada la linea 220V
    digitalWrite(testbat,HIGH); //Activo el rele
    delay(1000); ;
    tb = map(analogRead(batt),0,1023,9,14);      // Mido la bateria pero bajo carga controlada
    digitalWrite(testbat,LOW); //desactivo rele
    
    if (tb < 12 && aviso_bat == 0){
        envia_SMS(NUMERO1,5); 
        aviso_bat = 1;
    }
    if (tb > 12 && aviso_bat == 1){
        envia_SMS(NUMERO1,6); 
        aviso_bat = 0;
    }
  }
}

void temphum(void) {
       int err;
       //if((err = interior.read(hum1, temp1)) == 0 && (err = exterior.read(hum2, temp2)) == 0 && dht11_flag == 0 && TimeVar.sec == 15)    // Si devuelve 0 es que ha leido bien, y actualiza cada 1 min
       if((err = dht11.read(hum1, temp1)) == 0 && dht11_flag == 0 && TimeVar.sec == 15)    // Si devuelve 0 es que ha leido bien, y actualiza cada 1 min
       {
             dht11_flag = 1;
          }
       else
          {
             dht11_flag = 0;
          }
}

void fotocel(void) {
  if(TimeVar.sec == 20){ //Tomo lectura cada 1 min
      foto = map(analogRead(fotocelula),0,1023,200,0);  
  }  
}


