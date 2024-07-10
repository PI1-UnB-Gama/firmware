// #include <SPI.h>
#include <SD.h>

// String msg;

// Informações úteis:
// Para ativar o modo de upload, ative a terceira e quarta chave.
// Para ativar o modo de execução, ative a primeira e segunda chave.

// Definição dos pinos do motor.
// M1 representa o motor da esquerda e M2 o motor da direita.
#define M1 11
#define M2 9

// Definição dos pinos dos sensores.
// S1 representa o sensor da esquerda e S2 o sensor da direita.
#define S1 7
#define S2 6

// Definição do pino do amperímetro.
#define AMP A0

// Representa a velocidade do carrinho ao se mover para frente.
const int velocity = 60;
// Representa a velocidade do carrinho ao se mover para os lados.
const int turn_velocity = 50;
// Representa o tempo de espera para o carrinho. Isso é necessário para
// que o carrinho tenha tempo de virar antes de continuar a se mover para
// frente, caso contrário ele se moverá muito pouco para o lado, perderá o
// contato com a linha e começará a se mover para frente, saindo da curva.
const int default_delay = 450;
// Nome do arquivo CSV que será gravado no cartão SD.
const char filename[] = "data.txt";

// Representa o incremento do ângulo do carrinho ao se mover para os lados.
// Server apenas para mostrar o ângulo atual do carrinho.
const int angle_incrementor = 30;
// Representa o ângulo atual do carrinho.
int angle = 0;
// Define se o carrinho terminou de seguir a linha.
bool finished = false;

// Arquivo de dados que será gravado no cartão SD.
File data;

// volatile int cont = 0;
// int voltas = 0;
// unsigned long time;

//
// Algoritmo para seguir a linha e determinar o ângulo atual do carrinho.
//
void follow_line() {
  bool sensor_left = digitalRead(S1), sensor_right = digitalRead(S2);

  if ((sensor_left == 0) && (sensor_right == 0)) {
    // Serial.println("Nenhuma linha detectada. Seguindo em frente.");

    analogWrite(M1, velocity);
    analogWrite(M2, velocity);
  }

  if ((sensor_left == 1) && (sensor_right == 0)) {
    // Serial.println("Linha detectada à esquerda. Desativando motor esquerdo.");

    analogWrite(M1, 0);
    analogWrite(M2, turn_velocity);

    angle -= angle_incrementor;

    if (angle < 0)
      angle = 360 + angle;
  }

  if ((sensor_left == 0) && (sensor_right == 1)) {
    // Serial.println("Linha detectada à direita. Desativando motor direito.");

    analogWrite(M1, turn_velocity);
    analogWrite(M2, 0);

    angle += angle_incrementor;
  }

  if ((sensor_left == 1) && (sensor_right == 1)) {
    // Serial.println("Linha detectada em ambos os lados. Parando.");

    analogWrite(M1, 0);
    analogWrite(M2, 0);

    finished = true;
    Serial.println("Carrinho terminou de seguir a linha.");
  }

  angle = angle % 360;

  // Serial.print("Ângulo atual: ");
  // Serial.println(angle);
}

//
// Salvar dados no cartão SD.
//
void save_data(unsigned long timestamp, int angle, int amperage) {
  data.print(timestamp);
  data.print(",");
  data.print(angle);
  data.print(",");
  data.print(amperage);
  data.println();

  Serial.print("Dados salvos: ");
  Serial.print(timestamp);
  Serial.print(",");
  Serial.print(angle);
  Serial.print(",");
  Serial.print(amperage);
  Serial.println();
}

void setup() {
  Serial.begin(9600);
//   Serial3.begin(9600);

//   pinMode(pinoCS, OUTPUT);
//   delay(500);

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
//   digitalWrite(dir1, LOW);
//   digitalWrite(dir2, LOW);

//   pinMode(3, INPUT);
//   attachInterrupt(digitalPinToInterrupt(3), interrupcao, RISING);

  if (!SD.begin()) {
    Serial.println("Falha ao iniciar o cartão micro SD.");
    return;
  }

  Serial.println("Cartão micro SD inicializado com sucesso.");

  // Remove o arquivo de dados antigo, caso exista.
  int removed = SD.remove(filename);

  if (removed)
    Serial.println("Arquivo de dados antigo removido com sucesso.");
  else
    Serial.println("Falha ao remover o arquivo de dados antigo (o arquivo possivelmente não existe).");

  // Cria um novo arquivo de dados.
  data = SD.open(filename, FILE_WRITE);

  if (data)
    Serial.println("Arquivo de dados criado com sucesso.");
  else
    Serial.println("Falha ao criar o arquivo de dados.");
}

void loop() {
  if (finished) {
    data.close();
    return;
  }

  //
  // Algoritmo para seguir a linha e obter o ângulo do carrinho.
  //
  follow_line();

  //
  // Obtendo o timestamp atual.
  //
  unsigned long timestamp = millis();

  //
  // Obtendo a leitura do amperímetro em miliamperes.
  //
  int amperage = analogRead(AMP);

  //
  // Salvando dados no cartão SD.
  //
  save_data(timestamp, angle, amperage);

  //
  // Aguardando um tempo para que o carrinho tenha tempo de virar antes de
  // continuar a se mover para frente.
  //
  delay(default_delay);

//   delay(1000);
//   detachInterrupt(digitalPinToInterrupt(3));
//   voltas = (cont * 3);
//   time = millis();
//   Serial.println(voltas);
//   cont = 0;
//   attachInterrupt(digitalPinToInterrupt(3), interrupcao, RISING);

//   if (Serial3.available()) {
//     char data = Serial3.read();
//     Serial.print(cont);
//     Serial.print(voltas);
//     Serial.print(time);
//     msg += data;

//     if (data == 13) {
//       msg = "";
//     }
//     if (msg.indexOf("LED_ON") > 0) {
//       digitalWrite(LED_BUILTIN, HIGH);
//     } else if (msg.indexOf("LED_OFF") > 0) {
//       digitalWrite(LED_BUILTIN, LOW);
//     }
//   }

//   numeroLeitura++;
//   leituraLDR = analogRead(pinoLDR);

//   unsigned long timestamp = millis(); // Obtém o timestamp atual

//   Serial.print("Leitura ");
//   Serial.print(numeroLeitura);
//   Serial.print(": ");
//   Serial.println(leituraLDR);

//   File data = SD.open("arquivo.txt", FILE_WRITE);
//   if (data) {
//     data.print(timestamp);
//     data.print(",");
//     data.print(Sensor1);
//     data.print(",");
//     data.print(Sensor2);
//     data.print(",");
//     data.println(leituraLDR);
//     data.close();
//   } else {
//     Serial.println("Erro ao abrir o arquivo");
//   }

//   delay(5000);
}

// void interrupcao() {
//   cont++;
// }
