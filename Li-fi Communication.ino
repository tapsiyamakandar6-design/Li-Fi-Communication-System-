#include <LiquidCrystal.h>

// --- Your LCD and LDR Pins ---
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
#define ldr 8

// --- Morse Code Timing (in milliseconds) ---
int DOT_TIME = 200; // MATCHES YOUR APP!

// --- Tolerances (to allow for sloppy sending) ---
int dot_len = DOT_TIME;                     // 200ms
int dash_len = DOT_TIME * 3;                // 600ms
int letter_gap = DOT_TIME * 3;              // 600ms
int word_gap = DOT_TIME * 7;                // 1400ms
int end_of_transmission_gap = 2500;         // 2.5 sec

// --- Global variables to build the message ---
String currentLetter = "";
String currentMessage = "";
int lastState = 0; // 0 = light, 1 = dark
unsigned long lastChangeTime = 0;

// --- Function declaration ---
String decodeMorse(String morseLetter);

void setup() {
  Serial.begin(9600);
  pinMode(ldr, INPUT);
  
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Morse Decoder");
  lcd.setCursor(0, 1);
  lcd.print("Ready...");
  delay(3000);
  lcd.clear();
  lastChangeTime = millis(); // Start the timer
}

void loop() {
  int currentState = digitalRead(ldr); // 0 = light, 1 = dark
  unsigned long now = millis();

  // --- 1. Check if the light state has changed ---
  if (currentState != lastState) {
    unsigned long duration = now - lastChangeTime;

    // --- A. It was LIGHT, now it's DARK ---
    if (lastState == 0) { 
      // It was a light pulse, check if it was a dot or dash
      if (duration > (dot_len - 50) && duration < (dot_len + 50)) {
        currentLetter += ".";
      } 
      else if (duration > (dash_len - 100) && duration < (dash_len + 100)) {
        currentLetter += "-";
      }
    } 
    
    // --- B. It was DARK, now it's LIGHT ---
    else { 
      // It was a dark pause, check if it's a letter or word gap
      if (duration > (letter_gap - 100) && duration < (letter_gap + 100)) {
        // End of a letter. Decode it and add to the message.
        currentMessage += decodeMorse(currentLetter);
        currentLetter = "";
      } 
      else if (duration > (word_gap - 200) && duration < (word_gap + 200)) {
        // End of a word. Decode last letter, then add a space.
        currentMessage += decodeMorse(currentLetter);
        currentMessage += " ";
        currentLetter = "";
      }
    }
    
    // Update the state and timer
    lastState = currentState;
    lastChangeTime = now;
  }

  // --- 2. Check for "End of Transmission" ---
  // If it's dark and it has been dark for a long time
  if (currentState == 1 && (now - lastChangeTime > end_of_transmission_gap)) {
    
    // Check if there's a final letter to decode
    if (currentLetter.length() > 0) {
      currentMessage += decodeMorse(currentLetter);
      currentLetter = "";
    }

    // Only print if we have a message
    if (currentMessage.length() > 0) {
      lcd.clear();
      lcd.print("Received:");
      lcd.setCursor(0, 1);
      
      // Handle scrolling for long messages
      if (currentMessage.length() > 16) {
        lcd.print(currentMessage.substring(0, 16));
        delay(2000);
        
        for (int i = 0; i < currentMessage.length() - 15; i++) {
          lcd.clear();
          lcd.print(currentMessage.substring(i, i + 16));
          delay(300);
        }
      } 
      else {
        lcd.print(currentMessage);
      }

      // Reset for the next full transmission
      currentMessage = "";
      delay(5000); // Show message for 5 seconds
      lcd.clear();
      lcd.print("Ready...");
    }

    // Reset the timer to prevent this from looping
    lastChangeTime = now; 
  }
}

// --- Morse Code Dictionary ---
String decodeMorse(String morseLetter) {
  if (morseLetter == ".-") return "A";
  if (morseLetter == "-...") return "B";
  if (morseLetter == "-.-.") return "C";
  if (morseLetter == "-..") return "D";
  if (morseLetter == ".") return "E";
  if (morseLetter == "..-.") return "F";
  if (morseLetter == "--.") return "G";
  if (morseLetter == "....") return "H";
  if (morseLetter == "..") return "I";
  if (morseLetter == ".---") return "J";
  if (morseLetter == "-.-") return "K";
  if (morseLetter == ".-..") return "L";
  if (morseLetter == "--") return "M";
  if (morseLetter == "-.") return "N";
  if (morseLetter == "---") return "O";
  if (morseLetter == ".--.") return "P";
  if (morseLetter == "--.-") return "Q";
  if (morseLetter == ".-.") return "R";
  if (morseLetter == "...") return "S";
  if (morseLetter == "-") return "T";
  if (morseLetter == "..-") return "U";
  if (morseLetter == "...-") return "V";
  if (morseLetter == ".--") return "W";
  if (morseLetter == "-..-") return "X";
  if (morseLetter == "-.--") return "Y";
  if (morseLetter == "--..") return "Z";
  if (morseLetter == ".----") return "1";
  if (morseLetter == "..---") return "2";
  if (morseLetter == "...--") return "3";
  if (morseLetter == "....-") return "4";
  if (morseLetter == ".....") return "5";
  if (morseLetter == "-....") return "6";
  if (morseLetter == "--...") return "7";
  if (morseLetter == "---..") return "8";
  if (morseLetter == "----.") return "9";
  if (morseLetter == "-----") return "0";

  return "?"; // Return '?' if not found
}
