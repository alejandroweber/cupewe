void beeps (int bucles, int duracion) { //Funcion para avisos mediante beeps en la sirena
  
int i=0;
for (i=0;i<bucles;i++)
  {
  digitalWrite(sirena, HIGH);
  delay(duracion);
  digitalWrite(sirena, LOW);
  delay(duracion);
  }
}

void llavero(void){ //Cualquiera de los dos botones, arma o desarma.. toggle

  if(digitalRead(llave_arme) == HIGH || digitalRead(llave_desarme) == HIGH){
    switch(estado) {
      case 0:
          beeps(2, 100);
          estado_txt="Desarmada";
          estado = 2;
          break;
      case 1:
          beeps(2, 100);
          estado_txt="Desarmada";
          estado = 2;
          break;
      case 2:
          if(zona_inmediata() == false && zona24hs() == false && panico() == false){
             beeps(1, 100);
             estado_txt="Armada OK";
             estado = 0;
          }
          else beeps(5,70);  //Tono de error. Falta sumarle el aviso por display       
          break;
      case 3:
          beeps(4, 100);
          estado_txt="Desarmada";
          estado = 2;
          break;
    }
  }
}

void ledonoff(void){ //Update del led
  if(estado != 2) digitalWrite(led,HIGH); // prende para todos los otros estados
  else {
    digitalWrite(led,LOW);
    sirena_flag = 0;
  } 
}
void botones_display(void) {
  //pres_ok = digitalRead(bt_ok);
  //pres_up = digitalRead(bt_up);
  //pres_down = digitalRead(bt_down);
  //pres_cancel = digitalRead(bt_cancel);
  unsigned int BtValue = analogRead(bt_display);

  /* Mapeo de valores de botones
   * Boton 1 (254) 200 - 300
   * Boton 2 (505) 450 - 550
   * Boton 3 (761) 700 - 800
   * Boton 4 (1019) 900 - 1020
   */
  //if (BtValue > 50) delay(100); //Tiempo de espera para que no tenga rebote
  
  if (BtValue > 254 && BtValue < 257) { pres_ok = true;}
  else if (BtValue > 450 && BtValue < 550){ pres_up = true; }
  else if (BtValue > 700 && BtValue < 800){ pres_down = true; }
  else if (BtValue > 900 && BtValue < 1030){ pres_cancel = true; }
  else {
    pres_ok = false;
    pres_up = false;
    pres_down = false;
    pres_cancel = false;
    }

#ifdef DEBUG
  Serial.println(BtValue);
#endif 
}

/*
 * Si no se aprieta algun boton, cada 10 segundos cambia lo que muestra por el display
 * Estado 0: Muestra Estado de la alarma
 * Estado 1: Muestra nivel de señal
 * Estado 2: Muestra valores de tension de bateria y linea "Alimentacion"
 * Estado 3: Muestra temperatura y humedad de afuera "CLIMA EXTERIOR"
 * Estado 4: Muestra Fecha y Hora
 * Estado 5: Muestra si hay un error de zona al tratar de armar la alarma
 * Estado 6: Muestra ultima zona disparada
 * Estado 7: Entra al menu de configuracion
 */

