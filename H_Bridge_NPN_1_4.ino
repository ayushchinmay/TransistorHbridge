/**
*	PROGRAM: CAPACITIVE MUSCLE CONTROL
*	INFO	       : Control an H-Bridge using cascading digital gates
*	AUTHOR       : AYUSH CHINMAY
*	DATE CREATED : 2 JULY 2023
* MODIFIED: 20 July 2023
*
*	CHANGELOG: Use NPN Transistor (N2222) to drive LED's using an external power supply
*           -   Replaced Momentary Pushbutton with Flip-Switch for POWER_BTN
*           -   Modified POWER_BTN Read code to support the Flip-Switch
*/

// BUTTON PINS
const int POWER_BTN = 2;
const int POLAR_BTN = 3;

// INDICATOR PINS
const int POWER_LED = 4;
const int DRAIN_LED = 5;
const int P_HIGH_LED = 6;
const int P_LOW_LED = 7;

// SWITCH PINS
const int P_LOW = 8;    // Polarize: LOW
const int P_HIGH = 9;   // Polarize: HIGH
const int GROUND = 10;  // Ground

// SYSTEM CONSTANTS
bool power = LOW;
bool polarity = LOW;
// STATE CONSTANTS
int stateCount = 0;  //0:GND, 1:+ve, 2:GND, 3:-ve

// BUTTON DEBOUNCE VARIABLES
bool powerBtnState = LOW;
bool polarBtnState = LOW;
bool polarBtnLastState = LOW;
unsigned long polarBtnLastDebounceTime = 0;
unsigned long debounceDelay = 50;

/// FOR MOMENTARY PUSH-BUTTON
// bool powerBtnLastState = LOW;
// unsigned long powerBtnLastDebounceTime = 0;

  // if (debounceRead(powerRead, &powerBtnLastState, &powerBtnState, &powerBtnLastDebounceTime, debounceDelay)) {
  //   if (powerBtnState == HIGH) {
  //     Serial.print("POWER BUTTON PRESSED: ");
  //     power = !power;

  //     if (power == HIGH) {
  //       Serial.println("POWER ON");
  //       stateCount = 0; // myCode
  //       digitalWrite(POWER_LED, HIGH);
  //     } else {
  //       Serial.println("POWER OFF");
  //       Ground();
  //       digitalWrite(POWER_LED, LOW);
  //     }
  //   }
  // }

/*===========================*
*==========[ SETUP ]=========*
*============================*/
void setup() {
  // Initialize Serial
  Serial.begin(115200);

  // Configure Buttons
  pinMode(POWER_BTN, INPUT_PULLUP);
  pinMode(POLAR_BTN, INPUT);

  // Configure Indicators
  pinMode(POWER_LED, OUTPUT);
  pinMode(DRAIN_LED, OUTPUT);
  pinMode(P_HIGH_LED, OUTPUT);
  pinMode(P_LOW_LED, OUTPUT);

  // Configure Switches
  pinMode(P_HIGH, OUTPUT);
  pinMode(P_LOW, OUTPUT);
  pinMode(GROUND, OUTPUT);

  // Start the Program with Device GROUNDED
  Ground();

  // Check if Power is Already ON
  PowerRead();
}


/*===========================*
*==========[ LOOP ]==========*
*============================*/
void loop() {
  /// Read POWER_BTN State
  int powerRead = digitalRead(POWER_BTN);
  if (powerRead != powerBtnState) {
    PowerRead();
  }

  /// If Power ON, then Read POLAR_BTN State
  if (power == HIGH) {
    int polarRead = digitalRead(POLAR_BTN);

    if (DebounceRead(polarRead, &polarBtnLastState, &polarBtnState, &polarBtnLastDebounceTime, debounceDelay)) {
      if (polarBtnState == HIGH) {  // Cycle through the Polarity State --> 0:GND, 1:+ve, 2:GND, 3:-vec
      stateCount++;
      StateSelect();
      }
    }
  }

  delay(10);  // Delay a little bit to improve simulation performance
}


/*===========================*
*========[ FUNCTIONS ]=======*
*============================*/

void PowerRead() {
  int pwrRead = digitalRead(POWER_BTN);

  if (pwrRead == LOW) {
	stateCount = 0;
	power = HIGH;
	digitalWrite(POWER_LED, HIGH);
	Serial.println("POWER STATUS: POWER ON");
  } else {
	Ground();
	power = LOW;
	digitalWrite(POWER_LED, LOW);
	Serial.println("POWER STATUS: POWER OFF");
  }

  powerBtnState = pwrRead;
}


// Helper function to perform button debounce
bool DebounceRead(int buttonState, bool* lastButtonState, bool* currentButtonState, unsigned long* lastDebounceTime, unsigned long debounceDelay) {
  if (buttonState != *lastButtonState) {
	*lastDebounceTime = millis();
  }

  if ((millis() - *lastDebounceTime) > debounceDelay) {
	if (buttonState != *currentButtonState) {
	  *currentButtonState = buttonState;
	  return true;
	}
  }

  *lastButtonState = buttonState;
  return false;
}


// Disconnect the Device, and Drain it to GROUND
void Ground() {
  Serial.println("DEVICE GROUNDED");
  // Update Inndicator LED
  digitalWrite(P_HIGH_LED, LOW);
  digitalWrite(P_LOW_LED, LOW);
  digitalWrite(DRAIN_LED, HIGH);

  // Update OC Switches
  digitalWrite(P_HIGH, LOW);
  digitalWrite(P_LOW, LOW);
  digitalWrite(GROUND, HIGH);
}


// Flip Polarity for Device based on a variable
void FlipPolarity() {
  Serial.print("FLIPPING POLARITY: ");
  digitalWrite(GROUND, LOW);
  digitalWrite(DRAIN_LED, LOW);

  if (polarity == HIGH) {
	Serial.println("HIGH");
	digitalWrite(P_HIGH_LED, HIGH);
	digitalWrite(P_HIGH, HIGH);
  } else {
	Serial.println("LOW");
	digitalWrite(P_LOW_LED, HIGH);
	digitalWrite(P_LOW, HIGH);
  }
  polarity = !polarity;
}


// State Control based on stateCount
void StateSelect() {
  switch (stateCount % 2) {
	case 0:
	  Ground();
	  break;
	case 1:
	  FlipPolarity();
	  break;
  }
}