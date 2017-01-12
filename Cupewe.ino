#include <Wire.h> //Para el display
#include <DFPlayer_Mini_Mp3.h>
#include <DS3231.h> //RTC
#include <DHT11.h> //Sensor Temp Hum
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

#define DEBUG 1 //Debug Habilitado/desabilitado
#undef EN_PRODUCCION //Habilita y desabilita algunas funcionalidades

//Entradas Digitales
#define lluvia  3 //Inhibir el encendido de la bomba de riego
#define llave_arme  4 //Arme
#define llave_desarme  5 //Desarme
#define z24     22 //Tamper sirena 
#define bpanic  23 //pulsador panico
#define zin1    24
#define zin2    25
#define zin3    26
#define zin4    27
#define zin5    28
#define zin6    29
#define zin7    30
#define zin8    31
#define zin9    32
#define zin10   33
#define zin11   34
#define zin12   35
#define zin13   36
#define zin14   37
#define zin15   38
#define zin16   39
#define zin17   40
#define zin18   41
#define zin19   42
#define zin20   43
#define zin21   44
#define zin22   45
#define zin23   46
#define zpr1    47
#define zpr2    48
#define zpr3    49
#define zperi1  50
#define zperi2  51
#define zperi3  52
#define zperi4  53


//Salidas Digitales
#define testbat 6 // Con salida en 1, activa prueba de bateria bajo carga
#define sirena 8 // Rele 1 Placa 1
#define luces1 9 // Rele 2 Placa 1
#define luces2 10 //Placa Inexistente
#define led    11
#define bba_pileta 12 //Placa Inexistente
#define bba_riego  13 //Rele 3 Placa 1

//entradas analogicas
#define batt A0
#define linea A1
#define fotocelula A2
#define temp_calefon A3

//Variables Globales
byte tb = 0, tl = 0, foto = 0;
byte debug_slower; //La uso en funcion fechahora() para que no tire debug varias veces por segundo
unsigned long disparo_sirena = 0; //Almacena el valor de millis al momento del disparo
unsigned long disparo_proxim = 0;
unsigned long disparo_instant = 0;
unsigned long disparo_z24 = 0;
unsigned long disparo_presencia = 0; //Almacena el valor de millis al momento de disparar la zona
unsigned long ultimo_evento_presencia = 0 ;
unsigned long evento_reporte = millis(); //Comienzo con el valor de millis

String inputString = "";
String NUMERO1 = "02613370836";
String pass = "1234";
String COMANDO1 = pass + ' ' + "estado";
String COMANDO2 = pass + ' ' + "armar";
String COMANDO3 = pass + ' ' + "desarmar";
String estado_txt="";

bool sirena_flag = false;
bool proxim_flag = false;
bool instant_flag = false;
bool z24_flag = false;
bool Reporte_Flag = false;
bool dht11_flag = false;
bool auth_flag = false;

byte estado = 0;
byte contador_eventos_presencia = 0;
//int pin_DHT11 = 2;

byte aviso_bat = 0;
byte aviso_220 = 0;
byte aviso_sirena = 0;
byte nivel_senal = 0;

const long intervalo = 300000; //5 minutos
float temp, hum;

//Variables para suavizar los valores de los analogicos (por ahora es solo para 220v
const byte numReadings = 10;
unsigned int tlReadings[numReadings];      // the readings from the analog input
byte readIndex = 0;              // the index of the current reading
unsigned int tlTotal = 0;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // YwRobot Arduino LCM1602 IIC V1

DHT11 dht11(2); // Usar el pin 2
DS3231  rtc(SDA, SCL); // Init the DS3231 using the hardware interface
Time TimeVar; //Variable para guardar el valor que entrega el RTC DS3231

