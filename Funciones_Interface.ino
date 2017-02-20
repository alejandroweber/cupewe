
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
          estado = 2;
          break;
      case 1:
          beeps(2, 100);
          estado = 2;
          break;
      case 2:
          if(zona_inmediata() == false && zona24hs() == false && panico() == false){
             beeps(1, 100);
             estado = 0;
          }
          else beeps(5,70);  //Tono de error. Falta sumarle el aviso por display       
          break;
      case 3:
          beeps(4, 100);
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

