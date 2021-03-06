
void sirenaonoff(void)
{
 
if (sirena_flag == 1 &&  (millis() - disparo_sirena <= 300000)) {   //5m = 300000
                digitalWrite(sirena, HIGH);
                digitalWrite(luces1, HIGH);
                estado = 3; //Estado de disparada
              }
else {
              digitalWrite(sirena, LOW);
              digitalWrite(luces1, LOW);
              sirena_flag = false;
              proxim_flag = false;
              instant_flag = false;
     }

}

void simular_presencia(void)
//Si llamo a la funcion, debo almacenar en el contador la cantidad de eventos
//Si el ultimo evento fue hace mas de 5 minutos, hago reset del contador
//Si el contador llega a 4, dispara la alarma como normalmente.
{

if (millis() - ultimo_evento_presencia <= intervalo) {              
              //Ejecuto rutina de DFPlayer para simular presencia
              ultimo_evento_presencia = millis(); //Actualizo el valor de ultimo evento
              contador_eventos_presencia ++; //Aumento en uno el contador
              //Serial.println(contador_eventos_presencia);
              mp3_play(random(1,11));
              //beeps(1,2000); //Mientras no tengamos el audio OK
              //beeps(3,100);
              }
else {
              contador_eventos_presencia = 0;
              ultimo_evento_presencia = millis(); //Actualizo el valor de ultimo evento
     }

if (contador_eventos_presencia >= 5) //Numero maximo de simulacion de presencia
      {
        sirena_flag = 1; //Aviso que se debe activar sirena
        contador_eventos_presencia = 0; //Reset del contador
        disparo_sirena = millis(); //Actualizo el contador
        envia_SMS(NUMERO1,10); //Envio el mensaje con el motivo correcto
        ZonaDisparada = "Presencia 1";
      }

}

void reporte(void) {  
  //Reporte por condicion horaria
  if(TimeVar.hour == 13 && TimeVar.min == 0 && (TimeVar.sec >= 0 && TimeVar.sec <= 10) && Reporte_Flag == 0) {
      Reporte_Flag = 1; //Con la primer entrada levanto el flag. Se baja despues de enviar el SMS
      SIM300_rxSMS();
      envio_aprobado = true;
      envia_SMS(NUMERO1,9); //Envio el mensaje con el motivo correcto
      SIM300_flushSMS(); //Borro todos los SMS que tenia
      inputString = "";
      }
  //Reporte automatico, si son las 21Hs y no esta activada.
  if(estado == 2 && TimeVar.hour == 21 && TimeVar.min == 0 && (TimeVar.sec >= 0 && TimeVar.sec <= 10) && Reporte_Flag == 0) {
      Reporte_Flag = 1; //Con la primer entrada levanto el flag
      SIM300_rxSMS();
      envio_aprobado = true;
      envia_SMS(NUMERO1,9); //Envio el mensaje con el motivo correcto
      SIM300_flushSMS(); //Borro todos los SMS que tenia
      inputString = "";
      }
}

void pedido_senial(void){ //Cada 10 minutos pido el nivel de señal
  if((TimeVar.min == 0 || TimeVar.min == 10 || TimeVar.min == 20 || TimeVar.min == 30 || TimeVar.min == 40 || TimeVar.min == 50) && TimeVar.sec == 0) {
    #ifdef DEBUG
    Serial.println("------>>> Pidiendo el nivel de señal <<<------");
    #endif 
    Serial3.write("AT+CSQ\r\n"); //Pido nivel de señal
    delay(1500); //Espero un poco
  }
}