void setup() {
  //Ojo con los seriales que cada uno ocupa 2% de memoria RAM
  Serial.begin(9600); // USB debug
  Serial1.begin(9600); // DFPlayer
  Serial2.begin(9600); // Panel
  Serial3.begin(9600); // SIM300

  //Salidas Digitales
  pinMode(sirena, OUTPUT);
  pinMode(luces1, OUTPUT);
  pinMode(luces2, OUTPUT);
  pinMode(led,    OUTPUT);
  pinMode(bba_pileta, OUTPUT);
  pinMode(bba_riego,  OUTPUT);

  //Entradas Digitales
  pinMode(llave_arme, INPUT);
  pinMode(llave_desarme, INPUT);
  pinMode(z24,    INPUT);
  pinMode(bpanic, INPUT);
  pinMode(zin1, INPUT);
  pinMode(zin2, INPUT);
  pinMode(zin3, INPUT);
  pinMode(zin4, INPUT);
  pinMode(zin5, INPUT);
  pinMode(zin6, INPUT);
  pinMode(zin7, INPUT);
  pinMode(zin8, INPUT);
  pinMode(zin9, INPUT);
  pinMode(zin10, INPUT);
  pinMode(zin11, INPUT);
  pinMode(zin12, INPUT);
  pinMode(zin13, INPUT);
  pinMode(zin14, INPUT);
  pinMode(zin15, INPUT);
  pinMode(zin16, INPUT);
  pinMode(zin17, INPUT);
  pinMode(zin18, INPUT);
  pinMode(zin19, INPUT);
  pinMode(zin20, INPUT);
  pinMode(zin21, INPUT);
  pinMode(zin22, INPUT);
  pinMode(zin23, INPUT);
  pinMode(zpr1, INPUT);
  pinMode(zpr2, INPUT);
  pinMode(zpr3, INPUT);
  pinMode(zperi1, INPUT);
  pinMode(zperi2, INPUT);
  pinMode(zperi3, INPUT);
  pinMode(zperi4, INPUT);


  // DFSerial.begin (9600);
  mp3_set_serial (Serial1);    //set Serial DFPlayer-mini mp3 module
  delay(1);                     // delay 1ms to set volume
  mp3_set_volume (30);          // value 0~30
  
  // Initialize the rtc object
  rtc.begin();
  
  //Configuro el modem para almacenar en la SIM  
  Serial3.println("AT+CNMI=1,1,0,0,0\r\n"); 
  
  //(Cuando aca habia un Serial.println se colgaba la carga del programa?

  estado = EEPROM.read(0); //Leo el valor del ultimo estado guardado
  if (estado == 0)      estado_txt="Armada OK";
  else if (estado == 1) estado_txt="Arma Pres";
  else if (estado == 2) estado_txt="Desarmada";
  else if (estado == 3) estado_txt="Disparada";
  
  tb = map(analogRead(A0),0,1023,9,14); // Leo el valor de la bateria al iniciar
  foto = map(analogRead(fotocelula),0,1023,100,0); //Leo el valor inicial de luz

  //Inicia el array de lecturas de 220v en algun valor real
  for (byte thisReading = 0; thisReading < numReadings; thisReading++) {
    tlReadings[thisReading] = analogRead(linea);
    tlTotal = tlTotal + tlReadings[thisReading]; //Acumulador
    delay(10); //Espero para estabilizar
  }
  tl = map(tlTotal/numReadings,0,1023,160,225);   //Linea 220v


  //Aviso a NUMERO1 con motivo "reset"
  #ifdef EN_PRODUCCION
  envia_SMS(NUMERO1, 0); 
  #endif

  //LCD
  lcd.begin (16,2);
  lcd.setBacklight(1);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Alarma CUPEWE"); 
  lcd.setCursor(0,1);
  lcd.print(estado_txt); 
  delay(2000); 
  
  //Pido nivel de señal
  Serial3.write("AT+CSQ\r\n");   
}


void loop() {
  #ifdef EN_PRODUCCION
  EEPROM.update(0, estado); //Actualiza el valor en la memoria solamente si es diferente al almacenado.
  #endif
  
  #ifdef DEBUG
    fechahora(1);
  #endif
  

  
  switch (estado) {
    case 0: //Armado normal
      estado_txt="Armada OK";
      zona_inmediata();
      zona_presencia();
      zona_perimetral();
      //Sensado de zonas inmediatas, perifericas y sensor golpe
      //Si hay disparo:
      //Zona inmediata: Dispara alarma
      //                Manda SMS
      //Zona periferica y golpe: Simulacion de presencia
      //                Luego de la 4ta vez Dispara alarma, Manda SMS
      //                (Hacer un buffer de 8 bits que se vaya llenando con bits de disparo, si pasa 1 min
      //                 sin dispararse ese buffer se hace shift con un 0, si hay 4 bits con 1, se dispara.
      //                00110011 -> Disparo  00001111 -> Disparo
      //                Si hay disparo, Se pone en cero el buffer y el sensor de golpe
      break;

    case 1: //Armado Presente
      zona_perimetral();
      estado_txt="Arma Pres";
      break;

    case 2: //Desarmada
      estado_txt="Desarmada";
      sirena_flag = 0;
      break;

    case 3: //Disparada
      estado_txt="Disparada";
      zona_inmediata();
      zona_presencia();
      zona_perimetral();
      break;

  }//Switch estado

  reporte(); //Si el horario es el correcto generar reporte
  alimentacion(); //Revisar estado de las tensiones
  fotocel();
  temphum(); //Sensa humedad y temp
  sirenaonoff(); //Revisa si hay que activar sirena
  zona24hs(); //Revisa z24
  panico(); //Revisa boton panico
  fechahora(0); //Tomo el horario
  llavero(); //lectura botones llavero
  ledonoff();//Actualiza estado del led
  pedido_senial();
  SIM300_rxSMS(); //Reviso si no hay nada en el buffer del SIM300
  
}//Loop principal.

//La funcion de envio de SMS debe usar una variable con las zonas abiertas desde que fue armada
//La funcion de simulacion de presencia debe desactivar el sensor de golpe(microfono) mientras ejecuta MP3


