#include <Wire.h>                 //Bibioteca para lidar com a comunicação I2C utilizada pelo display Oled
#include <Adafruit_GFX.h>         //Biblioteca da Adafuit para manipular imagens e vetores para o display
#include <Adafruit_SSD1306.h>     //Biblioteca da Adafuit com funçoes para trabalhar com o display Oled
#include "RTClib.h"               //Biblioteca para trabalhar com RTC
#include <PubSubClient.h>         //Biblioteca para trabalhar com envio de dados usando servidor MQTT
#include <WiFi.h>                 //Biblioteca para conexão com wifi
#include <ESPAsyncWebServer.h>    //Biblioteca para trabalhar com requisiçoes assincronas de um web service
#include <Flash.h>                //Biblioteca que encapsula metodos para gravar e ler dados da memoria não volatil (Flash) do microcontrolador
#include "EmonLib.h"              //Biblioteca para realizar calculo de tensao de pico e RMS mediante a uma senoide


WiFiClient espClient;                             //Instanciando a biblioteca de conexao com wifi
PubSubClient client(espClient);                   //Instanciando a biblioteca de envio de dados via mqtt
AsyncWebServer server(80);                        //Instanciando a biblioteca que permite requisiçoes assincronas atravez do webservice que ira ser gerado
RTC_DS3231 rtc;                                   // Instanciando a biblioteca para trabalhar com o rtc
Adafruit_SSD1306 display(128, 64, &Wire, -1);     //Instanciando biblioteca para trabalhar com o display
EnergyMonitor emon1;                              //Instanciando biblioteca para realizar calculos utilizando os dados dos sensores de corrente e tensão
Flash flash;                                      //Instanciando biblioteca trabalhar com arquivos na flash
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;  //Instancia de um semaforo implementado pela ESPIDF para controlar variaveis dentro de interrupçoes


int btn = 27;     //Pino do botao de interrupçao
int zmtp = 39;    //Pino do sensor de tensao zmtp101b
int acs = 36;     //Pino do sensor de corrente acs
int rele = 33;    //Pino do relê de segurança
int ledOn = 23;   //Pino led on/off
int buzzer = 13;  //Pino buzzer piezoeletrico
float ultCorrenteLida = 0; //Variavel globlal que salva a ultima corrente lida na funçao getCorrente
float ultTensaoLida = 0;  //Variavel globlal que salva a ultima Tensao lida na funçao getTensao
float CompTensao = 1;     //Variavel globlal que contem a variavel que compensa o valor da tensao
float CompCorrente = 1;   //Variavel globlal que contem a variavel que compensa o valor da corrente
float tensaoSegura = 0;   //Variavel globlal que contem a tensao onde o rele deve desarmar
float correnteSegura = 0; //Variavel globlal que contem a corrente onde o rele deve desarmar
String ssidAp;        //Variavel globlal que contem o SSID do ponto de acesso da IHM
String senhaAp;       //Variavel globlal que contem a senha do ponto de acesso da IHM
String ssidWifi;      //Variavel globlal que contem o SSID da rede wifi a ser acessada
String senhaWifi;     //Variavel globlal que contem a senha da rede wifi a ser acessada
String servidorMQTT;  //Variavel globlal que contem o endereço do servidor mqtt (test.mosquito)
String portaMQTT;     //Variavel global que contem a porta do web service do servidor mqtt
String loginMQTT;     //Variavel global que contem o login do servidor mqtt
String senhaMQTT;      //Variavel global que contem a senha do servidor mqtt
String CalibraTensao;   //Valor da tensao lida em um multimetro usado como base para calibrar a variavel de compensaçao da tensao
String CalibraCorrente; //Valor da corrente lida em um multimetro usado como base para calibrar a variavel de compensaçao da corrente
String tensaoDesarme;   //Variavel globlal que contem a tensao onde o rele deve desarmar
String correnteDesarme; //Variavel globlal que contem a corrente onde o rele deve desarmar
bool newData = false;   //Variavel que indica que dados foram recebidos do web service assincrono, e devem ser salvos na flash não volatil
bool conexaoDisponivel = false;  //Indica se existe conexao com a internet para poder enviar os dados via mqtt
bool stateDisplay = true;   //Variavel que indica o estado atual do display (ligado / desligado)
volatile bool lastStateDisplay = true; //variavel que contem o último estado do botão quando ocorreu a interrupção


