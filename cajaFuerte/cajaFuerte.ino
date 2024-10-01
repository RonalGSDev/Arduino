#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h> 

LiquidCrystal_I2C lcd(0x27, 16, 2);
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {7, 6, 5, 4};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int bocina = 3;

String Contra;
String ingreso;
String auxIngreso;

int contFallos = 1;
int duplicadorEspera = 1;
bool puerta = false;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(bocina, OUTPUT);
  //borrarEEPROM();
  leerContraMemoria();
  if (Contra == "") {
    Contra = "1234";
    guardarContraMemoria(Contra);
  }
}



void loop() {

  char customKey = customKeypad.getKey();

  if (contFallos <= 3) {
    if (customKey) {
      tone(bocina, 5000, 100); 
    }

    if (!puerta) {
      lcd.setCursor(0, 0); lcd.print("Password:");
      ingresoTeclas(customKey,ingreso,auxIngreso);   
    }

    if (customKey == 'A' && !puerta) {
      if (ingreso == Contra) {
        lcd.clear();
        lcd.setCursor(4, 0); lcd.print("Password");
        lcd.setCursor(4, 1); lcd.print("Correcto");
        puerta = true;
        contFallos= duplicadorEspera = 1;

        //----servo abrir puerta pentiente
        
      } else {
        tone(bocina, 5000, 1000);
        lcd.clear();
        lcd.setCursor(4, 0); lcd.print("Password");
        lcd.setCursor(3, 1); lcd.print("Incorrecto");
        delay(1000);
        lcd.clear();
        lcd.setCursor(3, 0); lcd.print("Fallo No." + String(contFallos));
         
        contFallos++;
      }
      ingreso = auxIngreso = "";
      delay(1000);
      lcd.clear();
    }

    // Puerta abierta
    if (puerta) {
      lcd.setCursor(0, 0); lcd.print("B-Cerrar Puerta");
      lcd.setCursor(0, 1); lcd.print("C-Nueva Password");

      // Cerrar la puerta
      if (customKey == 'B') {

        puerta = false;
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("Cerrando....");
        delay(2000);
        lcd.clear();
        // ----Servo cerrar puerta pendiente


      }//cambiar contra
      else if (customKey == 'C') {
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("Nuevo Password:");
        cambiarPassword(); 
      }
    }

  } else {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Intente de nuevo"); 
    for (int i = 10 * duplicadorEspera; i >= 1; i--) {
      lcd.setCursor(4, 1); lcd.print("                            ");
      lcd.setCursor(4, 1); lcd.print("en " + String(i) + " seg");
      tone(bocina, 5000, 100); 
      delay(1000);
    }
    duplicadorEspera++;
    contFallos = 1;
    lcd.clear();
  }
}

void ingresoTeclas(char& caracter, String& cadena, String& auxCadena){

  if (caracter >= '0' && caracter <= '9') { 
    cadena += caracter;
    auxCadena += "*";
    lcd.setCursor(0, 1); lcd.print(auxCadena);

  }else if (caracter == '#') { 
    if (cadena.length() > 0) {
      cadena.remove(cadena.length() - 1);
      auxCadena.remove(auxCadena.length() - 1);
      lcd.setCursor(0, 1); lcd.print("                        ");
      lcd.setCursor(0, 1); lcd.print(auxCadena);
    }
  }else if (caracter == '*') {
    cadena = auxCadena = "";
    lcd.setCursor(0, 1); lcd.print("                        ");
    lcd.setCursor(0, 1); lcd.print(auxCadena);
  }
}

void cambiarPassword() {

  String nuevaContra = "";
  String auxNuevaContra = "";
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Nuevo Password:");

  while (true) {
    //sonido al presionar una tecla
    char customKey = customKeypad.getKey();
    if (customKey) {
      tone(bocina, 5000, 100); 
    }

    if (customKey == 'D') {
      if (nuevaContra.length() > 0) {
        Contra = nuevaContra;
        guardarContraMemoria(Contra);
        lcd.clear();
        lcd.setCursor(4, 0); lcd.print("Password");
        lcd.setCursor(4, 1); lcd.print("Cambiado");
        delay(2000);
        lcd.clear();
        
        break;
      }
    }else{
      ingresoTeclas(customKey,nuevaContra,auxNuevaContra);
    }
  }
}


void leerContraMemoria() {
  Contra = "";
  char caracter;
  for (int i = 0; i < 20; i++) {
    caracter = EEPROM.read(i);
    if (caracter == '\0')break;
    Contra += caracter;
  }
}

void guardarContraMemoria(String& contraMemoria) {
  for (int i = 0; i < contraMemoria.length(); i++) {
    EEPROM.write(i, contraMemoria[i]);
  }
  EEPROM.write(contraMemoria.length(), '\0'); 
}

void borrarEEPROM() {
  for (int i = 0; i < 20; i++) { 
    EEPROM.write(i, '\0');
  }
}