void muestradisplay(void) { //Va cambiando cada 15 segundos o cuando se aprieta botones
  
  if ((millis() - refresco > 15000) && !pres_ok && !pres_up && !pres_down){
    estado_display++;
    refresco = millis();
  }
  else if (pres_up) {
    estado_display++;
    refresco = millis();
  }
  else if (pres_down && estado_display != 0) {
    estado_display--;
    refresco = millis();
  }
  else if (pres_down && estado_display == 0) {
    estado_display = 5;
    refresco = millis();
  }
  else if (pres_ok) menu_config();
  if (estado_display > 6) estado_display = 0;

/*  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(BtValue); 
  lcd.setCursor(0,1);
  if (pres_ok) lcd.print("OK"); 
  else if (pres_up) lcd.print("UP"); 
  else if (pres_down) lcd.print("DOWN"); 
  else if (pres_cancel) lcd.print("CANCEL"); 

  delay(200);
  */
  switch (estado_display) {
    case 0: 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Alarma CUPEWE"); 
      lcd.setCursor(0,1);
      lcd.print(estado_txt); 
      delay(100); 
      break;
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Nivel Senial"); 
      lcd.setCursor(0,1);
      lcd.print(nivel_senal); 
      delay(100);   
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("V. Linea: ");
      lcd.print(tl); 
      lcd.setCursor(0,1);
      lcd.print("V. Bateria: ");
      lcd.print(tb); 
      delay(100);   
      break;
    case 3: 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Temp In: ");
      lcd.print(temp1);
      lcd.setCursor(0,1);
      lcd.print("Humedad: ");
      lcd.print(hum1);
      delay(100);   
      break;
    case 4:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Dia : "); 
      lcd.print(rtc.getDateStr());
      lcd.setCursor(0,1);
      lcd.print("Hora: "); 
      lcd.print(rtc.getTimeStr());
      delay(100);   
      break;
    case 5:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Zonas abiertas"); 
      lcd.setCursor(0,1);
      if(digitalRead(zin1) == LOW) lcd.print("Zi1-");
      if(digitalRead(zin2) == LOW) lcd.print("Zi2-");
      if(digitalRead(zin3) == LOW) lcd.print("Zi3-");
      if(digitalRead(zin4) == LOW) lcd.print("Zi4-");
      if(digitalRead(zin5) == LOW) lcd.print("Zi5-");
      //Ver como se puede hacer algo mejor que esto es una cochinada 
      delay(100);   
      break;
      
      case 6:
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Zona Disparada"); 
        lcd.setCursor(0,1);
        if(ZonaDisparada != "") lcd.print(ZonaDisparada);
        else lcd.print("Ninguna 24Hs");
        delay(100);     
      break;

  }
}

int menu_config(void){
  
  //botones_display(); //No deberia hacer falta
  config_flag = true; //Levanto el flag de configuracion
  if (pres_cancel) {
    estado_display = 0; //Vuelvo al menu anterior
    config_flag = false;
    return 0; //Si aprieto el boton cancel vuelvo de donde me llamaron
  }
  if (pres_ok) {
    //Por ahora salto al prox estado, aca debe ir confirmando cada posicion 
    estado_menu++;  
    }
    
  //Maquina de estado para el config 
  switch (estado_menu) {
    case 0:
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Hora y Fecha"); 
          lcd.setCursor(0,1);
          lcd.print("10:24 - 21/01/17"); 
          delay(100);
          //Aca debe ir if pres_up y pres_down para configurar
          //Guarda en el RTC el valor  
    break;
    case 1:
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Telefonos Aviso "); 
          lcd.setCursor(0,1);
          lcd.print("1 - 2613370836"); 
          delay(100);
          //Aca debe ir if pres_up y pres_down para configurarr
    break;
    
    case 2:
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Tiempo Sirena"); 
          lcd.setCursor(0,1);
          lcd.print("10 Minutos"); 
          delay(100);
          //Aca debe ir if pres_up y pres_down para configurar
    break;

    case 3:
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Simu. Presencia"); 
          lcd.setCursor(0,1);
          lcd.print("5 ciclos 5 min"); 
          delay(100);
          //Aca debe ir if pres_up y pres_down para configurar
    break;

    case 4:
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Tiempos Riego"); 
          lcd.setCursor(0,1);
          lcd.print("Riego 1"); //Riego 2, Riego 3.. Cuando presiono OK, debe poner el nombre arriba y seguir con lo que sigue
          /*
          lcd.setCursor(0,0);
          lcd.print("Riego 1");
          lcd.setCursor(0,1);
          lcd.print("I:10:20 F:11:30"); //I: Inicio F: Fin, al presionar OK salto al prox
          lcd.setCursor(0,1);
          lcd.print("L M M J V S D"); //Si hay un dia que no quiero, se borra la letra
          */
          delay(100);
          //Aca debe ir if pres_up y pres_down para configurar
    break;
    
  }
}

