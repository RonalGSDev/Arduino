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

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
}

String ingreso;
String auxIngreso;
String Contra = "1234";
int contFallos=1;
int duplicadorEspera=1;
bool puerta=false;


void loop() {

  char customKey = customKeypad.getKey();

  if(contFallos <= 3){

    if(!puerta){
      lcd.setCursor(0, 0); lcd.print("Password:");
      
      if (customKey >= '0' && customKey <= '9') {

        ingreso += customKey;
        auxIngreso+="*";
        lcd.setCursor(0, 1); lcd.print(auxIngreso);

      }else if (customKey == '#') {

        if (ingreso.length() > 0) {
          ingreso.remove(ingreso.length() - 1);
          auxIngreso.remove(auxIngreso.length()-1);
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

    if (customKey == 'A' && puerta==false) {

      if (ingreso == Contra) {
        lcd.clear();
        lcd.setCursor(4, 0); lcd.print("Password");
        lcd.setCursor(4, 1); lcd.print("Correcto");
        puerta=true;
        contFallos=1;
        duplicadorEspera=1;

      }else {
        Serial.println("Contraseña Incorrecta");
        lcd.clear();
        lcd.setCursor(4, 0); lcd.print("Password");
        lcd.setCursor(3, 1); lcd.print("Incorrecto");
        delay(1000);

        lcd.clear();
        lcd.setCursor(3, 0); lcd.print("Fallo No."+ String(contFallos));
        contFallos+=1;
      }
      ingreso = "";
      auxIngreso = "";
      delay(1000);
      lcd.clear();
    }
    
    if(puerta){

      lcd.setCursor(0, 0); lcd.print("B-Cerrar Puerta");
      lcd.setCursor(0, 1); lcd.print("C-Nueva Password");

      if(customKey == 'B'){
        puerta=false;
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("Cerrando....");
        delay(3000);
        lcd.clear();
        //colocar codigo para mover el servo y cerrar la puerta
      }else if(customKey == 'C'){
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("Nuevo Password:");

        delay(2000);
      }
      
    }

  }else{
    lcd.clear();

    for(int i=15*duplicadorEspera; i>=1;i--){
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Intente de nuevo"); 
      lcd.setCursor(4, 1); lcd.print("en " +String(i) +" seg");
      delay(1000);
    }
    duplicadorEspera++;
    contFallos=1;
    lcd.clear();
  }
}

