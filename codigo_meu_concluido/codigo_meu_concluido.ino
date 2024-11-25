#include <TFT.h>  // Biblioteca do LCD TFT
#include <SPI.h>  // Biblioteca SPI

// Definição dos pinos para o Arduino Uno
#define cs   10
#define dc    8
#define rst   9
#define BUTTON_FORWARD 2
#define BUTTON_BACKWARD 3
#define LED_RED 6
#define LED_BLUE 5
#define LED_GREEN 4

TFT TFTscreen = TFT(cs, dc, rst); // Inicializa o objeto da tela

// Variáveis globais para o debounce
unsigned long lastDebounceTimeForward = 0;
unsigned long lastDebounceTimeBackward = 0;
const unsigned long debounceDelay = 50; // 50ms de debounce

// Estados
enum STATES {
  IDLE,
  BLUE_BLINK,
  RED_BLINK
};

STATES currentState = IDLE;

// Variáveis para monitorar o estado dos botões
int lastButtonStateForward = LOW;
int lastButtonStateBackward = LOW;

void setup() {
  // Configuração dos pinos
  pinMode(BUTTON_FORWARD, INPUT);
  pinMode(BUTTON_BACKWARD, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  // Inicializa os LEDs apagados
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);

  // Inicializa a tela TFT
  TFTscreen.begin();
  TFTscreen.setRotation(3); // Rotaciona a tela 90 graus para a esquerda
  TFTscreen.background(255, 255, 255); // Fundo branco
  displayInitialMessage(); // Exibe a mensagem inicial
}

void loop() {
  // Atualiza os estados dos botões
  int readingForward = digitalRead(BUTTON_FORWARD);
  int readingBackward = digitalRead(BUTTON_BACKWARD);

  // Tratamento de debounce para o botão forward
  if (readingForward != lastButtonStateForward) {
    lastDebounceTimeForward = millis();
  }
  if ((millis() - lastDebounceTimeForward) > debounceDelay) {
    if (readingForward == HIGH) {
      // Desliga todos os LEDs antes de começar a piscar
      turnOffAllLEDs();
      currentState = BLUE_BLINK; // Muda para estado de piscar azul
      drawRotatedArrowWithText(0, 0, 255, false); // Seta azul com texto "Backward"
    }
  }
  lastButtonStateForward = readingForward;

  // Tratamento de debounce para o botão backward
  if (readingBackward != lastButtonStateBackward) {
    lastDebounceTimeBackward = millis();
  }
  if ((millis() - lastDebounceTimeBackward) > debounceDelay) {
    if (readingBackward == HIGH) {
      // Desliga todos os LEDs antes de começar a piscar
      turnOffAllLEDs();
      currentState = RED_BLINK; // Muda para estado de piscar vermelho
      drawRotatedArrowWithText(255, 0, 0, true); // Seta vermelha com texto "Forward"
    }
  }
  lastButtonStateBackward = readingBackward;

  // Sistema de estados
  switch (currentState) {
    case IDLE:
      // Estado inicial: LEDs apagados
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_BLUE, LOW);
      break;

    case BLUE_BLINK:
      // Piscar LED azul continuamente
      blinkLED(LED_BLUE);
      break;

    case RED_BLINK:
      // Piscar LED vermelho continuamente
      blinkLED(LED_RED);
      break;

    default:
      currentState = IDLE;
      break;
  }
}

// Função para piscar um LED
void blinkLED(int ledPin) {
  static unsigned long lastBlinkTime = 0;
  static bool ledState = LOW;

  if (millis() - lastBlinkTime >= 500) { // Alterna o estado do LED a cada 500ms
    lastBlinkTime = millis();
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}

// Função para desligar todos os LEDs
void turnOffAllLEDs() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);
}

// Função para exibir a mensagem inicial
void displayInitialMessage() {
  TFTscreen.stroke(0, 0, 0); // Cor do texto: preto
  TFTscreen.setTextSize(1); // Tamanho do texto reduzido
  TFTscreen.text("Clique num botao", 10, 30); // Texto na posição (10, 30)
  TFTscreen.text("para movimentar", 10, 50); // Texto na posição (10, 50)
  TFTscreen.text("o guincho", 10, 70); // Texto na posição (10, 70)
}

// Função para desenhar uma seta com texto centralizado
void drawRotatedArrowWithText(int r, int g, int b, bool forward) {
  // Configura o fundo como preto
  TFTscreen.background(0, 0, 0);

  // Configura a cor da seta
  TFTscreen.stroke(r, g, b);

  // Desenha a seta e o texto
  if (forward) {
    // Seta vermelha rotacionada 180 graus para a esquerda
    for (int i = -1; i <= 1; i++) {
      TFTscreen.line(60 + i, 80, 60 + i, 40);
      TFTscreen.line(60 + i, 80, 40 + i, 60);
      TFTscreen.line(60 + i, 80, 80 + i, 60);
    }
    // Texto "Forward" centralizado
    TFTscreen.setTextSize(2); // Ajustar tamanho do texto
    TFTscreen.stroke(r, g, b); // Mesma cor da seta
    TFTscreen.text("Backward", 20, 90); // Ajustado mais para a esquerda
  } else {
    // Seta azul rotacionada 180 graus para a direita
    for (int i = -1; i <= 1; i++) {
      TFTscreen.line(60 + i, 40, 60 + i, 80);
      TFTscreen.line(60 + i, 40, 40 + i, 60);
      TFTscreen.line(60 + i, 40, 80 + i, 60);
    }
    // Texto "Backward" centralizado
    TFTscreen.setTextSize(2); // Ajustar tamanho do texto
    TFTscreen.stroke(r, g, b); // Mesma cor da seta
    TFTscreen.text("Forward", 25, 90); // Mantido na mesma posição
  }
}


