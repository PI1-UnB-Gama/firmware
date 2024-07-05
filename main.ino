#include <SD.h>
#include <SPI.h>

//Definição dos pinos de controle do motor
#define M1 9 // Pino_Velocidade 1º Motor ( 0 a 255)_ Porta IN2 ponte H; 9
#define M2 11 //Pino_Velocidade 2º Motor ( 0 a 255) _ Porta IN4 ponte H;11
#define dir1 8 //Pino_Direção do 1º Motor: Para frente / Para trás (HIGH ou LOW)_ porta IN1 ponte H; 8
#define dir2 10 //Pino_Direção do 2º Motor: Para frente / Para trás (HIGH ou LOW)_ porta IN3 ponte H;10
//Definição dos pinos dos sensores
#define pin_S1 7
#define pin_S2 6

// --- Mapeamento de Hardware ---
#define   vcc      5
#define   gnd      4
#define   pino_D0  3
#define   analog   2

bool Sensor1 = 0;
bool Sensor2 = 0;
File myFile;

int pinoSS = 53; // Pin 53 para Mega / Pin 10 para UNO

float voltage;

//variável responsável por controlar a velocidade dos motores
int velocidade = 80;

volatile int cont = 0;
int voltas = 0;
int time = millis();

// ========================================================================================================
// --- Variáveis Globais ---
int           rpm;
volatile byte pulsos;
unsigned long timeold;

//Altere o numero abaixo de acordo com o seu disco encoder
unsigned int pulsos_por_volta = 20;


// ========================================================================================================
// --- Interrupção ---
void contador()
{
  //Incrementa contador
  pulsos++;
}

void setup(){
//Setamos os pinos de controle dos motores como saída
pinMode(M1, OUTPUT);
pinMode(M2, OUTPUT);
pinMode(dir1, OUTPUT);
pinMode(dir2, OUTPUT);
//Setamos a direção inicial do motor como 0, isso fará com que ambos os motores girem para frente
digitalWrite(dir1, LOW);
digitalWrite(dir2, LOW);
//Setamos os pinos dos sensores como entrada
pinMode(pin_S1, INPUT);
pinMode(pin_S2, INPUT);

  Serial.begin(9600);

  pinMode(vcc,    OUTPUT);
  pinMode(gnd,    OUTPUT);
  pinMode(pino_D0, INPUT);
  pinMode(analog,  INPUT);

  digitalWrite(vcc, HIGH);
  digitalWrite(gnd,  LOW);

  //Interrupcao 1 - pino digital 3
  //Aciona o contador a cada pulso
  attachInterrupt(1, contador, FALLING);
  pulsos = 0;
  rpm = 0;
  timeold = 0;



// put your setup code here, to run once:
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), interrupcao, RISING);
  Serial.begin(9600);
  Serial.println(" - Inicio - ");

// Executado uma vez quando ligado o Arduino

Serial.begin(9600); // Define BaundRate
pinMode(pinoSS, OUTPUT); // Declara pinoSS como saída

if (SD.begin()) { // Inicializa o SD Card
Serial.println("SD Card pronto para uso."); // Imprime na tela
}

else {
Serial.println("Falha na inicialização do SD Card.");
return;
}

myFile = SD.open("dados.csv", FILE_WRITE); // Cria / Abre arquivo .txt

if (myFile) { // Se o Arquivo abrir imprime:
Serial.println("Escrevendo no Arquivo .csv"); // Imprime na tela
myFile.println("Usinainfo 1, 2 ,3 ..."); // Escreve no Arquivo
myFile.println(voltas);
myFile.println("/n /n");
myFile.println(cont);
myFile.println("/n /n");
myFile.println(time);

myFile.close(); // Fecha o Arquivo após escrever
Serial.println("Terminado."); // Imprime na tela
}

else {     // Se o Arquivo não abrir
Serial.println("Erro ao Abrir Arquivo .csv"); // Imprime na tela
}

myFile = SD.open("Dados.csv"); // Abre o Arquivo

if (myFile) {
Serial.println("Conteúdo do Arquivo:"); // Imprime na tela

while (myFile.available()) { // Exibe o conteúdo do Arquivo
Serial.write(myFile.read());
}

myFile.close(); // Fecha o Arquivo após ler
}

else {
Serial.println("Erro ao Abrir Arquivo .csv"); // Imprime na tela
}
}



void loop(){
//Neste processo armazenamos o valor lido pelo sensor na variável que armazena tais dados.
Sensor1 = digitalRead(pin_S1);
Sensor2 = digitalRead(pin_S2);
//Aqui está toda a lógica de comportamento do robô: Para a cor branca atribuímos o valor 0 e, para a cor preta, o valor 1.
if((Sensor1 == 0) && (Sensor2 == 0)){ // Se detectar na extremidade das faixas duas cores brancas
analogWrite(M1, velocidade); // Ambos motores ligam na mesma velocidade
analogWrite(M2, velocidade);
}
if((Sensor1 == 1) && (Sensor2 == 0)){ // Se detectar um lado preto e o outro branco
analogWrite(M2, 0); // O motor 1 desliga
analogWrite(M1, velocidade); // O motor 2 fica ligado, fazendo assim o carrinho virar
}
if((Sensor1 == 0) && (Sensor2 == 1)){ // Se detectar um lado branco e o outro preto
analogWrite(M2, velocidade); //O motor 1 fica ligado
analogWrite(M1, 0); // O motor 2 desliga, fazendo assim o carrinho virar no outro sentido
}

//Atualiza contador a cada segundo
  if (millis() - timeold >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(1);
    rpm = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold) * pulsos;
    timeold = millis();
    pulsos = 0;

    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM = ");
    Serial.println(rpm, DEC);
    //Habilita interrupcao
    attachInterrupt(1, contador, FALLING);
  }
  unsigned int x=0;
  float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;
  for (int x = 0; x < 10; x++)          //Get 10 samples
  {
    AcsValue = analogRead(A0);           //Read current sensor values
    Samples = Samples + AcsValue;        //Add samples together
    delay (3);                           // let ADC settle before next sample 3ms
  }
  AvgAcs=Samples/10.0;                   //Taking Average of Samples
  voltage=AvgAcs*(5.0 / 1024.0);         //((AvgAcs * (5.0 / 1024.0)) is converitng the read voltage in 0-5 volts
  Serial.print("Raw Voltage:");
  Serial.print(voltage);
  AcsValueF = (2.5 - voltage)*1000/0.185; //2.5 is offset,,,   0.185v is rise in output voltage when 1A current flows at input
  Serial.print("\t");
  Serial.print("Motor Current :");
  Serial.print(AcsValueF);               //Print the read current on Serial monitor
  Serial.println(" mA");

}

void interrupcao(){
  cont++;
}