//Abaixo temos o htm da pagina do web service assincrono, oque é define nossa IHM de entrada
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <title>Enpoint Config</title>
    <meta charset="utf-8">
  </head>
  <body>
    <table align='center' width='400' border='0'>
        <form action="/get">
        <tr>
          <td align='center'>
            <h3>Multimedidor </h3>
          </td>
        </tr>
        <tr>
          <td  align='left'>
        <hr/>
              <h3 align='center' >Rede Access Point</h3>
              &nbsp;&nbsp;&nbsp;&nbsp;SSID: <input type="text" name="ssidAp"></br></br>
              &nbsp;&nbsp;&nbsp;&nbsp;Senha: <input type="text" name="senhaAp"></br></br>
        <hr/>
              <h3 align='center' >Rede Wifi</h3>
              &nbsp;&nbsp;&nbsp;&nbsp;SSID: <input type="text" name="ssidWifi"></br></br>
              &nbsp;&nbsp;&nbsp;&nbsp;Senha: <input type="text" name="senhaWifi"></br></br>
        <hr/>
              <h3 align='center' >Servidor MQTT</h3>
              &nbsp;&nbsp;&nbsp;&nbsp;Servidor: <input type="text" name="servidorMQTT"> </br></br>
              &nbsp;&nbsp;&nbsp;&nbsp;Porta: <input type="text" name="portaMQTT" size="5"></br></br>
              &nbsp;&nbsp;&nbsp;&nbsp;Login: <input type="text" name="loginMQTT"></br></br>
       &nbsp;&nbsp;&nbsp;&nbsp;Senha: <input type="text" name="senhaMQTT"></br></br>
    <hr/>
              <h3 align='center' >Calibrar sensores</h3>
              &nbsp;&nbsp;&nbsp;&nbsp;Tensão atual: <input type="text" name="calibraTensao" size="5" > V </br></br>
              &nbsp;&nbsp;&nbsp;&nbsp;Corrente atual: <input type="text" name="calibraCorrente" size="5"  > A</br></br>
     <hr/>
              <h3 align='center' >Circuito de Segurança</h3>
              &nbsp;&nbsp;&nbsp;&nbsp;Tensão de Desarme: <input type="text" name="tensaoDesarme" size="5" > V </br></br>
              &nbsp;&nbsp;&nbsp;&nbsp;Corrente de Desarme: <input type="text" name="correnteDesarme" size="5"> A</br></br>
       <tr>
          <td align='center'>
            <hr/>
      <input type="submit" value="Aplicar Configurções">
          </td>
    </form>
        </tr>
        <br>   
      </table>
 </body></html>)rawliteral";

