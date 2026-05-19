#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 with 16 columns and 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();         // Initialize the LCD
  lcd.backlight();    // Turn on the backlight
  
  lcd.setCursor(0, 0);       // Go to column 0, row 0 (Top line)
  lcd.print("Hello World!"); // Print your message
  
  lcd.setCursor(0, 1);       // Go to column 0, row 1 (Bottom line)
  lcd.print("It works!");    // Print your second message
}

void loop() {
  // Clear loop. Nothing needs to repeat to keep text on screen.
}
