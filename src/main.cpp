#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
#include "tm1637display.h"
#include <U8glib.h>

int led_red = 3;
int button_pin = 2;     // пин кнопки

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
// boolean displayCode = false; // флаг для отображения количества нажатий кнопок (кода устройства)
boolean loading = true; // флаг для отображения загрузки
const unsigned long debounceDelay = 500; // задержка для устранения дребезга

void string2display(char* msg){
  OurDisplay.clear();
  My_u8g_Panel.firstPage();
    do {
    u8g_prepare();
    My_u8g_Panel.drawStr( 5, 10, msg);
  } while ( My_u8g_Panel.nextPage() ); 
}

void displayWrite (int num) {
  OurDisplay.clear();
  My_u8g_Panel.firstPage();
    do {
    u8g_prepare();
    String code = "code:" + num;
    My_u8g_Panel.drawStr( 5, 10, "code:");
    My_u8g_Panel.drawStr( 40, 10, String(num).c_str());
  } while ( My_u8g_Panel.nextPage() );  
  
}
void blink(uint8_t pin, int times, int delayTime) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(delayTime);
    digitalWrite(pin, LOW);
    delay(delayTime);
  }
}


volatile uint32_t debounce;
volatile uint8_t buttonPressCount = 0;

// boolean ledOn = false;        // текущее состояние свтодиода
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
volatile unsigned long lastDebounceTime = 0;
volatile boolean interrupted = false;

void handleButtonPress() {
  interrupted = true;
  unsigned long currentTime = millis();
  if ((currentTime - lastDebounceTime) > debounceDelay) {
    buttonPressCount++;
    lastDebounceTime = currentTime;
  }
}

void setup() {
  Serial.begin(9600); // Start serial monitor
  OurDisplay.setBrightness(7);
  OurDisplay.clear();
  pinMode(button_pin, INPUT); // Инициализируем цифровой вход/выход в режиме входа.
  pinMode(led_red, OUTPUT); // set pin 13 as output pin
   My_u8g_Panel.firstPage();
  attachInterrupt(digitalPinToInterrupt(button_pin), handleButtonPress, RISING); // Настройка прерывания на падение сигнала
  //выводим однажды подсказуку о программировании кода
  My_u8g_Panel.firstPage();
  do {
    u8g_prepare();
    My_u8g_Panel.drawStr( 5, 10, "programming code");
    My_u8g_Panel.drawStr( 5, 30, "for 30 sec");
  } while ( My_u8g_Panel.nextPage() );  
  Serial.println("programming code for 30 sec");
}
void displayCode(){
    displayWrite(buttonPressCount);
    Serial.println(buttonPressCount);
    blink(led_red, buttonPressCount, 800);
    string2display("code is set");
}
boolean wasClicked(){
   currentButten = debvance(lastButten);
    if (lastButten != currentButten) { // Если кнопка нажата 
      digitalWrite(led_red, currentButten);// зажигаем светодиод
      lastButten = currentButten;
      return true;
    }
    return false;
}
void loop () {
  // чтение прерывание только после загрузки контроллера
  if (loading && millis() >= 10000) {
    loading = false;
    detachInterrupt(button_pin);
    displayCode();
  }
  //мигаем диодом при нажатии кнопки во время программирования контроллера
  if(loading){
    if (wasClicked()){
      digitalWrite(led_red, currentButten);
    }
  }
  //рабочий цикл контроллера
  if(!loading ) {
    currentButten = debvance(lastButten);
    if (lastButten != currentButten) { // Если кнопка нажата 
      digitalWrite(led_red, currentButten);// зажигаем светодиод
      lastButten = currentButten;
      string2display(currentButten ? "button pressed" : "button released");
    }
  }
  
}