//Funçao que executa as sinalizaçoes sonoras no buzzer piezoeletrico usando um DAC
void beep(int tempo, int qtdBeeps) {
  for (int i = 0; i <= qtdBeeps - 1; i++) {
    ledcWrite(0, 256);
    delay(tempo);
    ledcWrite(0, 0);
    delay(250);
  }
}
//Funçao  que configura o ponto de acesso wifi (IHM)
void configWifiManager() {
  String ssidString = flash.lerArquivo("/ssidAp.txt"); //Lê na memoria flash o arquivo previamente salvo
  String senhaString = flash.lerArquivo("/senhaAp.txt");
  char ssid2[ssidString.length() + 3] = {};
  char password2[senhaString.length() + 3] = {};
  ssidString.toCharArray(ssid2, ssidString.length() + 3);
  senhaString.toCharArray(password2, senhaString.length() + 3);
  WiFi.softAP(ssid2, password2);
  IPAddress IP = WiFi.softAPIP();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    if (request->hasParam("calibraTensao")) {
      CalibraTensao = request->getParam("calibraTensao")->value();
    }
    if (request->hasParam("calibraCorrente")) {
      CalibraCorrente = request->getParam("calibraCorrente")->value();
    }
    if (request->hasParam("ssidAp")) {
      ssidAp = request->getParam("ssidAp")->value();
    }
    if (request->hasParam("senhaAp")) {
      senhaAp = request->getParam("senhaAp")->value();
    }
    if (request->hasParam("ssidWifi")) {
      ssidWifi = request->getParam("ssidWifi")->value();
    }
    if (request->hasParam("senhaWifi")) {
      senhaWifi = request->getParam("senhaWifi")->value();
    }
    if (request->hasParam("servidorMQTT")) {
      servidorMQTT = String(request->getParam("servidorMQTT")->value());
    }
    if (request->hasParam("loginMQTT")) {
      loginMQTT = String(request->getParam("loginMQTT")->value());
    }
    if (request->hasParam("senhaMQTT")) {
      senhaMQTT = String(request->getParam("senhaMQTT")->value());
    }
    if (request->hasParam("tensaoDesarme")) {
      tensaoDesarme = String(request->getParam("tensaoDesarme")->value());
    }
    if (request->hasParam("correnteDesarme")) {
      correnteDesarme = String(request->getParam("correnteDesarme")->value());
    }
    newData = true;
    request->send(200, "text/html", "Dados Salvos com Sucesso<br><a href=\"/\">Retornar para Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
  beep(200, 2); // 2 beep curtos - wifi manager configurado com sucesso
}
//Funçao que apresenta erro se nao encontrar a pagina no webservice
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

//Funçao que salva os dados recebidos do webservice da IHM na flash do microcontrolador
void attDados() {
  if (newData) {
    if (!CalibraTensao.equals("")) {
      setCalibraTensao(CalibraTensao);
      CalibraTensao = "";
    }
    if (!CalibraCorrente.equals("")) {
      setCalibraCorrente(CalibraCorrente);
      CalibraCorrente = "";
    }
    if (!ssidAp.equals("")) {
      flash.salvarArquivo("/ssidAp.txt", ssidAp);
      ssidAp = "";
    }
    if (!senhaAp.equals("")) {
      flash.salvarArquivo("/senhaAp.txt", senhaAp);
      senhaAp = "";
    }
    if (!ssidWifi.equals("")) {
      flash.salvarArquivo("/ssidWifi.txt", ssidWifi);
      ssidWifi = "";
    }
    if (!senhaWifi.equals("")) {
      flash.salvarArquivo("/senhaWifi.txt", senhaWifi); senhaWifi);
      senhaWifi = "";
      conectarWifi();
    }
    if (!servidorMQTT.equals("")) {
      flash.salvarArquivo("/servidorMQTT.txt", servidorMQTT);
      servidorMQTT = "";
    }
    if (!portaMQTT.equals("")) {
      flash.salvarArquivo("/portaMQTT.txt", portaMQTT);
      portaMQTT = "";
    }
    if (!loginMQTT.equals("")) {
      flash.salvarArquivo("/loginMQTT.txt", loginMQTT); loginMQTT);
      loginMQTT = "";
    }
    if (!senhaMQTT.equals("")) {
      flash.salvarArquivo("/senhaMQTT.txt", senhaMQTT);
      senhaMQTT = "";
    }
    if (!tensaoDesarme.equals("")) {
      setTensaoDesarme(tensaoDesarme);
      tensaoDesarme = "";
    }
    if (!correnteDesarme.equals("")) {
      setCorrenteDesarme(correnteDesarme);
      correnteDesarme = "";
    }
    beep(400, 1); //1 beep longo - reprogramaçao remota realizada
    newData = false;
  }
  return;
}

