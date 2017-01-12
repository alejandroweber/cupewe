/*

Domotica y alarma de fin de semana con GSM
Por Alejandro Weber

Version 3.0 Arduino Mega

Caracteristicas:
  1) Sensores analogicos
      A0: Tension de bateria
      A1: Tesion AC (220v)
      A2: Cantidad de luz con fotocelula
      A3: Temperatura agua calefon solar
      A4: 
      A5: 
      A6: 
      A7: 
      A8: 
      A9: 
      A10:
      A11:
      A12:
      A13:
      A14:
      A15:
  
  2) Entradas digitales (ver plano en directorio del proyecto!)
     Zona 24hs
       22) Alarma Zona 24hs (Tamper Sirena)
       23) Boton Panico
     Zona Inmediata 
       24) Alarma Zona 1  (puerta principal frente reja y madera)
       25) Alarma Zona 2  (ventanas frente casa)
       26) Alarma Zona 3  (ventana lateral cocina)
       27) Alarma Zona 4  (claraboya baño)
       28) Alarma Zona 5  (puerta reja galpon)
       29) Alarma Zona 6  (Sensor golpe casa)
       30) Alarma Zona 7  (Sensor golpe galpon)
       31) Alarma Zona 8  (Sensor golpe hab. ladrillo)
       32) Alarma Zona 9  (Sensor golpe ampliacion)
       33) Alarma Zona 10 (ventana habitacion ladrillo)
       34) Alarma Zona 11 (puerta reja fondo ampliacion)
       35) Alarma Zona 12 (puerta reja frente ampliacion)
       36) Alarma Zona 13 (ventanas frente ampliacion)
       37) Alarma Zona 14 (ventana fondo ampliacion)
       38) Alarma Zona 15 (ventana habitacion madera)
       39) Alarma Zona 16 (Sensor bomba de riego)
       40) Alarma Zona 17 (camara decoy fondo)
       41) Alarma Zona 18 (camara decoy frente)
       42) Alarma Zona 19 (camara decoy pasillo)
       43) Alarma Zona 20 (PIR interior casa)
       44) Alarma Zona 21 (PIR interior galpon)
       45) Alarma Zona 22 (PIR interior ampliacion)
       46) Alarma Zona 23 (PIR interior hab ladrillo)       

     Zona presencia
       47) Alarma Zona 1 (PIR exterior fondo)
       48) Alarma Zona 2 (PIR exterior pasillo)
       49) Alarma Zona 3 (PIR exterior casa)
       
     Zona Perimetral
       50) Alarma Zona 1 (barrera frente)
       51) Alarma Zona 2 (Alambrada fondo)
       52) Alarma Zona 3 (Alambrada lateral oeste)
       53) Alarma Zona 4 (Alambrada lateral este)
      
  3) Salidas digitales
      6) Test de Bateria
      8) Rele Sirena
      9) Rele Luces 1 (fondo)
      10) Rele Luces 2 (frente) //Sin Placa
      11) Led Sirena
      12) Rele Motor bomba pileta
      13) Rele Motor bomba riego
       
      
  4) Entradas digitales Domotica (digitales)
      2) Sensor temperatura y humedad ambiente (DHT11)
      3) Sensor lluvia
  
  5) Entradas Perifericos
      4) Boton Llave 1
      5) Boton LLave 2
      7) Sensa si hay cortocircuito (CC) en la salida de 12v 
  6) Comunicaciones
      0)  Comunicaciones RX0 (Debug PC)
      1)  Comunicaciones TX0
      14) Comunicaciones TX3 (SIM900)
      15) Comunicaciones RX3
      16) Comunicaciones TX2 (Panel)
      17) COmunicaciones RX2
      18) Comunicaciones TX1 (DFPlayer Mini) 
      19) Comunicaciones RX1
      20) Comunicaciones SDA (RTC)+(Display 16x2)
      21) COmunicaciones SCL
 
 PINES DIGITALES SIN USO
      --- NO QUEDAN! :( ----
      
 7) Interfaz con display
      1) Usa la libreria serial EasyTransfer para comunicarse bidireccional (ver ejemplos)
      2) Central envia al display actualizadamente la fecha y hora; temp y humedad, valores de bateria y linea.
      3) Central envia al display la ultima zona disparada.
      4) Central envia al display la zona que no deja armar.
      4) Central envia aviso de alarma presente por buzzer y display de zona
      5) Teclado Autoadhesivo 4x4
      6) Menu setup para poner en fecha y hora el RTC de la central
      7) Menu para anular/habilitar una zona
      8) Menu para configuracion de riego
      9) Menu setup para configuracion de numeros de telefonos
      10) Menu setup protegido con contraseña
      11) Boton de panico, que no sea facil de activar (Se podria usar un boton del arduino y pasarlo por serial)
      
      

 FUNCIONAMIENTO: ((REVISAR))

1) Tiene 4 grandes estados; Armada, Armada Presente, Disparada y Desarmada
1.1) Bucle general
  Sensa alimentacion
  Sensa Domotica
  Sensa cambios de estado (llavero y SMS) y actualiza estado led
  Con cualquiera de los dos botones del llavero arma y desarma
  Sensa z24hs y boton Panico
  Recibe SMS para pedido de estado
  Lee RTC y actualiza variable TimeVar
  Revisa estado de sirena
  Lee sensor de temp y humedad segun el valor de RTC
2) Armado
  2.2) Sensa todas las zonas de alarma 
  2.3) Avisa SMS Alarma
  2.4) Activa Salidas
3) Armado Presente
  3.1) Sensa algunas zonas de alarma
  3.2) Activa salidas (luces principalmente)
  3.3) Avisa SMS
4) Disparada
  4.1) Guarda el valor de la zona disparada y hace titilar en el panel
  4.2) Evita que suene la sirena muchas veces
  4.3) Recibe ACK y cambia de estado, reset de alarmas
5) Desarmado
  5.1) Evita pasar a estado Armado con una zona abierta
  5.2) Avisa por SMS con la zona en problemas luego de 5 intentos fallidos


5) El SMS que se envia debe tener el formato:
  5.1) Activar: ACTIVAR
  5.2) Desactivar: DESACTIVAR
  5.3) Configura hora: 00:00:00
  5.4) Pedir reporte: REPORTE
6) Debe responder en cada caso con otro SMS reporte

7) El reporte es un SMS con un texto como este (136/160):
  Alarma: ACTIVA/DESACTIVA/REINICIADA
  Hora: 21:52:48
  VLinea:219.4V Vbatt:12.6V
  Temp:18.5C Foto:20%
  Pasiv:NO Magnet:SI
  Proxi:NO Boyero:NO
  Panic:NO Activa:SI

8) Cuando suena una alarma, debe mandar el reporte inmediatamente.

9) Si son mas de las 21 y no esta activada, manda reporte.

CARACTERISTICAS:

1)  Arme y desarme con llavero
2)  Arme y desarme por SMS
3)  Reporte general por pedido SMS o por alarma.
4)  Boton de Panico independiente
5)  Sirena exterior antidesarme 
6)  Luces parpadeantes para aviso policia (rele luces) 12v
7)  Sensores magneticos en puertas y ventanas
8)  Boyero conectado a las rejas
9)  Sensores pasivos para detectar proximidad
10) Alarma inteligente minimizando falsos positivos
11) Efecto disuasivo con simulacion de presencia
12) Configuracion de hora del reloj de reporte mediante SMS  
*/

