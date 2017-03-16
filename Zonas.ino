
bool zona24hs(void) { /*
  if (digitalRead(z24) == LOW) {
  sirena_flag = 1; //Al pasar por el bucle y ejecutar sirenaonoff(), se va a disparar
    if (!z24_flag) {
    disparo_z24 = millis(); //Almaceno el momento del primer disparo
    disparo_sirena = millis(); //Almaceno el momento donde comienza a sonar la sirena REVISAR SI ES EL MEJOR LUGAR
    envia_SMS(NUMERO1, 3);
    z24_flag = true;
    }
  }*/
return 0; //Todo ok
}

bool zona_inmediata(void) {
  //if (digitalRead(zin1) == LOW || digitalRead(zin2) == LOW || digitalRead(zin3) == LOW ) {
  if (digitalRead(zin1) == LOW) {
      if(estado != 2){
        sirena_flag = 1; //Al pasar por el bucle y ejecutar, se va a disparar sirena
        if (!instant_flag) {
        disparo_instant = millis(); //Almaceno el momento del primer disparo
        disparo_sirena = millis(); //Almaceno el momento donde comienza a sonar la sirena REVISAR SI ES EL MEJOR LUGAR
        estado_txt="Disparada";
        mp3_play(random(1,11)); //Ejecuto un sonido antes de que suene la sirena
        envia_SMS(NUMERO1, 1);
        instant_flag = true;
        ZonaDisparada = "Inmediata 1";
        }
      }
  return 1; //Si la zona esta abierta devuelve 1
  }
  else return 0; //Sino devuelve 0
}

void zona_presencia(void) {
  //if (digitalRead(zpr1) == LOW || digitalRead(zpr2) == LOW || digitalRead(zpr3) == LOW){
   if (digitalRead(zpr1) == LOW && (millis() - disparo_presencia >= 5000)){
    disparo_presencia = millis();
    simular_presencia();
  }
}

void zona_perimetral(void) {
  //Preguntar si el estado es 1 (armado presente) que avise por beeper, no por sirena para no asustarse
  
}

bool panico(void){
return 0; //Todo ok
}



