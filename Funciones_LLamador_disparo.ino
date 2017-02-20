
/*
 * motivo_SMS puede tener estos valores:
 * 0 => Reset
 * 1 => Disparo zona inmediata
 * 2 => Disparo zona perimetral
 * 10 => Disparo zona presencia
 * 3 => Disparo Z24
 * 4 => Disparo Boton Panico
 * 5 => Bateria baja
 * 6 => Bateria OK
 * 7 => Corte Electrico
 * 8 => Reposicion electricidad
 * 9 => Reporte Diario
 * 11 => 
 */


void envia_SMS(String num_tel, byte motivo_SMS)
{
  String MENSAJE;
  
/*  
  if (motivo_SMS == 1 && millis() - disparo_proxim > 60000){
    fechahora(1);
    Serial.print(F("Estoy en condiciones de enviar nuevamente el aviso por proximidad"));
  }
*/

//Maquina de estados para control de tiempos
/*
 * Si recibe el ACK por sms inibe el envio por 60 min de esa zona pero debe aceptar nuevos eventos de otras zonas
 * Si la alarma es rearmada se reset del inibidor
 * Ciclo 1: Al persistir la zona se envia 1 mensaje por minuto durante 3 minutos
 * Ciclo 2: Si persiste zona, envia el mensaje 4 y 5 espaciados 5 min
 * Ciclo 3: Si persiste zona, envia mensaje 6, 7, 8, 9, 10 cada 10 min
 */

switch (control_sms_zin) {
  case 0:
  case 1:
  case 2:
  case 3:
    //Enviador cada 1 min
    if(millis() - ultimo_sms_zin > 60000){
      envio_aprobado = true;
      control_sms_zin++;
    }
    
    break;
  case 4:
  case 5:
    //enviador cada 5 min
    if(millis() - ultimo_sms_zin > 300000){
      envio_aprobado = false;
      control_sms_zin++;
    }
    break;
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
    //enviador cada 10 min
    if(millis() - ultimo_sms_zin > 600000){
      envio_aprobado = true;
      control_sms_zin++;
      if(control_sms_zin == 11) control_sms_zin = 0;
    }
    break;
}



//Mensaje Base para todos los demas mensajes.
  MENSAJE="Estado: ";
  MENSAJE.concat(estado_txt);
  MENSAJE.concat("\r\n");
  MENSAJE.concat("VLin: ");
  MENSAJE.concat(tl);
  MENSAJE.concat("  VBat: ");
  MENSAJE.concat(tb);
  MENSAJE.concat("\r\n");
  MENSAJE.concat("Temp1: ");
  MENSAJE.concat(temp1);
  MENSAJE.concat(" Hum1: ");
  MENSAJE.concat(hum1);
  MENSAJE.concat("\r\n");
  
  #ifdef DEBUG
  fechahora(1);
  Serial.print(F("Avisos al numero: "));
  Serial.print(num_tel);
  Serial.print(F(" Motivo: "));
  #endif
  
  switch (motivo_SMS){
        case 0: 
              MENSAJE.concat("Sistema reiniciado");
        break;
        case 1: 
              MENSAJE.concat("Disparo Zona inmediata");
        break;
        case 2:
              MENSAJE.concat("Disparo Zona perimetral");
        break;
        case 3:
              MENSAJE.concat("Disparo Zona 24hs");
        break;
        case 4:
              MENSAJE.concat("Disparo por boton Panico");
        break;
        case 5:
              MENSAJE.concat("Bateria Baja");
        break;
        case 6:
              MENSAJE.concat("Bateria OK");
        break;
        case 7:
              MENSAJE.concat("Corte de red electrica");
        break;
        case 8:
              MENSAJE.concat("Tension de red electrica OK");
        break;
        case 9:  //Reporte Diario y por demanda
              MENSAJE.concat("Hora: ");
              MENSAJE.concat(rtc.getTimeStr());
              MENSAJE.concat("\r\n");
              MENSAJE.concat("Luz: ");
              MENSAJE.concat(foto);
              MENSAJE.concat(" Senal: ");
              MENSAJE.concat(nivel_senal);              
        break;
        case 10:
              MENSAJE.concat("Disparo Zona Presencia");
        break;
        default:
              MENSAJE.concat("SMS sin motivo declarado");
        break;    
   } //switch
  
   #ifdef DEBUG 
   Serial.println(MENSAJE);
   #endif

if (envio_aprobado){
  Serial3.println("AT\r\n");  // Mandamos un comando "AT" para comenzar la comunicación.
  delay(800);
  Serial3.println("AT+CMGF=1\r\n");// Módulo GSM en modo texto.
  delay(800);
  Serial3.println("AT+CMGS=\"02613370836\"\r\n"); // Teléfono al cúal le enviaremos el mensaje. Ale
  delay(800);
  Serial3.println(MENSAJE);
  delay(800);
  Serial3.write(0x1a);  // Comando para enviar el mensaje. Equivale al CRTL+Z.      

  #ifdef DEBUG
  Serial.println("SMS Enviado");
  #endif
  }
  envio_aprobado = false;
  ultimo_sms_zin = millis();
}