//Funçao para conectar no wifi
void conectarWifi() {
  int tentativasDeConexao = 0;
  String ssidString = flash.lerArquivo("/ssidWifi.txt");
  String senhaString = flash.lerArquivo("/senhaWifi.txt");
  char ssid2[ssidString.length() + 3] = {};
  char password2[senhaString.length() + 3] = {};
  ssidString.toCharArray(ssid2, ssidString.length() + 3);
  senhaString.toCharArray(password2, senhaString.length() + 3);
  WiFi.begin(ssid2, password2);
  while ((WiFi.status() != WL_CONNECTED)) {
    tentativasDeConexao++;
    if (tentativasDeConexao > 5) {
      break;
    }
    delay(500);
  }
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.setAutoReconnect(false);
    conexaoDisponivel = true;
    beep(200, 3); //3 beep curtos - wifi conectado

  } else {
    conexaoDisponivel = false;
     beep(400, 2); //2 beep longo - wifi nao pode conectar
  }
}

//Funçao para enviar via MQTT os dados
void enviarMQTT(String mensagem, String topico) {
  String servidor = flash.lerArquivo("/servidorMQTT.txt");
  String porta = flash.lerArquivo("/portaMQTT.txt");
  String login = flash.lerArquivo("/loginMQTT.txt");
  String senha = flash.lerArquivo("/senhaMQTT.txt");
  char server[servidor.length() + 3] = {};
  char logon[login.length() + 3] = {};
  char pass[senha.length() + 3] = {};
  servidor.toCharArray(server, servidor.length() + 3);
  int port = porta.toInt();
  login.toCharArray(logon, login.length() + 3);
  senha.toCharArray(pass, senha.length() + 3);
  int tentativasDeConexao = 0;
  char msg[mensagem.length() + 3];
  mensagem.toCharArray(msg, mensagem.length() + 3);

  if (WiFi.status() == WL_CONNECTED) {
    client.setServer(server, port);
    if (!client.connected()) {
      while (!client.connected()) {
        String clientId = "Multimedidor";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), logon, pass)) {
        } else
          delay(500);
      }
    }
  }
  char topic[topico.length() + 3];
  topico.toCharArray(topic, topico.length() + 3);
  client.publish(topic, msg, true);
}
}

//Cria a estrutura inicial de arquivos na flash do microcontrolado
void criaEstruturaInicial() {
  String estruturaCriada = flash.lerArquivo("/estrutura.txt");
  if (!estruturaCriada.equals("EstruturaOK")) {
    flash.salvarArquivo("/ssidAp.txt", "Multimedidor");
    flash.salvarArquivo("/senhaAp.txt", "123456789");
    flash.salvarArquivo("/ssidWifi.txt", "Notebook");
    flash.salvarArquivo("/senhaWifi.txt", "1234567890");
    flash.salvarArquivo("/servidorMQTT.txt", "test.mosquitto.org");
    flash.salvarArquivo("/portaMQTT.txt", "1883");
    flash.salvarArquivo("/loginMQTT.txt", "Usuario");
    flash.salvarArquivo("/senhaMQTT.txt", "12345");
    flash.salvarArquivo("/CalibraTensao.txt", "1");
    flash.salvarArquivo("/CalibraCorrente.txt", "1");
    flash.salvarArquivo("/tensaoDesarme.txt", "2500");
    flash.salvarArquivo("/correnteDesarme.txt", "1000");
    flash.salvarArquivo("/estrutura.txt", "EstruturaOK");
  }
}
String getCalibraTensao() {
  String dado =  flash.lerArquivo("/CalibraTensao.txt");
  return dado;
}

void setCalibraTensao(String calibraTensao) {
  float novaCalibracao = ((calibraTensao.toFloat() * getCalibraTensao().toFloat()) / ultTensaoLida);
  flash.salvarArquivo("/CalibraTensao.txt", String(novaCalibracao));
}

