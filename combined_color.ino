// Include the necessary libraries
#include <ESP8266WiFi.h>
#include <Adafruit_TCS34725.h>
#include <Wire.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "PIXEL CONNECT";
char pass[] = "Pixel@frica2023.";

// Define the buzzer pin
#define BUZZER_PIN D6

// Telegram Bot Token and Chat ID
#define BOTtoken "7237669698:AAE51D1ep89PiDGnEYCd3gx2AqttBT1o8B0"
#define CHAT_ID "6777570976"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Initialize the color sensor with specific integration time and gain
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);

void setup() {
  // Initialize the serial communication
  Serial.begin(9600);

  // Initialize the buzzer pin as an output
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Ensure the buzzer is off at the start

  // Initialize the color sensor
  if (tcs.begin()) {
    Serial.println("Sensor is working");
  } else {
    Serial.println("Sensor not connected, please check the connection...");
    while (1);
  }

  // WiFi and Telegram setup
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  bot.sendMessage(CHAT_ID, "System is Ready", "");
  delay(1000);
}

// Function to determine the color name from RGB values
String getColorName(uint16_t r, uint16_t g, uint16_t b) {
  if (r > 200 && g > 200 && b > 200) return "White";
  if (r > 200 && g < 100 && b < 100) return "Red";
  if (r < 100 && g > 200 && b < 100) return "Green";
  if (r < 97 && g < 182 && b > 214) return "Blue";
  if (r > 200 && g > 200 && b < 100) return "Yellow";
  if (r > 200 && g < 100 && b > 200) return "Magenta";
  if (r < 100 && g > 200 && b > 200) return "Cyan";
  if (r < 73 && g < 140 && b < 135) return "Black";
  return "Unknown";
}

void loop() {
  uint16_t r, g, b, c, colorTemp, lux;

  // Get the raw data from the color sensor
  tcs.getRawData(&r, &g, &b, &c);

  // Calculate color temperature and lux (for informational purposes)
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);

  // Determine the color name based on the RGB values
  String colorName = getColorName(r, g, b);

  // Print the color data to the serial monitor
  Serial.print("Color Temp: "); Serial.print(colorTemp); Serial.print(" K - ");
  Serial.print("Lux: "); Serial.print(lux); Serial.print(" - ");
  Serial.print("Color: "); Serial.print(colorName); Serial.print(" - ");
  Serial.print("Red: "); Serial.print(r); Serial.print(" ");
  Serial.print("Green: "); Serial.print(g); Serial.print(" ");
  Serial.print("Blue: "); Serial.print(b); Serial.print(" ");
  Serial.print("Clear: "); Serial.print(c); Serial.print(" ");
  Serial.println(" ");

  // If the detected color is black, turn on the buzzer, send a Telegram message, and print a message
  if (colorName == "Black") {
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("Buzzer ON: Black color detected");
    bot.sendMessage(CHAT_ID, "Black color detected!", "");
    Serial.println("Telegram message sent");
    delay(2000);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Add a delay to make the output more readable
  delay(500);
}
