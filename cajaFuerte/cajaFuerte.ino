#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(bocina, OUTPUT);
}

String Contra = "1234";
String ingreso;
String auxIngreso;

int contFallos = 1;
int duplicadorEspera = 1;
bool puerta = false;

void loop() {
  char customKey = customKeypad.getKey();

  if (customKey) {
    tone(bocina, 5000, 100); 
  }

  if (contFallos <= 3) {

    if (!puerta) {
      lcd.setCursor(0, 0); lcd.print("Password:");
      
      if (customKey >= '0' && customKey <= '9') {
        ingreso += customKey;
        auxIngreso += "*";
        lcd.setCursor(0, 1); lcd.print(auxIngreso);

      } else if (customKey == '#')  {
        if (ingreso.length() > 0) {
          ingreso.remove(ingreso.length() - 1);
          auxIngreso.remove(auxIngreso.length() - 1);
          lcd.clear();
          lcd.setCursor(0, 1); lcd.print(auxIngreso);
        }

      } else if (customKey == '*') {
        ingreso = "";
        auxIngreso = "";
        lcd.clear();
        lcd.setCursor(0, 1); lcd.print(auxIngreso);
      }
    }

    //Verificacion de la contraseña con la tecla A
    if (customKey == 'A' && !puerta) {
      if (ingreso == Contra) {
        lcd.clear();
        lcd.setCursor(4, 0); lcd.print("Password");
        lcd.setCursor(4, 1); lcd.print("Correcto");
        puerta = true;
        contFallos = 1;
        duplicadorEspera = 1;
        //----servo abrir puerta pentiente
        
      } else {

        lcd.clear();
        lcd.setCursor(4, 0); lcd.print("Password");
        lcd.setCursor(3, 1); lcd.print("Incorrecto");
        delay(1000);

        lcd.clear();tone(bocina, 5000, 1000);
        lcd.setCursor(3, 0); lcd.print("Fallo No." + String(contFallos));
         
        contFallos++;
      }
      ingreso = "";
      auxIngreso = "";
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
        delay(3000);
        lcd.clear();
        // ----Servo cerrar puerta pendiente
      }

      // cambio de contra con la tecla C
      else if (customKey == 'C') {
        cambiarPassword(); 
      }
    }

  } else {
    lcd.clear();
    for (int i = 10 * duplicadorEspera; i >= 1; i--) {
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Intente de nuevo"); 
      lcd.setCursor(4, 1); lcd.print("en " + String(i) + " seg");
      tone(bocina, 5000, 100); 
      delay(1000);
    }
    duplicadorEspera++;
    contFallos = 1;
    lcd.clear();
  }
}


void cambiarPassword() {

  String nuevaContra = "";
  String nuevaAuxContra = "";
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Nuevo Password:");

  while (true) {
    //sonido al presionar una tecla
    char customKey = customKeypad.getKey();
     if (customKey) {
      tone(bocina, 5000, 100);
    }

    if (customKey >= '0' && customKey <= '9') { 
      nuevaContra += customKey;
      nuevaAuxContra += "*";
      lcd.setCursor(0, 1); lcd.print(nuevaAuxContra);
    }

    if (customKey == '#') { 
      if (nuevaContra.length() > 0) {
        nuevaContra.remove(nuevaContra.length() - 1);
        nuevaAuxContra.remove(nuevaAuxContra.length() - 1);
        lcd.clear();
        lcd.setCursor(0, 1); lcd.print(nuevaAuxContra);
      }
    }

    if (customKey == '*') {
        nuevaContra = "";
        nuevaAuxContra = "";
        lcd.clear();
        lcd.setCursor(0, 1); lcd.print(nuevaAuxContra);
      }

    if (customKey == 'D') { 
      if (nuevaContra.length() > 0) {
        Contra = nuevaContra;
        lcd.clear();
        lcd.setCursor(4, 0); lcd.print("Password");
        lcd.setCursor(4, 1); lcd.print("Cambiado");
        delay(2000);
        lcd.clear();
        break;
      }
    }

    
  }
}
