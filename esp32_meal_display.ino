#include <Arduino.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

// Definește pinii pentru segmente
#define SEG_A 4
#define SEG_B 16
#define SEG_C 17
#define SEG_D 5
#define SEG_E 18
#define SEG_F 19
#define SEG_G 21
#define DIGIT1 15
#define DIGIT2 2
#define DOT 22

#define MAX_TIME 5 // Timpul maxim în milisecunde pentru multiplexare

// Maparea segmentelor pentru literele A-Z și spațiu
const uint8_t letterMap[27] = {
  0b11101110, // A
  0b00111110, // B
  0b10011100, // C
  0b01111010, // D
  0b10011110, // E
  0b10001110, // F
  0b10111100, // G
  0b01101110, // H
  0b00001100, // I
  0b01111000, // J
  0b10101110, // K
  0b00011100, // L
  0b11101100, // M
  0b00101010, // N
  0b11111100, // O
  0b11001110, // P
  0b11100110, // Q
  0b00001010, // R
  0b10110110, // S
  0b10001100, // T
  0b01111100, // U
  0b01000110, // V
  0b00011110, // W
  0b01101100, // X
  0b01110110, // Y
  0b11011011, // Z
  0b00010000  // Space (spațiu)
};

// Maparea segmentelor pentru cifrele 0-9
const uint8_t numberMap[10] = {
  0b11111100, // 0
  0b01100000, // 1
  0b11011010, // 2
  0b11110010, // 3
  0b01100110, // 4
  0b10110110, // 5
  0b10111110, // 6
  0b11100000, // 7
  0b11111110, // 8
  0b11110110  // 9
};

// Array de pini pentru controlul segmentelor
const uint8_t segmentPins[7] = { SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G };

int count = 0; // Contor pentru schimbarea literelor
unsigned long previousMillis = 0; // Timpul anterior la care s-a schimbat litera
unsigned long interval = 330; // Intervalul pentru schimbarea literelor

// Textul de afișat
char text[100] = ""; // Textul actualizat
int textLength; // Lungimea textului inclusiv spații

// Definește indicii pentru diferite părți ale textului
int idMealStart, idMealEnd;
int strMealStart, strMealEnd;
int strCategoryStart, strCategoryEnd;  //PE ACESTEA LE CONCATENAM LA AFISARE

//Afisare caracteristici wifi pe serial 

void handleWiFiDetails() {  
     if (WiFi.status() == WL_CONNECTED) {
        int rssi = WiFi.RSSI();
        int rssi_positive = -rssi; // Convertim valoarea negativă în pozitivă
        // Afișează detaliile rețelei WiFi
        Serial.print("Connected to WiFi SSID: ");
        Serial.println(WiFi.SSID());  // Afișează SSID-ul rețelei conectate
        Serial.print("Signal Strength (RSSI): ");
        Serial.print(rssi_positive);  // Afișează RSSI-ul convertit în pozitiv
        Serial.println(" dB");  // Afișează 'dB' în loc de 'dBm'
    } else {
        Serial.println("WiFi not connected");
    }
}

void setup() {
  // Initializează pinii pentru cifre și segmente
  pinMode(DIGIT1, OUTPUT);
  pinMode(DIGIT2, OUTPUT);
  for (uint8_t i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW); // Asigură că toate segmentele sunt inițial dezactivate
  }

  Serial.begin(115200); // Inițializează comunicația serială la 115200 bps

  delay(1000);

// conectare la retea

  WiFi.mode(WIFI_STA); // Setează modul stație pentru WiFi
  WiFi.disconnect(); // Deconectează orice conexiune existentă
  delay(100);
  Serial.println("Setup done");

  Serial.println("Connecting to WiFi...");
  const char* WIFI_SSID = "YOUR_WIFI_NAME";
  const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi!");

  Serial.println("Please enter the idMeal:");
  while (Serial.available() == 0) {
    // Așteaptă intrarea utilizatorului
  }

  String idMeal = Serial.readStringUntil('\n');
  idMeal.trim(); // Elimină orice caracter newline de la sfârșit

  handleRoot(idMeal);
  handleWiFiDetails();  //apelarea functiei 
}

