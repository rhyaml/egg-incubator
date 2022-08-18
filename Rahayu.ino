/*
  Board : LOLIN (WEMOS) D1 R2 & Mini
*/

#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

LiquidCrystal_I2C lcd (0x27, 16, 2);
DHT dht(D3, DHT22);

const char* ssid = "rhyaml";
const char* password = "lolipop123";

// Rahayu
#define BOTtoken "5301276504:AAEHUISnJljP0TJXN1ih6lB7x5-zuMqxaf8"
#define CHAT_ID "1222844442"


#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

#define relayLamp D4
#define motor  D5
#define fan D6

float hum, temp;
String relayLampTG, motorTG = "";
String stringSpeed;
bool relayLampState, motorState = 1;
bool dbg = 0;
bool lcdTrig = 1;
bool notifTrigOn = 1;
bool notifTrigOff = 1;
bool motorStartup = 1;
bool allowAutomatic = 1;
unsigned long motorTimes;

int x;
int y;
int speedValue;
String z;

float A, B;
int sel_;
float a1, b1a, b1b, c1;
float A1, A2, A3, A4, A5, A6, A7;
float M1, M2, M3, M4, M5, M6, M7;
float suhu, motordc;
int pwmMamdani;
int fanFixSpeed = 500;
String fanStrSpeed;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Telegram Smart Home. Silakan Gunakan Perintah Dibawah Ini Untuk Mengontrol Rumah\n";
      welcome += "/status : Melihat Data Sensor\n";
      welcome += "/motor_on : Menyalakan Manual Motor Penggerak\n";
      welcome += "/automaticMotor_off : Mematikan Perintah Otomatis Motor\n";
      welcome += "/automaticMotor_on : Menyalakan Perintah Otomatis Motor";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/status") {
      if (relayLampState == 0) {
        relayLampTG = "Menyala";
      } else {
        relayLampTG = "Mati";
      }
      String noteAllowAutomatic;
      if (allowAutomatic) {
        noteAllowAutomatic = "Menyala";
      } else {
        noteAllowAutomatic = "Mati";
      }
      hum = dht.readHumidity();
      temp = dht.readTemperature();
      if (isnan(hum) || isnan(temp) ) {
        Serial.println("Failed to read from DHT sensor!");
        return;
      }
      String text = "Status Inkubator Penetasan Telur Saat Ini : \n";
      text += "Suhu : ";
      text += temp;
      text += "°C \n";
      text += "Kelembaban : ";
      text += hum;
      text += "%\n";
      text += "Lampu : ";
      text += relayLampTG + "\n";
      text += "Kecepatan Kipas : ";
      text += fanStrSpeed + "\n";
      text += "Otomatis Motor : ";
      text += noteAllowAutomatic;
      bot.sendMessage(chat_id, text, "");
    } else if (text == "/motor_on") {
      String text = "Motor Dinyalakan Manual\n";
      bot.sendMessage(chat_id, text, "");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Motor Bergerak");
      digitalWrite(motor, 0);
      delay(6000);
      digitalWrite(motor, 1);
      motorTimes = millis();
      String ntf = "Motor Akan Menyala Lagi 3 Jam Dari Sekarang\n";
      bot.sendMessage(chat_id, ntf, "");
    } else if (text == "/automaticMotor_off") {
      allowAutomatic = 0;
      String text = "Motor Sudah Tidak Menyala Otomatis\n";
      bot.sendMessage(chat_id, text, "");
    } else if (text == "/automaticMotor_on") {
      allowAutomatic = 1;
      String text = "Motor Sudah Menyala Otomatis\n";
      bot.sendMessage(chat_id, text, "");
    }

  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  lcd.begin();
  lcd.setCursor (0, 0);
  lcd.print("Starting Program");
#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
#endif
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
#ifdef ESP32
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  dht.begin();
  pinMode(relayLamp, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(fan, OUTPUT);
  digitalWrite(relayLamp, relayLampState);
  digitalWrite(motor, motorState);
  if (dbg) {
    debug();
  }
  Serial.println("Starting TelegramBot...");

}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  getDht();
  display();
  if (temp <= 37.8 ) {
    if (notifTrigOn) {
      String text = "Lampu Menyala";
      bot.sendMessage(CHAT_ID, text, "");
    }
    notifTrigOn = 0;
    notifTrigOff = 1;
    relayLampState = 0;
    digitalWrite(relayLamp, relayLampState);
  } else if (temp >= 38.3) {
    if (notifTrigOff) {
      String text = "Lampu Mati";
      bot.sendMessage(CHAT_ID, text, "");
    }
    notifTrigOff = 0;
    notifTrigOn = 1;
    relayLampState = 1;
    digitalWrite(relayLamp, relayLampState);
  }
  periodic();
  moment();
  mamdaniDisplay();
  analogWrite(fan, fanFixSpeed);
  //  if (z == "Lambat") {
  //    analogWrite(fan, fanFixSpeed);
  //  } else if (z == "Cepat") {
  //    analogWrite(fan, fanFixSpeed);
  //  }
  delay(250);
}

void getDht() {
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  suhu = dht.readTemperature();
  if (isnan(hum) || isnan(temp) || isnan(suhu)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (dbg) {
    Serial.print("suhu: ");
    Serial.println(temp);
    Serial.print(" ");
    Serial.print("kelembaban: ");
    Serial.print(hum);
  }

}

void display() {
  if (lcdTrig) {
    lcd.clear();
    lcdTrig = 0;
  }
  lcd.setCursor(0, 0);
  lcd.print("Temp : ");
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Hum  : ");
  lcd.print(hum);
  lcd.print("%");
  delay(1000);
}

void periodic() {
  //  if (millis()-motorTimes <= 10800000UL){
  if (millis() - motorTimes >= 10800000UL || motorStartup) {
    motorStartup = 0;
    if (allowAutomatic) {
      motorTimes = millis();
      String text = "Motor Begerak";
      bot.sendMessage(CHAT_ID, text, "");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Motor Bergerak");
      digitalWrite(motor, 0);
      delay(20000);
      digitalWrite(motor, 1);
    }
  }
}



void debug() {
  while (dbg) {
    getDht();
    lcd.setCursor(0, 0);
    lcd.print("Temp : ");
    lcd.print(temp);
    lcd.print((char)223);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("Hum  : ");
    lcd.print(hum);
    lcd.print("%");
    delay(1000);
  }
}