void SIM300_rxSMS(void) {
   while (Serial3.available()) {
    char inByte = (char)Serial3.read();
    inputString += inByte;
    //Serial.write(inByte); //Muestro por el monitor serial lo que dice el modem
   }
   
   if (inputString.indexOf("+CMTI: \"SM\",") >= 0) {
      Serial.println("Nuevo SMS!!");
      inputString = "";
      Serial3.write("AT+CMGR=1\r\n"); //Leo por si hay un nuevo mensaje
      delay(100);    
      }
      
   if (inputString.indexOf("+CSQ:") >= 0) {
      if (inputString.indexOf(" ") >= 0) {
        if (inputString.indexOf(",") >= 0) {
          #ifdef DEBUG
          Serial.println("------>>>> Calculando el nivel de senial <<<<------");
          #endif
          nivel_senal = inputString.substring(inputString.indexOf(" ") + 1,inputString.indexOf(",")).toInt(); //Es un string convertido a Int
          inputString = "";
          }
        }
      }
    
   if (inputString.indexOf(NUMERO1) >= 0 && auth_flag == 0) {     
      Serial.println(F("SMS Autorizado"));
      auth_flag = 1;
      }
    
   if (auth_flag == 1 && inputString.indexOf(COMANDO1) >= 0) {     
      Serial.println(F("Comando SMS: pedido de estado"));
      inputString = "";
      auth_flag = 0;
      SIM300_flushSMS();
      envia_SMS(NUMERO1, 9); 
      }
    
   if (auth_flag == 1 && inputString.indexOf(COMANDO2) >= 0) {     
      Serial.println(F("Comando SMS: pedido de armado"));
      inputString = "";
      auth_flag = 0;
      beeps(1, 100);
      SIM300_flushSMS();
      if(zona_inmediata() == false && zona24hs() == false && panico() == false) {
        estado = 0;
        estado_txt="Armada OK";
        }
      else estado_txt="Problema";
      envia_SMS(NUMERO1, 9); //Respondo con el estado
      }
    
   if (auth_flag == 1 && inputString.indexOf(COMANDO3) >= 0) {     
      Serial.println(F("Comando SMS: pedido de desarmado"));
      inputString = "";
      auth_flag = 0;
      SIM300_flushSMS();
      beeps(2, 100);
      estado = 2;
      estado_txt="Desarmada";
      envia_SMS(NUMERO1, 9); //Respondo con el estado
      }
}

void SIM300_flushSMS(void) {
  for (byte i=1; i<=35 ; i++) {
    Serial3.print("AT+CMGD=");
    Serial3.print(i);
    Serial3.println("\r\n"); //Borro todos los SMS
    delay(100);
    
    #ifdef DEBUG 
    Serial.print(F("Eliminando Mensaje: "));
    Serial.println(i);
    #endif
  }
}

