#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "tm1637display.h"
#include <U8glib.h>

int led_red = 3;
int button_pin = 5;     // пин кнопки

/**************/
// Define the display connections pins:
#define DCLK 6
#define DIO 5
// Create display object of type TM1637Display
// The display shows the current score
TM1637Display OurDisplay = TM1637Display(DCLK, DIO);
// Create the oled display object - this shows gameplay
// and welcome/win/loss messages

U8GLIB_SH1106_128X64  My_u8g_Panel(U8G_I2C_OPT_NONE); // I2C / TWI
void u8g_prepare(void) {
  My_u8g_Panel.setFont(u8g_font_6x10);
  My_u8g_Panel.setFontRefHeightExtendedText();
  My_u8g_Panel.setDefaultForegroundColor();
  My_u8g_Panel.setFontPosTop();
}
void draw(void) {
  u8g_prepare();
  My_u8g_Panel.drawStr( 0, 10, "Welcome to");
  My_u8g_Panel.drawStr( 10, 30, "Dino!!");
  My_u8g_Panel.drawStr( 0, 50, "Push to begin");

}

//######################
#define SCREEN_WIDTH        128 // OLED display width, in pixels
#define SCREEN_HEIGHT        64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// #define OLED_RESET            4 // Reset pin # (or -1 if sharing Arduino reset pin)
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/**************/

/* переменные */
boolean lastButten = LOW;     // предыдущее состояние кнопки
boolean currentButten = LOW;  // текущее состояние кнопки
boolean ledOn = false;        // текущее состояние свтодиода
boolean debvance (boolean last) //убираем дребизг
{
  boolean current = digitalRead (button_pin); // считываем данные с кнопки
  if (last != current) // Если текущее состояние отличететься от предыдущего 
  {
   delay (5);   // Ждем 5 милисек.
   current = digitalRead (button_pin); // Присваеваем текущее состояние кнопки
    return current; 
  }
  return current;
}

void setup() {
  Serial.begin(9600); // Start serial monitor
  OurDisplay.setBrightness(7);
  OurDisplay.clear();

  // if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
  //   Serial.println(F("SSD1306 allocation failed"));
  //   for(;;);
  // }
  pinMode(button_pin, INPUT); // Инициализируем цифровой вход/выход в режиме входа.
  pinMode(led_red, OUTPUT); // set pin 13 as output pin
   My_u8g_Panel.firstPage();
   do{
  u8g_prepare();
  My_u8g_Panel.drawStr( 0, 10, "press any key");
   }
  while ( My_u8g_Panel.nextPage() );
  // Serial.println(F("SSD1306 ready"));
  // display.setTextSize(1);
  // display.setTextColor(SSD1306_WHITE);
  // display.setCursor(2,50);
  // display.println(F("GAMEOVER"));
  // display.display();
  // delay(1000);
}
void drawScore() {
  OurDisplay.showNumberDecEx(9876);
  // display.setTextSize(1);
  // display.setTextColor(SSD1306_WHITE);
  // display.setCursor(2,2);
  // display.print(F("Score:"));
  // display.println(98765);
  // display.display();
}
// void ShowScore() {
//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(SSD1306_WHITE);
//   display.setCursor(2,2);
//   display.print(F("Score:"));
//   display.println(currentButten == HIGH? "HIGH" : "LOW");
//   display.display();
// }

void loop () {
  // drawScore();
  currentButten = debvance(lastButten);
   if (lastButten != currentButten) { // Если кнопка нажата 
      // drawScore();
      digitalWrite(led_red, currentButten);// зажигаем светодиод
      lastButten = currentButten;
      String str1 = "switch to ";
      Serial.print(str1);
      String str2 = currentButten == HIGH? "HIGH" : "LOW";
      Serial.println(str2);
  My_u8g_Panel.firstPage();
  do {
    // draw();
  u8g_prepare();
  My_u8g_Panel.drawStr( 0, 10, currentButten == HIGH? "HIGH" : "LOW");
  } while ( My_u8g_Panel.nextPage() );
 }
  
}