void handleRoot(String idMeal) { 
  HTTPClient http;
  String url = "http://proiectia.bogdanflorea.ro/api/the-meal-db/recipe?idMeal=" + idMeal;
  http.begin(url);

  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      // Citește și analizează răspunsul JSON
      DynamicJsonDocument doc(4096);
      DeserializationError error = deserializeJson(doc, http.getString());
      if (!error) {
        // Extrage câmpurile necesare
        const char* idMeal = doc["idMeal"];
        const char* strMeal = doc["strMeal"];
        const char* strCategory = doc["strCategory"];
        const char* strArea = doc["strArea"];
        const char* strIngredient1 = doc["strIngredient1"];
        const char* strIngredient2 = doc["strIngredient2"];
        const char* strIngredient3 = doc["strIngredient3"];
        // Afișează câmpurile extrase pe monitorul serial
        Serial.println("PIA - Echipa 229 ");
        Serial.print("ID: ");
        Serial.println(idMeal);
        Serial.print("Meal: ");
        Serial.println(strMeal);
        Serial.print("Category: ");
        Serial.println(strCategory);
        Serial.println("Zona geografica: ");
        Serial.println(strArea);

        Serial.println("Reteta: ");
        Serial.println(strIngredient1);
        Serial.println(strIngredient2);
        Serial.println(strIngredient3);

        Serial.println("Tema este retete."); // Text explicativ pt tema
        
        // Concatenează câmpurile într-un singur șir
        char concatenatedText[100]; // Ajustează dimensiunea după necesități
        snprintf(concatenatedText, sizeof(concatenatedText), " %s %s %s ", idMeal, strMeal, strCategory);

        // Copiază șirul concatenat în variabila text
        strncpy(text, concatenatedText, sizeof(text) - 1);
        text[sizeof(text) - 1] = '\0'; // Asigură terminarea cu null

        // Actualizează variabila textLength
        textLength = strlen(text);

        // Actualizează indicii pentru diferite părți ale textului
        idMealStart = 1;
        idMealEnd = idMealStart + strlen(idMeal);
        strMealStart = idMealEnd + 1;
        strMealEnd = strMealStart + strlen(strMeal);
        strCategoryStart = strMealEnd + 1;
        strCategoryEnd = strCategoryStart + strlen(strCategory);

      } else {
        Serial.println("Error parsing JSON: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("HTTP error: ");
      Serial.println(httpCode);
    }
  } else {
    Serial.println("Error connecting to server.");
  }
  
  http.end();
}

void loop() {
  unsigned long currentMillis = millis();

  // Actualizează litera la fiecare interval
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    count++;
    if (count >= textLength - 1) {
      count = 0; // Resetează contorul după ce ajunge la sfârșitul textului
    }
  }

  // Afișează partea curentă a textului
  displayText(count);
}

void displayText(int index) {
  char firstChar = toupper(text[index]);
  char secondChar = toupper(text[index + 1]);

  static unsigned long previousMillis = 0;
  static int digit = 0;

  // Determină câmpul curent pe baza indexului
  int fieldNumber = 0;
  if (index >= idMealStart && index < idMealEnd) {
    fieldNumber = 1;
  } else if (index >= strMealStart && index < strMealEnd) {
    fieldNumber = 2;
  } else if (index >= strCategoryStart && index < strCategoryEnd) {
    fieldNumber = 3;
  }

  if (millis() - previousMillis >= MAX_TIME) {
    previousMillis = millis();
    
    // Dezactivează ambele cifre
    digitalWrite(DIGIT1, LOW);
    digitalWrite(DIGIT2, LOW);

    // Selectează cifra de afișat
    if (digit == 0) {
      if (index + 1 < textLength && secondChar != ' ') {
        if (secondChar >= '0' && secondChar <= '9') {
          displayDigit(numberMap[secondChar - '0']); // Afișează al doilea caracter (număr) pe prima cifră
        } else {
          displayDigit(letterMap[secondChar - 'A']); // Afișează al doilea caracter (literă) pe prima cifră
        }
        Serial.print(fieldNumber); // Afișează pe rânduri diferite numărul câmpului pe serial
      } else {
        clearDigit();
      }
      digitalWrite(DIGIT1, HIGH);
      digit = 1;
    } else {
      if (firstChar != ' ') {
        if (firstChar >= '0' && firstChar <= '9') {
          displayDigit(numberMap[firstChar - '0']); // Afișează primul caracter (număr) pe a doua cifră
        } else {
          displayDigit(letterMap[firstChar - 'A']); // Afișează primul caracter (literă) pe a doua cifră
        }
        Serial.print(fieldNumber); // Afișează pe rânduri diferite numărul câmpului pe serial
      } else {
        clearDigit();
      }
      digitalWrite(DIGIT2, HIGH);
      digit = 0;
    }
  }
}

void displayDigit(uint8_t segments) {
  // Setează segmentele conform hărții de segmente
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], (segments >> (7 - i)) & 0x01);
  }
}

void clearDigit() {
  // Dezactivează toate segmentele
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], LOW);
  }
}