String getCalibraCorrente() {
  String dado =  flash.lerArquivo("/CalibraCorrente.txt");
  return dado;
}
void setCalibraCorrente(String CalibraCorrente) {
  float novaCalibracao = ((CalibraCorrente.toFloat() * getCalibraCorrente().toFloat()) / ultCorrenteLida);
  if (novaCalibracao <= 0) {
    novaCalibracao = 1;
  }
  flash.salvarArquivo("/CalibraCorrente.txt", String(novaCalibracao));
}

String getTensaoDesarme() {
  String dado =  flash.lerArquivo("/tensaoDesarme.txt");
  return dado;
}

void setTensaoDesarme(String tensaoDesarme) {
  flash.salvarArquivo("/tensaoDesarme.txt", tensaoDesarme);
}

String getCorrenteDesarme() {
  String dado = flash.lerArquivo("/correnteDesarme.txt");
  return dado;
}

void setCorrenteDesarme(String correnteDesarme)  Serial.println("Calback flash.salvarArquivo(" / senhaMQTT.txt",) ");
flash.salvarArquivo("/correnteDesarme.txt", correnteDesarme);
}

void getTensao() {
  emon1.calcVI(17, 400);    //FUNÇÃO DE CÁLCULO (17 SEMICICLOS, TEMPO LIMITE PARA FAZER A MEDIÇÃO)
  ultTensaoLida =  emon1.Vrms * getCalibraTensao().toFloat();
  if (ultTensaoLida < 50) {   //SE O VALOR DA VARIÁVEL FOR MENOR QUE 0
    ultTensaoLida = 0;    //VARIÁVEL RECEBE 0
  }
}

void getCorrente() {
  emon1.calcVI(17, 100);
  ultCorrenteLida = (emon1.Irms) * (getCalibraCorrente().toFloat()); //VARIÁVEL RECEBE O VALOR RESULTANTE DA CORRENTE RMS MENOS O RUÍDO
  if (ultCorrenteLida < 0) { //SE O VALOR DA VARIÁVEL FOR MENOR QUE 0, FAZ
    ultCorrenteLida = 0; //VARIÁVEL RECEBE 0
  }
}

void ConfiguraSensores() {
  correnteSegura = getCorrenteDesarme().toFloat();
  tensaoSegura = getTensaoDesarme().toFloat();
  emon1.voltage(zmtp, 211.6 , 1.7); //PASSA PARA A FUNÇÃO OS PARÂMETROS (PINO ANALÓGIO / VALOR DE CALIBRAÇÃO / MUDANÇA DE FASE);
  emon1.current(acs, 18.40);
}

void verificaTensaoSegura() {
  //Valores acima do seguro, desarmar rele
  if ((ultTensaoLida >= tensaoSegura) || ultCorrenteLida >= correnteSegura) {
    digitalWrite(rele, LOW);
    beep(1000, 1); //1 beep muito longo - tensao segura excedida
  }
}

void configuraRTC() {
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // Ajusta a data e hora do rtc de acordo com a data que o firmware foi gravado
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void ConfiguraDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  display.clearDisplay();
  SplashScreen();
  display.setTextSize(1);
  display.setTextColor(WHITE);

}
String zeroFill(int tamanhoDaString, int numero) {
  String numeroComZeros = "";
  while ((numeroComZeros.length() + (String(numero)).length()) < tamanhoDaString) {
    numeroComZeros += "0";
  }
  numeroComZeros += String(numero);
  return numeroComZeros;
}
String getDateTime() {
  DateTime now = rtc.now();
  return String(zeroFill(2, now.day()) + "/" + zeroFill(2, now.month()) + "/" + String(now.year()) + " " + zeroFill(2, now.hour()) + ":" + zeroFill(2, now.minute()) + ":" + zeroFill(2, now.second()));
}

void SplashScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //Desenha um circulo, nas coodenadas cartesianas x=48 e y=6, e com raio 6
  display.fillCircle(48, 6, 6, WHITE);
  display.fillRoundRect(42, 15,  12, 12,  2, WHITE);
  display.fillRoundRect(42, 29,  12, 12,  2, WHITE);
  display.fillRoundRect(42, 43,  12, 12,  2, WHITE);
  display.fillRoundRect(57, 1,  12, 12,  2, WHITE);
  display.fillRoundRect(57, 15,  12, 12,  2, WHITE);
  display.fillRoundRect(57, 29,  12, 12,  2, WHITE);
  display.fillRoundRect(57, 43,  12, 12,  2, WHITE);
  display.fillRoundRect(72, 1,  12, 12,  2, WHITE);
  display.fillRoundRect(72, 15,  12, 12,  2, WHITE);
  display.setCursor(30, 57); //Posiciona o cursor na posição x=30 e y=57
  display.println("MULTIMEDIDOR"); //Escreve o texto iniciando da posição do cursor
  display.display(); //Aplica todas as alteraçoes no display

}

void ApresentarNoDisplay() {
  if (stateDisplay) {
    display.ssd1306_command(SSD1306_DISPLAYON);
    display.clearDisplay();
    display.setTextSize(1);
    //Data
    DateTime now = rtc.now();
    display.setCursor(0, 10);
    display.println(getDateTime());
    //Tensao
    display.setCursor(0, 20);
    display.print("Tensao: ");
    display.print(String(ultTensaoLida));
    display.print("V");
    //Corrente
    display.setCursor(0, 30);
    display.print("Corrente: ");
    display.print(String(ultCorrenteLida));
    display.print("A");
    //Potencia instantanea
    display.setCursor(0, 40);
    display.print("Potencia: ");
    display.print(String(ultCorrenteLida * ultTensaoLida));
    display.print("W");
    //IP Wifi Manager
    display.setCursor(0, 50);
    display.print("192.168.4.1");
    display.display();
  } else {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
  }
}


void IRAM_ATTR interrupcao() {
  portENTER_CRITICAL_ISR(&mux);
  lastStateDisplay = !lastStateDisplay;
  delayMicroseconds(100000);
  portEXIT_CRITICAL_ISR(&mux);
}

void setup() {
  Serial.begin(115200); //INICIALIZA A SERIAL
  delay(2000);
  pinMode(rele , OUTPUT);
  pinMode(ledOn , OUTPUT);
  pinMode(buzzer , OUTPUT);
  digitalWrite(rele, HIGH);
  digitalWrite(ledOn, HIGH);
  ledcAttachPin(buzzer, 0);//Atribuimos o pino 23 ao canal 0.
  ledcSetup(0, 1000, 10);
  pinMode(btn, INPUT_PULLUP);  // Pull up to 3.3V on input - some buttons already have this don
  attachInterrupt(digitalPinToInterrupt(btn), interrupcao, CHANGE);   //configura a interrupção do botão no evento CHANGE para a função handleButtonInterrupt
  beep(200, 1); // 1 beeb indicando que o multimedidor foi iniciado
  ConfiguraDisplay();
  flash.inicializaFlash();
  criaEstruturaInicial();
  configuraRTC();
  configWifiManager();
  ConfiguraSensores();
  conectarWifi();
  delay(3000);
}

void loop() {
  portENTER_CRITICAL_ISR(&mux); // início da seção crítica
  stateDisplay = lastStateDisplay;
  portEXIT_CRITICAL_ISR(&mux); // fim da seção crítica
  getTensao();
  getCorrente();
  verificaTensaoSegura();
  ApresentarNoDisplay();
  String mensagem = "{'tensao':'" + String(ultTensaoLida) + "','corrente':'" + String(ultCorrenteLida) + "','data':'" + getDateTime() + "'}";
  enviarMQTT(mensagem, "Dados");
  attDados();
}
