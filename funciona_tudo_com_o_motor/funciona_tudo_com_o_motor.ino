#include <TFT.h>
#include <SPI.h>

#define cs   10
#define dc    8
#define rst   9
#define BUTTON_FORWARD 2
#define BUTTON_BACKWARD 3
#define BUTTON_STOP 7
#define LED_RED 6
#define LED_BLUE 5
#define LED_GREEN 4
#define IN1 22
#define IN2 24
#define IN3 26
#define IN4 28

const int stepSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

int currentStep = 0; // Passo atual do motor

TFT TFTscreen = TFT(cs, dc, rst);

unsigned long lastDebounceTimeForward = 0;
unsigned long lastDebounceTimeBackward = 0;
unsigned long lastDebounceTimeStop = 0;
const unsigned long debounceDelay = 50;

unsigned long lastStepTime = 0; // Tempo do último passo do motor
const unsigned long stepDelay = 1000; // Intervalo entre passos em microssegundos

enum STATES {
  IDLE,
  STOP,
  BLUE_BLINK,
  GREEN_BLINK
};

STATES currentState = IDLE;
STATES previousState = IDLE;

int lastButtonStateForward = LOW;
int lastButtonStateBackward = LOW;
int lastButtonStateStop = LOW;

bool isXDrawn = false;

void setup() {
  pinMode(BUTTON_FORWARD, INPUT);
  pinMode(BUTTON_BACKWARD, INPUT);
  pinMode(BUTTON_STOP, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  turnOffAllLEDs();
  TFTscreen.begin();
  TFTscreen.setRotation(3);
  TFTscreen.background(255, 255, 255);
  displayInitialMessage();
}

void loop() {
  handleButtonPresses();

  // Só atualiza o display e LEDs se o estado mudou
  if (currentState != previousState) {
    updateDisplayAndLEDs();
    previousState = currentState; // Atualiza o estado anterior
  }

  // Controle do motor baseado no estado atual
  if (currentState == GREEN_BLINK) {
    moveMotorForward(); // Motor para frente
  } else if (currentState == BLUE_BLINK) {
    moveMotorBackward(); // Motor para trás
  } else if (currentState == STOP) {
    stopMotor(); // Para o motor
  }

  // Controle dos LEDs piscantes
  if (currentState == GREEN_BLINK) {
    blinkGreen();
  } else if (currentState == BLUE_BLINK) {
    blinkBlue();
  }
}

void handleButtonPresses() {
  int readingForward = digitalRead(BUTTON_FORWARD);
  int readingBackward = digitalRead(BUTTON_BACKWARD);
  int readingStop = digitalRead(BUTTON_STOP);

  // Debounce do botão "Forward"
  if (readingForward != lastButtonStateForward) {
    lastDebounceTimeForward = millis();
  }
  if ((millis() - lastDebounceTimeForward) > debounceDelay && readingForward == HIGH) {
    currentState = GREEN_BLINK;
    isXDrawn = false;
  }
  lastButtonStateForward = readingForward;

  // Debounce do botão "Backward"
  if (readingBackward != lastButtonStateBackward) {
    lastDebounceTimeBackward = millis();
  }
  if ((millis() - lastDebounceTimeBackward) > debounceDelay && readingBackward == HIGH) {
    currentState = BLUE_BLINK;
    isXDrawn = false;
  }
  lastButtonStateBackward = readingBackward;

  // Debounce do botão "Stop"
  if (readingStop != lastButtonStateStop) {
    lastDebounceTimeStop = millis();
  }
  if ((millis() - lastDebounceTimeStop) > debounceDelay && readingStop == HIGH) {
    currentState = STOP;
    isXDrawn = false;
  }
  lastButtonStateStop = readingStop;
}

void updateDisplayAndLEDs() {
  turnOffAllLEDs(); // Garante que todos os LEDs estejam apagados antes de atualizar o novo estado

  switch (currentState) {
    case IDLE:
      TFTscreen.background(255, 255, 255);
      displayInitialMessage();
      break;

    case GREEN_BLINK:
      drawRotatedArrowWithText(true); // Exibe "Forward"
      break;

    case BLUE_BLINK:
      drawRotatedArrowWithText(false); // Exibe "Backward"
      break;

    case STOP:
      drawStopX(); // Exibe o "X"
      break;
  }
}

void blinkBlue() {
  static unsigned long lastBlinkTime = 0;
  static bool ledState = LOW;

  if (millis() - lastBlinkTime >= 500) {
    lastBlinkTime = millis();
    ledState = !ledState;
    digitalWrite(LED_RED, ledState); // Pisca o LED azul
  }
}

void blinkGreen() {
  static unsigned long lastBlinkTime = 0;
  static bool ledState = LOW;

  if (millis() - lastBlinkTime >= 500) {
    lastBlinkTime = millis();
    ledState = !ledState;
    digitalWrite(LED_GREEN, ledState); // Pisca o LED verde
  }
}

void turnOffAllLEDs() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);
}

void displayInitialMessage() {
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.setTextSize(1);
  TFTscreen.text("Clique num botao", 10, 30);
  TFTscreen.text("para movimentar", 10, 50);
  TFTscreen.text("o guincho", 10, 70);
}

void drawStopX() {
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(0, 0, 255);

  for (int i = -1; i <= 1; i++) {
    TFTscreen.line(40 + i, 40, 80 + i, 80);
    TFTscreen.line(40 + i, 80, 80 + i, 40);
  }
  digitalWrite(LED_BLUE, HIGH);
}

void drawRotatedArrowWithText(bool forward) {
  if (forward) {
    TFTscreen.background(0, 0, 0);
    TFTscreen.stroke(0, 255, 0);

    for (int i = -1; i <= 1; i++) {
      TFTscreen.line(60 + i, 40, 60 + i, 80);
      TFTscreen.line(60 + i, 40, 40 + i, 60);
      TFTscreen.line(60 + i, 40, 80 + i, 60);
    }
    TFTscreen.setTextSize(2);
    TFTscreen.stroke(0, 255, 0);
    TFTscreen.text("Forward", 25, 90);
  } else {
    TFTscreen.background(0, 0, 0);
    TFTscreen.stroke(255, 0, 0);

    for (int i = -1; i <= 1; i++) {
      TFTscreen.line(60 + i, 80, 60 + i, 40);
      TFTscreen.line(60 + i, 80, 40 + i, 60);
      TFTscreen.line(60 + i, 80, 80 + i, 60);
    }
    TFTscreen.setTextSize(2);
    TFTscreen.stroke(255, 0, 0);
    TFTscreen.text("Backward", 20, 90);
  }
}

void moveMotorForward() {
  if (micros() - lastStepTime >= stepDelay) {
    lastStepTime = micros();
    currentStep = (currentStep + 1) % 8;
    setMotorStep(currentStep);
  }
}

void moveMotorBackward() {
  if (micros() - lastStepTime >= stepDelay) {
    lastStepTime = micros();
    currentStep = (currentStep - 1 + 8) % 8;
    setMotorStep(currentStep);
  }
}

void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void setMotorStep(int step) {
  digitalWrite(IN1, stepSequence[step][0]);
  digitalWrite(IN2, stepSequence[step][1]);
  digitalWrite(IN3, stepSequence[step][2]);
  digitalWrite(IN4, stepSequence[step][3]);
}
