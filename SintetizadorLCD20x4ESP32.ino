#include <LiquidCrystal.h>
#include <Preferences.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Configuración de Pines para ESP32-S3
// PINES DEL LCD
const int LCD_RS = 15, LCD_EN = 16, LCD_D4 = 4, LCD_D5 = 5, LCD_D6 = 6, LCD_D7 = 7;

// PINES DE CONTROL 
const int PIN_MENU = 47, PIN_SUBIR = 21, PIN_BAJAR = 20, PIN_OK = 19;

// PINES DE PLL LOCK Y ALARMA
const int PIN_PLL_IN = 11, PIN_PLL_OUT = 12, PIN_ALARM_OUT = 13;

// PIN DE SALIDA DE FRECUENCIA MEDIANTE PWM
const int freqPin = 17;
// CONFIGURACION DE LA SEÑAL
const int frequency = 19000;  // SE AJUSTA LA FRECUENCIA EN Hz (19Khz)
const int ledChannel = 0; // CANAL (0-7)
const int resolution = 8; // RESOLUCION EN BITS (1-14 bits)

// PINES DE CANALES ADC
const int PIN_ADC_POT = 3, PIN_ADC_REF = 9, PIN_ADC_TEMP = 10, PIN_ADC_LEVEL = 8;
OneWire oneWire(PIN_ADC_TEMP);
DallasTemperature sensors(&oneWire); 
#define PIN_ADC_MOD 14 // MODULACION MPX
#define T_REFRESH    500
#define T_PEAKHOLD   (5 * T_REFRESH)

// Configuracion de pines para el ajuste de la frecuencia
#define PIN_1 43
#define PIN_2 44
#define PIN_3 1
#define PIN_4 2
#define PIN_5 42
#define PIN_6 41
#define PIN_7 40
#define PIN_8 39
#define PIN_9 38
#define PIN_10 37
#define PIN_11 36

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Preferences prefs;

