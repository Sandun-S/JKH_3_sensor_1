#include <Arduino.h>

// --- Pin Definitions ---
const int pinTop = 0;   // PB0 - Top Sensor
const int pinMid = 1;   // PB1 - Middle Sensor
const int pinBot = 2;   // PB2 - Bottom Sensor
const int pinPLC = 3;   // PB3 - Output to NPN Transistor Base
const int pinLED = 4;   // PB4 - Status LED (Optional, on your PCB)

// --- Debounce Settings ---
const unsigned long debounceDelay = 15; // 15ms filter for factory noise
// Default state is HIGH because of your physical 10k pull-up resistors
bool sensorState[3] = {HIGH, HIGH, HIGH}; 
bool lastReading[3] = {HIGH, HIGH, HIGH};
unsigned long lastDebounceTime[3] = {0, 0, 0};

// --- Logic Flags (Memory for which sensors saw a bottle) ---
bool flagTop = false;
bool flagMid = false;
bool flagBot = false;

// --- Pulse Settings ---
const unsigned long pulseDuration = 50; // 50ms pulse for the PLC
unsigned long pulseStartTime = 0;
bool isPulseActive = false;

void setup() {
  // Enable internal pull-ups as a backup to your physical 10k resistors
  pinMode(pinTop, INPUT_PULLUP);
  pinMode(pinMid, INPUT_PULLUP);
  pinMode(pinBot, INPUT_PULLUP);
  
  pinMode(pinPLC, OUTPUT);
  pinMode(pinLED, OUTPUT);
  
  // Start with Transistor OFF (PLC line floats to 24V)
  digitalWrite(pinPLC, LOW);
  digitalWrite(pinLED, LOW);
}

// Function to safely read and debounce a sensor
bool readSensor(int pin, int index) {
  bool reading = digitalRead(pin);
  
  if (reading != lastReading[index]) {
    lastDebounceTime[index] = millis(); // Reset timer if noise detected
  }
  
  if ((millis() - lastDebounceTime[index]) > debounceDelay) {
    if (reading != sensorState[index]) {
      sensorState[index] = reading;
      return true; // State genuinely changed
    }
  }
  lastReading[index] = reading;
  return false; 
}

void loop() {
  // 1. Handle PLC Pulse Output (Non-Blocking Timer)
  if (isPulseActive) {
    if (millis() - pulseStartTime >= pulseDuration) {
      // Turn OFF Transistor (PLC line goes back to floating/24V)
      digitalWrite(pinPLC, LOW);
      digitalWrite(pinLED, LOW);
      isPulseActive = false;
    }
  }

  // 2. Read Sensors (Returns true only on the exact millisecond the state changes)
  bool changedTop = readSensor(pinTop, 0);
  bool changedMid = readSensor(pinMid, 1);
  bool changedBot = readSensor(pinBot, 2);

  // 3. Set Flags on Bottle ARRIVAL 
  // (Sensor connects to GND -> ATtiny reads LOW)
  if (changedTop && sensorState[0] == LOW) flagTop = true;
  if (changedMid && sensorState[1] == LOW) flagMid = true;
  if (changedBot && sensorState[2] == LOW) flagBot = true;

  // 4. Evaluate Output on Bottle DEPARTURE (The Neck/Gap)
  // (Sensor floats -> Pull-up resistor kicks in -> ATtiny reads HIGH)
  bool triggerPulse = false;

  // Highest priority: Big Bottle Neck
  if (changedTop && sensorState[0] == HIGH) {
    if (flagTop) triggerPulse = true;
  }
  // Medium priority: Medium Bottle Neck (Ignored if Top flag is active)
  else if (changedMid && sensorState[1] == HIGH) {
    if (flagMid && !flagTop) triggerPulse = true;
  }
  // Lowest priority: Small Bottle Neck (Ignored if Top or Mid flags are active)
  else if (changedBot && sensorState[2] == HIGH) {
    if (flagBot && !flagTop && !flagMid) triggerPulse = true;
  }

  // 5. Fire the Pulse and Reset Flags
  if (triggerPulse) {
    // Turn ON Transistor (Sinks PLC line to GND)
    digitalWrite(pinPLC, HIGH);
    digitalWrite(pinLED, HIGH); 
    pulseStartTime = millis();
    isPulseActive = true;
    
    // Clear all memory flags. This stops the middle/bottom sensors from 
    // accidentally counting the fat bottom of a big bottle as it leaves.
    flagTop = false;
    flagMid = false;
    flagBot = false;
  }
}