#include <LiquidCrystal.h>
#include <Servo.h>

// Pin definitions for the color sensor
#define S0 PA0
#define S1 PA1
#define S2 PA4
#define S3 PB0
#define sensorOut PC1


// Pin definitions for the LCD
const int rs = PB5, en = PB4, d4 = PC7, d5 = PB6, d6 = PA7, d7 = PA6;
const int RST_BUTTON = PC0;  // the number of the pushbutton pin
const int SPLIT_BUTTON = PA8;  // the number of the pushbutton pin

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Servo setup
Servo myservo;
int servoPositionRed = 0;      // position for red
int servoPositionGreen = 60;   // position for green
int servoPositionBlue = 120;   // position for blue
int servoPositionWhite = 180;  // position for white
int servoStartPosition = 90;   // start or default position

const float RED_VALUE = 0.25;
const float WHITE_VALUE = 0.50;
const float BLUE_VALUE = 1.00;
const float GREEN_VALUE = 2.00;

float currentPot = 0.0; // The current pot of money
int redCount = 0;
int blueCount = 0;
int greenCount = 0;
int whiteCount = 0;

void setup() {
  myservo.attach(PB3); // attaches the servo on pin PB3 to the servo object

  // Setup the pins for the color sensor
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(RST_BUTTON, INPUT_PULLUP);
  pinMode(SPLIT_BUTTON, INPUT_PULLUP);
  pinMode(sensorOut, INPUT);

  // Setting frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // Setup serial communication for debugging
  Serial.begin(9600);

  // Setup the LCD's number of columns and rows
  lcd.begin(16, 2);
  lcd.print("Pot: $");
  lcd.print(currentPot, 2); // Print the pot with 2 decimal places
  
  // Move the servo to the start position on setup
  myservo.write(servoStartPosition);
  delay(1000); // wait for 1 second to let the servo reach the position
}

void loop() {  
  int redValue = readColor(LOW, LOW); // Reading Red filter (Red light)
  int greenValue = readColor(HIGH, HIGH); // Reading Green filter (Green light)
  int blueValue = readColor(LOW, HIGH); // Reading Blue filter (Blue light)

  String color = determineColor(redValue, greenValue, blueValue);

  if (color == "Red") {
    myservo.write(servoPositionRed);
    currentPot += RED_VALUE;
    redCount += 1;
  } else if (color == "Green") {
    myservo.write(servoPositionGreen);
    currentPot += GREEN_VALUE;
    greenCount += 1;
  } else if (color == "Blue") {
    myservo.write(servoPositionBlue);
    currentPot += BLUE_VALUE;
    blueCount += 1;
  } else if (color == "White") {
    myservo.write(servoPositionWhite);
    currentPot += WHITE_VALUE;
    whiteCount += 1;
  } else {
    myservo.write(servoStartPosition);
  }
  
    if (digitalRead(RST_BUTTON) == LOW) {
    delay(50); 
    if (digitalRead(RST_BUTTON) == LOW) { 

      currentPot = 0.00;
      redCount = 0;
      blueCount = 0;
      greenCount = 0;
      whiteCount = 0;

      lcd.clear();
      lcd.print("Pot: $");
      lcd.print(currentPot, 2); 
      
      while (digitalRead(RST_BUTTON) == LOW) {

      }
      delay(50); 
    }
  }

  bool splitButtonPressed = false;

  // Check if split button is pressed
  if (digitalRead(SPLIT_BUTTON) == LOW) {
    // Debounce the button
    delay(50);
    if (digitalRead(SPLIT_BUTTON) == LOW) {
      splitButtonPressed = true;
    lcd.clear(); // Clear the display
    lcd.setCursor(0, 0); // Set the cursor to the first column (0) and the first row (0)
    lcd.print("Both get:"); // Print on the first line
    lcd.setCursor(0, 1); // Set the cursor to the first column (0) and the second row (1)
    lcd.print("R:" + String(redCount / 2) + 
              " G:" + String(greenCount / 2) + 
              " B:" + String(blueCount / 2) +
              " W:" + String(whiteCount / 2)); 
    
      while (digitalRead(SPLIT_BUTTON) == LOW) {
      }
      delay(4000); // Delay after the button is released
    }
  }

  if (!splitButtonPressed) {
    lcd.clear();
    lcd.print("Pot: $");
    lcd.print(currentPot, 2); // Print with two decimal places
  }
  delay(750); // Wait for 1 second before next reading
}

// Rest of the functions (readColor, isInRange, determineColor) remain unchanged
int readColor(int s2state, int s3state) {
  digitalWrite(S2, s2state);
  digitalWrite(S3, s3state);
  // The pulseIn function reads a pulse on a pin in a particular state (HIGH or LOW)
  // For the color sensor, we are interested in reading the LOW pulse width, 
  // which is the time the sensor's output pin stays LOW (in microseconds)
  // during a cycle of the sensor's output frequency.
  return pulseIn(sensorOut, LOW);
}

bool isInRange(int value, int target, int tolerance) {
  return value >= (target - tolerance) && value <= (target + tolerance);
}

String determineColor(int red, int green, int blue) {
  // Adjust the tolerance if necessary to be more lenient with the color ranges
  const int tolerance = 20; // for example, a +/- 10 range for each color
  if (isInRange(red, 35, tolerance) && isInRange(green, 120, tolerance) && isInRange(blue, 80, tolerance)) {
    return "Red";
  } else if (isInRange(red, 82, tolerance) && isInRange(green, 52, tolerance) && isInRange(blue, 55, tolerance)) {
    return "Green";
  } else if (isInRange(red, 91, tolerance) && isInRange(green, 74, tolerance) && isInRange(blue, 41, tolerance)) {
    return "Blue";
  } else if (isInRange(red, 19, tolerance) && isInRange(green, 20, tolerance) && isInRange(blue, 15, tolerance)) {
    return "White";
  } else {
    return "Unknown";
  }
}