byte  fill[6] = {0x20, 0x00, 0x01, 0x02, 0x03, 0xFF};
byte  peak[7] = {0x20, 0x00, 0x04, 0x05, 0x06, 0x07, 0x20};
byte block[8][8]=
{
  {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10},
  {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
  {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C},
  {0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E},
 
  {0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08},
  {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},
  {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02},
  {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
};

// CARACTERES ESPECIALES PARA NÚMEROS GRANDES
byte LT[8] = {0x07, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};
byte UB[8] = {0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00};
byte RT[8] = {0x1C, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};
byte LL[8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x07};
byte LB[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F};
byte LR[8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1E, 0x1C};
byte UMB[8] = {0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0x1F};
byte LMB[8] = {0x1F, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F};

// Matriz de formas para números 0-9 (2 filas x 3 columnas)
const byte bigFont[10][6] = {
  {0, 1, 2, 3, 4, 5}, // 0
  {1, 2, 32, 4, 255, 4}, // 1
  {6, 6, 2, 3, 4, 4}, // 2
  {6, 6, 2, 4, 4, 5}, // 3
  {3, 4, 255, 32, 32, 5}, // 4
  {3, 6, 6, 4, 4, 5}, // 5
  {0, 6, 6, 3, 4, 5}, // 6
  {1, 1, 2, 32, 32, 5}, // 7
  {0, 6, 2, 3, 4, 5}, // 8
  {0, 6, 2, 32, 32, 5}  // 9
};

// Función para imprimir un dígito grande
void printBigDigit(int digit, int x) {
  for (int i = 0; i < 3; i++) {
    lcd.setCursor(x + i, 2);
    lcd.write(bigFont[digit][i]);
    lcd.setCursor(x + i, 3);
    lcd.write(bigFont[digit][i + 3]);
  }
}

// Función para imprimir el porcentaje completo (0-100)
void printLargePercent(int val) {
  int d1 = val / 100;          // Centenas
  int d2 = (val % 100) / 10;   // Decenas
  int d3 = val % 10;           // Unidades

  if (val == 100) {
    printBigDigit(1, 3);
    printBigDigit(0, 7);
    printBigDigit(0, 11);
  } else {
    lcd.setCursor(3, 2); lcd.print("   "); lcd.setCursor(3, 3); lcd.print("   "); // Limpiar centenas
    if (val >= 10) printBigDigit(d2, 7);
    else { lcd.setCursor(7, 2); lcd.print("   "); lcd.setCursor(7, 3); lcd.print("   "); }
    printBigDigit(d3, 11);
  }
  lcd.setCursor(15, 3); lcd.print("%");
}

// VARIABLES GLOBALES
int16_t cnt = 1, sub = 7, conteo = 0, conteo2 = 0, hz1, hz2, hz1_lee, hz2_lee, mod;
int16_t temp, frec1, estadoPll, lmax[2], dly[2], modAudio;
uint16_t poten, ref, leeAdc, valorAdc, level;
long lastT = 0;


void  bar(int row, int lev) {
    
    lcd.setCursor(0, 3);
    lcd.write(row ? "" : "");
 
    for(int i = 1; i < 21; i++) {
        int f = constrain(lev-i*5, 0, 5);
        int p = constrain(lmax[row]-i*5, 0, 6);
        if(f){
            lcd.write(fill[f]);
        }else{
            lcd.write(peak[p]);
        }
    }
 
    if(lev > lmax[row]) {
        lmax[row] = lev;
        dly[row]  = -(T_PEAKHOLD) / T_REFRESH;
    }
    else {
        if(dly[row] > 0){
            lmax[row] -= dly[row];
        }
        if(lmax[row] < 0){
            lmax[row] = 0;
        }else{
            dly[row]++;
        }
    }
}

void temporizador(){
  conteo2 ++;
  if (conteo2 == 500){
    conteo2 = 0;
    cnt = 5;
  }
}
void frecuencia_off() {
  digitalWrite(PIN_1, LOW);
  digitalWrite(PIN_2, LOW);
  digitalWrite(PIN_3, LOW);
  digitalWrite(PIN_4, LOW);
  digitalWrite(PIN_5, LOW);
  digitalWrite(PIN_6, LOW);
  digitalWrite(PIN_7, LOW);
  digitalWrite(PIN_8, LOW);
  digitalWrite(PIN_9, LOW);
  digitalWrite(PIN_10, LOW);
  digitalWrite(PIN_11, LOW);
}

// LOGICA DE CONDIGURACION DE FRECUENCIA
void set_frequency_output() {
  // LOGICA DE hz2 (Decimales)
  digitalWrite(PIN_1, (hz2 == 1 || hz2 == 3 || hz2 == 5 || hz2 == 7 || hz2 == 9) ? HIGH : LOW);
  digitalWrite(PIN_2, (hz2 == 3 || hz2 == 7) ? HIGH : LOW);
  digitalWrite(PIN_3, (hz2 == 5 || hz2 == 7) ? HIGH : LOW);
  digitalWrite(PIN_4, (hz2 == 9) ? HIGH : LOW);

  // RESETEO DE hz1 (Enteros)
  digitalWrite(PIN_5, LOW);
  digitalWrite(PIN_6, LOW);
  digitalWrite(PIN_7, LOW);
  digitalWrite(PIN_8, LOW);
  digitalWrite(PIN_9, LOW);
  digitalWrite(PIN_10, LOW);
  digitalWrite(PIN_11, LOW);
  
  // LOGICA DE hz1 (Enteros)
  switch(hz1) {
    case 0:  digitalWrite(PIN_5, HIGH); digitalWrite(PIN_6, HIGH); digitalWrite(PIN_7, HIGH); digitalWrite(PIN_11, HIGH); break; // 87
    case 1:  digitalWrite(PIN_8, HIGH); digitalWrite(PIN_11, HIGH); break; // 88
    case 2:  digitalWrite(PIN_5, HIGH); digitalWrite(PIN_8, HIGH); digitalWrite(PIN_11, HIGH); break; // 89
    case 3:  digitalWrite(PIN_9, HIGH); digitalWrite(PIN_11, HIGH); break; // 90
    case 4:  digitalWrite(PIN_5, HIGH); digitalWrite(PIN_9, HIGH); digitalWrite(PIN_11, HIGH); break; // 91
    case 5:  digitalWrite(PIN_6, HIGH); digitalWrite(PIN_9, HIGH); digitalWrite(PIN_11, HIGH); break; // 92
    case 6:  digitalWrite(PIN_5, HIGH); digitalWrite(PIN_6, HIGH); digitalWrite(PIN_9, HIGH); digitalWrite(PIN_11, HIGH); break; // 93
    case 7:  digitalWrite(PIN_7, HIGH); digitalWrite(PIN_9, HIGH); digitalWrite(PIN_11, HIGH); break; // 94
    case 8:  digitalWrite(PIN_5, HIGH); digitalWrite(PIN_7, HIGH); digitalWrite(PIN_9, HIGH); digitalWrite(PIN_11, HIGH); break; // 95
    case 9:  digitalWrite(PIN_6, HIGH); digitalWrite(PIN_7, HIGH); digitalWrite(PIN_9, HIGH); digitalWrite(PIN_11, HIGH); break; // 96x
    case 10: digitalWrite(PIN_5, HIGH); digitalWrite(PIN_6, HIGH); digitalWrite(PIN_7, HIGH); digitalWrite(PIN_9, HIGH); digitalWrite(PIN_11, HIGH); break; // 97
    case 11: digitalWrite(PIN_8, HIGH); digitalWrite(PIN_9, HIGH); digitalWrite(PIN_11, HIGH); break; // 98
    case 12: digitalWrite(PIN_5, HIGH); digitalWrite(PIN_8, HIGH); digitalWrite(PIN_9, HIGH); digitalWrite(PIN_11, HIGH); break; // 99
    case 13: digitalWrite(PIN_10, HIGH); digitalWrite(PIN_11, HIGH); break; // 100
    case 14: digitalWrite(PIN_5, HIGH); digitalWrite(PIN_10, HIGH); digitalWrite(PIN_11, HIGH); break; // 101
    case 15: digitalWrite(PIN_6, HIGH); digitalWrite(PIN_10, HIGH); digitalWrite(PIN_11, HIGH); break; // 102
    case 16: digitalWrite(PIN_5, HIGH); digitalWrite(PIN_6, HIGH); digitalWrite(PIN_10, HIGH); digitalWrite(PIN_11, HIGH); break; // 103
    case 17: digitalWrite(PIN_7, HIGH); digitalWrite(PIN_10, HIGH); digitalWrite(PIN_11, HIGH); break; // 104
    case 18: digitalWrite(PIN_5, HIGH); digitalWrite(PIN_7, HIGH); digitalWrite(PIN_10, HIGH); digitalWrite(PIN_11, HIGH); break; // 105
    case 19: digitalWrite(PIN_6, HIGH); digitalWrite(PIN_7, HIGH); digitalWrite(PIN_10, HIGH); digitalWrite(PIN_11, HIGH); break; // 106
    case 20: digitalWrite(PIN_5, HIGH); digitalWrite(PIN_6, HIGH); digitalWrite(PIN_7, HIGH); digitalWrite(PIN_10, HIGH); digitalWrite(PIN_11, HIGH); break; // 107
  }
}

// ALARMA DE TEMPERATURA
void alarmaTEMP() {
  if (temp >= 61) {
    digitalWrite(PIN_ALARM_OUT, HIGH);
    delay(10);
    lcd.clear();
    while (temp > 57) {
      temp = analogRead(PIN_ADC_TEMP) / 11;
      lcd.setCursor(0, 1);
      lcd.print("  HIGH TEMPERATURE  ");
      lcd.setCursor(0, 2);
      lcd.print(" PLEASE WAIT COOLING");
      delay(200);
    }
    cnt = cnt = 1;
  }
  if (temp <= 55){
    digitalWrite(PIN_ALARM_OUT, LOW);
  }
}

// ALARMA ROE
void alarmaROE() {
  if (ref >= 20 || poten>=1080) {
    digitalWrite(PIN_ALARM_OUT, HIGH);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("   ROE PROTECTION   ");
    lcd.setCursor(0, 2);
    lcd.print("   RESTART SYSTEM   ");
    
    for (int car = 0; car <= 20; car++) {
    delay(40);
    }

    while (ref > 19 || poten > 1050) {
      lcd.setCursor(0, 1);
      lcd.print("   ROE PROTECTION   ");
      lcd.setCursor(0, 2);
      lcd.print("   RESTART SYSTEM   ");
      level = analogRead(PIN_ADC_LEVEL) / 4;
      if (level < 100) {
        digitalWrite(PIN_ALARM_OUT, LOW); 
        poten = poten = 0;
        ref = ref = 0;
        cnt = cnt = 1;
        lcd.clear();
      }
    }
  }
}

// MONITOREO DE ENTRADAS ADC
void monitoreo(){
  delay(20);
  poten = analogRead(PIN_ADC_POT) / 2;
  delay(20);
  ref = analogRead(PIN_ADC_REF) / 80;
  // Lectura DS18B20
  sensors.requestTemperatures(); // Pide la temperatura al sensor
  float tempC = sensors.getTempCByIndex(0); // Lee el primer sensor encontrado
  
  // Validación simple (por si el sensor se desconecta devuelve -127)
  if(tempC != DEVICE_DISCONNECTED_C) {
    temp = (int16_t)tempC; 
  }
}

void setup() {
  sensors.begin(); // Inicializa el sensor DS18B20
  sensors.setWaitForConversion(false); // Crucial: No esperar a la lectura
  ledcAttachChannel(freqPin, frequency, resolution, ledChannel); // CONFIGURACION DEL CANAL, FRECUENCIA y RESOLUCION

  frecuencia_off();
  digitalWrite(PIN_PLL_OUT, LOW);
  digitalWrite(PIN_ALARM_OUT, LOW);
  
  lcd.begin(20, 4); // CONFIGURACION DE LCD A USAR

  // CARGAR CARACTERES PARA NÚMEROS GRANDES
  lcd.createChar(0, LT); lcd.createChar(1, UB); lcd.createChar(2, RT);
  lcd.createChar(3, LL); lcd.createChar(4, LB); lcd.createChar(5, LR);
  lcd.createChar(6, UMB); lcd.createChar(7, LMB);

  // CONFIGURACION DE LOS PINES DE FRECUENCIA EN SALIDA
  int outputs[] = {PIN_1, PIN_2, PIN_3, PIN_4, PIN_5, PIN_6, PIN_7, PIN_8, PIN_9, PIN_10, PIN_11};
  for(int p : outputs) pinMode(p, OUTPUT);
  
  pinMode(PIN_MENU, INPUT_PULLUP);
  pinMode(PIN_OK, INPUT_PULLUP);
  pinMode(PIN_BAJAR, INPUT_PULLUP);
  pinMode(PIN_SUBIR, INPUT_PULLUP);
  pinMode(PIN_PLL_IN, INPUT);
  pinMode(PIN_PLL_OUT, OUTPUT);
  pinMode(PIN_ALARM_OUT, OUTPUT);

  // PANTALLA DE ARRANQUE DEL SISTEMA
  lcd.setCursor(0, 0); lcd.print("     ELECTRONICA    ");
  lcd.setCursor(0, 1); lcd.print("      EL AVION      ");
  lcd.setCursor(0, 2); lcd.print("    V-20572883-6    ");
  lcd.setCursor(0, 3); lcd.print("    MERIDA - VZLA   ");
  delay(1500);
  lcd.clear();

  // LOADING CON NUMEROS GRANDES
  lcd.setCursor(0, 0); lcd.print("   LOADING SYSTEM   ");
  for (int car = 0; car <= 100; car++) {
    printLargePercent(car);
    delay(40);
  }
  delay(100);
  lcd.clear();

  // LEE MEMORIA DONDE SE ALMACENA LA CONDIFGURACION DE FRECUENCIA
  prefs.begin("fm_radio", false);
  hz1 = prefs.getChar("hz1", 0);
  hz2 = prefs.getChar("hz2", 5);

  cnt = cnt = 5;
}

void loop() {
  ledcWrite(freqPin, 127); //SE ESTABLECE EL DUTY CYCLE, SE USA EL 50% DEL VALOR DE RESOLUCION DE 8 bits (255), PARA UNA ONDA CUADRADA PERFECTA

  if (cnt == 1 || cnt == 2){
    if (digitalRead(PIN_MENU) == LOW && digitalRead(PIN_BAJAR) == HIGH && digitalRead(PIN_SUBIR) == HIGH && digitalRead(PIN_OK) == HIGH) {
      cnt = cnt + 1;
      lcd.clear();
      delay(200);
      if (cnt == 3) {
        cnt = 1;
      }
      if (cnt == 4){
        cnt = cnt = 4;
      }
    }
  }

  switch (cnt) {
    case 1: // MONITOREO DE POTENCIA, REFLEJADA, FRECUENCIA
      hz1_lee = prefs.getInt("valorEntero", hz1);
      hz2_lee = prefs.getInt("valorDecimal", hz2);
      
      frec1 = hz1_lee + 87;
      
      monitoreo();      

      lcd.setCursor(0, 0); lcd.print("      OLIMPICA      ");
      lcd.setCursor(0, 1); lcd.print(" FREQ:");
      lcd.setCursor(7, 1); lcd.print("["); lcd.print(frec1); lcd.print("."); lcd.print(hz2_lee); lcd.print("Mhz"); lcd.print("]");
      lcd.setCursor(0, 2); lcd.print(" FWD:"); lcd.print(poten); lcd.print("w  ");
      lcd.setCursor(0, 3); lcd.print(" RFL:"); lcd.print(ref); lcd.print("w   ");
      lcd.setCursor(11, 2); lcd.print(" PLL:"); lcd.print(digitalRead(PIN_PLL_IN) ? "ON  " : "OFF ");
      
      estadoPll = estadoPll = digitalRead(PIN_PLL_IN);
      if(estadoPll == HIGH ){
        digitalWrite(PIN_PLL_OUT, HIGH);
      }else{
        digitalWrite(PIN_PLL_OUT, LOW);
      }
      
      lcd.setCursor(10, 3); lcd.print("  TEMP:"); lcd.print(temp); lcd.print("C ");
      
      alarmaTEMP();
      alarmaROE();

      if(digitalRead(PIN_MENU) == HIGH && digitalRead(PIN_SUBIR) == LOW && digitalRead(PIN_BAJAR) == LOW && digitalRead(PIN_OK) == HIGH){
        conteo++;
        if(conteo == 50){
           lcd.clear();
                             
           lcd.setCursor(0, 1);
           lcd.print("     PLEASE     ");
           lcd.setCursor(0, 2);
           lcd.print("      WAIT.     ");
           delay(600);
           
           lcd.clear();
           lcd.setCursor(0, 1);
           lcd.print("     PLEASE     ");
           lcd.setCursor(0, 2);
           lcd.print("      WAIT. .   ");
           delay(600);
           
           lcd.clear();
           lcd.setCursor(0, 1);
           lcd.print("     PLEASE     ");
           lcd.setCursor(0, 2);
           lcd.print("      WAIT. . . ");
           delay(1000);

           lcd.clear();
           conteo = conteo = 0;
           cnt=cnt=4;
        }
      }else{
        conteo = conteo = 0;
      }
      
    break;

    case 2: // MODULACION DE AUDIO
      alarmaTEMP();
      alarmaROE();
      monitoreo();
      temporizador();
      
      estadoPll = estadoPll = digitalRead(PIN_PLL_IN);
      if(estadoPll == HIGH ){
        digitalWrite(PIN_PLL_OUT, HIGH);
      }else{
        digitalWrite(PIN_PLL_OUT, LOW);
      }

      lcd.setCursor(0, 0); lcd.print("     MODULATION     ");
      lcd.setCursor(0, 1); lcd.print("        MPX         ");
      lcd.setCursor(0, 2); lcd.print("---------------");
      lcd.setCursor(15, 2); lcd.write(255);
      lcd.setCursor(16, 2); lcd.write(255);
      lcd.setCursor(17, 2); lcd.write(255);
      lcd.setCursor(18, 2); lcd.write(255);
      lcd.setCursor(19, 2); lcd.write(255);
      // BARRA DE ENTRADA DE AUDIO
      for(int j = 0; j < 8; j++){
        lcd.createChar(j, block[j]);
      }
      if(millis() < lastT){
          return;
   
      lastT += T_REFRESH;
      }
      modAudio = modAudio = map(sqrt(analogRead(PIN_ADC_MOD)*16), 0, 128, 0, 80);
       
      bar(0, modAudio);
      
    break;

    case 4: // CONFIGURACION DE FRECUENCIA

      if (sub == 7 || sub == 8){

          if (digitalRead(PIN_BAJAR) == HIGH && digitalRead(PIN_SUBIR) == HIGH && digitalRead(PIN_MENU) == LOW && digitalRead(PIN_OK) == HIGH){
            sub = sub + 1;
            lcd.clear();
            delay(200);
            if (sub == 9){
              sub = sub = 7;
            }
          }
      }
          
      switch (sub) {
        
        case 7:
        
          frec1 = hz1 + 87;

          if (digitalRead(PIN_BAJAR) == LOW || digitalRead(PIN_SUBIR) == LOW){
            frecuencia_off();
            digitalWrite(PIN_PLL_OUT, LOW);
          }

          if (digitalRead(PIN_SUBIR) == LOW && digitalRead(PIN_BAJAR) == HIGH && digitalRead(PIN_MENU) == HIGH) { 
            delay(150); 
            hz1 = hz1 + 1; 
          }
            
          if (digitalRead(PIN_BAJAR) == LOW && digitalRead(PIN_SUBIR) == HIGH && digitalRead(PIN_MENU) == HIGH) { 
            delay(150); 
            hz1 = hz1 - 1;
          }
    
          // Lógica de límites
          if(hz1==21){        //CUANDO hz2==10 le resta 10 al primer digito y suma 1 a la frecuencia 87 a 107                    
              delay(50);
              hz1 = hz1 - 21;          
          }
           
          if(hz1==-1){         //CUANDO hz2==-2 le suma 10 al primer digito y resta 1 a la frecuencia 87 a 107
              delay(50);
              hz1 = hz1 + 21;
          }
          
          lcd.setCursor(0, 0); lcd.print(" FREQUENCY  SETTING ");
          lcd.setCursor(0, 1); lcd.print("--------------------");
          lcd.setCursor(0, 2); lcd.print("SELECT:");
          lcd.setCursor(8, 2); lcd.print("["); lcd.print(frec1); lcd.print("]"); lcd.print("."); lcd.print(hz2); lcd.print("Mhz ");
          lcd.setCursor(0, 3); lcd.print("[87-107]");

          // Guardar
          if (digitalRead(PIN_OK) == LOW && digitalRead(PIN_BAJAR) == HIGH && digitalRead(PIN_SUBIR) == HIGH && digitalRead(PIN_MENU) == HIGH) {
            prefs.putChar("hz1", hz1);
            prefs.putChar("hz2", hz2);
            
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("     FREQUENCY      ");
            lcd.setCursor(0, 1); lcd.print("       SAVED        ");
            lcd.setCursor(0, 2); lcd.print("    SUCCESSFULLY    ");
            delay(1500);
            lcd.clear();
            cnt = cnt = 5;
            sub = sub = 7;
            prefs.end();
          }
        break;

        case 8:
        
          frec1 = hz1 + 87;

          if (digitalRead(PIN_SUBIR) == LOW && digitalRead(PIN_BAJAR) == HIGH && digitalRead(PIN_MENU) == HIGH) { 
            delay(150); 
            hz2 = hz2 + 2; 
          }
            
          if (digitalRead(PIN_BAJAR) == LOW && digitalRead(PIN_SUBIR) == HIGH && digitalRead(PIN_MENU) == HIGH) { 
            delay(150); 
            hz2 = hz2 - 2;
          }
    
          // Lógica de límites   
          if(hz2==11){        //CUANDO hz2==10 le resta 10 al primer digito y suma 1 a la frecuencia 87 a 107                    
              delay(50);
              hz2 = hz2 - 10;          
          }
           
          if(hz2==-1){         //CUANDO hz2==-2 le suma 10 al primer digito y resta 1 a la frecuencia 87 a 107
              delay(50);
              hz2 = hz2 + 10;
          }

          if (digitalRead(PIN_BAJAR) == LOW || digitalRead(PIN_SUBIR) == LOW){
            frecuencia_off();
            digitalWrite(PIN_PLL_OUT, LOW);
          }
          
          lcd.setCursor(0, 0); lcd.print(" FREQUENCY  SETTING ");
          lcd.setCursor(0, 1); lcd.print("--------------------");
          lcd.setCursor(0, 2); lcd.print("SELECT:");
          lcd.setCursor(8, 2); lcd.print(frec1); lcd.print("."); lcd.print("["); lcd.print(hz2); lcd.print("]"); lcd.print("Mhz ");
          lcd.setCursor(0, 3); lcd.print("[1,3,5,7,9]");
    
          // Guardar
          if (digitalRead(PIN_OK) == LOW && digitalRead(PIN_BAJAR) == HIGH && digitalRead(PIN_SUBIR) == HIGH && digitalRead(PIN_MENU) == HIGH) {
            prefs.putChar("hz1", hz1);
            prefs.putChar("hz2", hz2);
            
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("     FREQUENCY      ");
            lcd.setCursor(0, 1); lcd.print("       SAVED        ");
            lcd.setCursor(0, 2); lcd.print("    SUCCESSFULLY    ");
            delay(1500);
            lcd.clear();
            cnt = cnt = 5;
            sub = sub = 7;
            prefs.end();
          }
        break;
      }
    break;

    case 5:
      set_frequency_output();
      delay(200);
      lcd.clear();
      delay(200);
      cnt = cnt = 1;

    break;
  }
  delay(10);
}
