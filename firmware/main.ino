#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// Initialize BME280
#define BME_SDA A4
#define BME_SCL A5
Adafruit_BME280 bme; // I2C

// Initialize the LCD. The address (0x27) and size (16, 2) may vary for your display
int lcdCols = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdCols, lcdRows);

const char* ssid = "arduino";
const char* password = "arduino1234";

unsigned long lastPostTime = 0;
bool showPressure = false;


void setup() {
    Serial.begin(9600);
    while (!Serial) {
        delay(10);
    }

    // 1. Initialize display
    lcd.init(); // Initialize the LCD
    lcd.backlight(); // Turn on the backlight
    lcd.setCursor(0, 0);
    lcd.print("Hello, there!");
    lcd.setCursor(0, 1);
    lcd.print("Loading data...");

    // 2. Check BME280 sensor
    if (!bme.begin(0x76)) {
        clearLCDLine(1);
        lcd.print("Could not find BME280 sensor, check wiring!");
        Serial.println("Could not find BME280 sensor, check wiring!");
        while (1);
    }

    clearLCDLine(1);
    lcd.setCursor(0, 1);
    lcd.print("WiFi Setup");

    // 2. Connect to WiFi
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        clearLCDLine(1);
        lcd.print("Loading data...");
        Serial.println("Connecting to WiFi...");
    }

    clearLCDLine(1);
    lcd.print("Connected to WiFi");
    Serial.println("Connected to WiFi");
}

void loop() {
    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;

    // Get weather API
    String message = "Nublado com chuvas! ";

    unsigned long currentTime = millis();
    if (currentTime - lastPostTime > 60000) {
        lastPostTime = currentTime;

        // Display the BME280 data on the display
        displayData(temperature, humidity, pressure, showPressure, message);

        // Post BME280 data to the endpoint
        postData(temperature, humidity, pressure);
        
        // Regularly consume an endpoint to check if it's going to rain
        checkRain();

        showPressure = !showPressure;     
    }
}

void displayData(float temperature, float humidity, float pressure) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(humidity);
    lcd.print("%");

    // Due to space constraints, you might need to cycle the pressure reading separately or display it based on an event
}

void postData(float temperature, float humidity, float pressure) {
    HTTPClient http;
    http.begin("YOUR_ENDPOINT");
    http.addHeader("Content-Type", "application/json");
    String postData = String("{\"temperature\":") + temperature + ",\"humidity\":" + humidity + ",\"pressure\":" + pressure + "}";
    int httpCode = http.POST(postData);
    http.end();
}

void checkRain() {
    HTTPClient http;
    http.begin("YOUR_RAIN_CHECK_ENDPOINT");
    int httpCode = http.GET();
    if (httpCode > 0) {
        String payload = http.getString();
        if (payload == "true") {
        // Speak that the rain is coming
        // ...
        }
    }
    http.end();
}

void clearLCDLine(int line) {
    // Clear the line
    lcd.setCursor(0, line); // Set cursor to the beginning of the first line
    for(int i = 0; i < lcdCols; i++) {
        lcd.print(" "); // Overwrite the line with spaces
    }
}