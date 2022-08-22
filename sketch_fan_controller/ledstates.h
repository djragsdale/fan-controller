const int LED_PIN_RED = D7;
const int LED_PIN_GREEN = D6;
const int LED_PIN_BLUE = D5;

struct ledColorMap {
  int redPin;
  int greenPin;
  int bluePin;
};

const ledColorMap OFF_LED_COLOR { LOW, LOW, LOW };
const ledColorMap WHITE_LED_COLOR { HIGH, HIGH, HIGH };

const ledColorMap RED_LED_COLOR { HIGH, LOW, LOW };
const ledColorMap GREEN_LED_COLOR { LOW, HIGH, LOW };
const ledColorMap BLUE_LED_COLOR { LOW, LOW, HIGH };

const ledColorMap CYAN_LED_COLOR { LOW, HIGH, HIGH };
const ledColorMap MAGENTA_LED_COLOR { HIGH, LOW, HIGH };
const ledColorMap YELLOW_LED_COLOR { HIGH, HIGH, LOW };

ledColorMap currentLedState = OFF_LED_COLOR;

const String LED_STATE_ERROR = "error";
const String LED_STATE_OFF = "off";
const String LED_STATE_POWER = "power";
const String LED_STATE_RECEIVING = "receiving";
const String LED_STATE_TRANSMITTING = "transmitting";

void setupLeds() {
  pinMode(LED_PIN_RED, OUTPUT);
  pinMode(LED_PIN_GREEN, OUTPUT);
  pinMode(LED_PIN_BLUE, OUTPUT);
}

bool isLedInError() {
  if (currentLedState.redPin != MAGENTA_LED_COLOR.redPin) return false;
  if (currentLedState.greenPin != MAGENTA_LED_COLOR.greenPin) return false;
  if (currentLedState.bluePin != MAGENTA_LED_COLOR.bluePin) return false;
  return true;
}

bool isLedOff() {
  if (currentLedState.redPin != OFF_LED_COLOR.redPin) return false;
  if (currentLedState.greenPin != OFF_LED_COLOR.greenPin) return false;
  if (currentLedState.bluePin != OFF_LED_COLOR.bluePin) return false;
  return true;
}

void setLedState(String state = LED_STATE_OFF) {
  if (state == LED_STATE_ERROR) {
    currentLedState = MAGENTA_LED_COLOR;
  } else if (state == LED_STATE_POWER) {
    currentLedState = RED_LED_COLOR;
  } else if (state == LED_STATE_RECEIVING) {
    currentLedState = GREEN_LED_COLOR;
  } else if (state == LED_STATE_TRANSMITTING) {
    currentLedState = BLUE_LED_COLOR;
  } else {
    currentLedState = OFF_LED_COLOR;
  }

  digitalWrite(LED_PIN_RED, currentLedState.redPin);
  digitalWrite(LED_PIN_GREEN, currentLedState.greenPin);
  digitalWrite(LED_PIN_BLUE, currentLedState.bluePin);
}
