#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display

bool showP = false;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }

  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.setCursor(0, 0);

  if (!bme.begin(0x76)) {
    lcd.print("Could not find BME280 sensor, check wiring!");
    Serial.println("Could not find BME280 sensor, check wiring!");
    while (1);
  }

  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.setCursor(0, 0);
  lcd.print("Hello, Davi!");
  lcd.setCursor(0, 1);
  lcd.print("Loading data...");

  delay(2000);
}

void loop() {

  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;

  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print("C");

  Serial.print(" | Hum: ");
  Serial.print(humidity);
  Serial.print("%");

  Serial.print(" | Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  String message = "Nublado com chuvas! ";

  displayData(temperature, humidity, pressure, showP, message);

  showP = !showP;
}

void displayData(float t, float h, float p, bool showP, String phrase) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(t);
  lcd.setCursor(5, 0);
  lcd.print("C");

  lcd.setCursor(10, 0);
  lcd.print(h);
  lcd.setCursor(15, 0);
  lcd.print("%");

  if (showP) {
    lcd.setCursor(0, 1);
    lcd.print(p);
    lcd.setCursor(7, 1);
    lcd.print("hPa");

    delay(5000); // Adjust delay as needed
  }
  else {
    int len = phrase.length();
    int displayWidth = 16; // The number of columns in the LCD

    for (int offset = 0; offset < 10; offset++) { // Infinite loop
      // Calculate the start and end indices for the substring
      int startIdx = offset % len;
      int endIdx = startIdx + displayWidth;
      String toDisplay;

      if (endIdx <= len) {
        // No wrap-around needed
        toDisplay = phrase.substring(startIdx, endIdx);
      } else {
        // Wrap around the end of the phrase to the beginning
        toDisplay = phrase.substring(startIdx, len) + phrase.substring(0, endIdx % len);
      }

      // Set the cursor position
      lcd.setCursor(0, 1); // Assuming you want to display it on the second row

      // Print the substring on LCD and Serial
      lcd.print(toDisplay);
      Serial.println(toDisplay);

      // Delay to control the speed of scrolling
      delay(500); // Adjust delay as needed

      // Clear the part of the LCD where the text is displayed to prevent ghosting
      lcd.setCursor(0, 1);
      for (int i = 0; i < displayWidth; i++) {
        lcd.print(" "); // Print spaces to clear
      }
    }
    // // Loop through each character of the phrase
    // if (phrase.length() > 16) {
    //   int remainingChars = phrase.length() % 16;
    //   for (int i = 0; i < remainingChars; i++) {
    //     // Set the cursor position to display the current character
    //     lcd.setCursor(0, 1); // 16 is the number of columns in the LCD
      
    //     // Print the current character
    //     Serial.println(phrase.substring(i, 16+remainingChars));
    //     lcd.print(phrase.substring(i, 16+remainingChars));
        
    //     // Delay to control the speed of scrolling
    //     delay(500); // Adjust delay as needed
    //   }
    // }
    // else {
    //   lcd.print(phrase);
    // }
  }
}