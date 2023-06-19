#include <EEPROM.h>
#include <GyverPortal.h>
#include <FastBot.h>

#define EEPROM_SIZE 4096
#define BOT_TOKEN "5883574988:AAEsK2a-iScfWiZxYMmXHOuwCo_mikhJLjo"

String esp_ssid = "set_info";
String esp_pwd = "adminadmin123";
GyverPortal ui;
FastBot bot(BOT_TOKEN);

struct {
  String ssid;
  String pwd;
  int time;
} info ;

String last_command = "";
String last_chat_id;

String IPtoString(IPAddress ip) { // IP v4 only
  String ips;
  ips.reserve(16);
  ips = ip[0];  ips += '.';
  ips += ip[1]; ips += '.';
  ips += ip[2]; ips += '.';
  ips += ip[3];
  return ips;
}

void try_to_connect() {
  int timer = millis();
  WiFi.mode(WIFI_STA);
  WiFi.begin(info.ssid, info.pwd);
  for (int i = 0 ; i < 30; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(WiFi.localIP());
      return;
    }
    delay(500);
  }
  WiFi.mode(WIFI_AP);
  WiFi.softAP(esp_ssid, esp_pwd);
  Serial.println("point acessed");
  Serial.println(WiFi.localIP());
}

bool check_num(String msg) {
  int res = 0;
  for (int i = 5 ; i < msg.length(); i++) {
    if ((msg[i] >= '0') && (msg[i] <= '9')) {
      res *= 10;
      res += msg[i] - '0';
    } else {
      return false;
    }
  }
  info.time = res;
  return true;
}

void newMsg(FB_msg& msg) {
  // выводим ID чата, имя юзера и текст сообщения
  Serial.print(msg.chatID);     // ID чата 
  Serial.print(", ");
  Serial.print(msg.username);   // логин
  Serial.print(", ");
  Serial.println(msg.text);     // текст
//  Serial.print(", ");
//  Serial.println(msg.query);
  last_command = msg.text;
  last_chat_id = msg.chatID;
}

// конструктор страницы
void build() {
  GP.BUILD_BEGIN(500);
  GP.THEME(GP_DARK);
  GP.TEXT("txt1", "ssid", info.ssid);
  GP.BUTTON_MINI("btn1", "Send", "txt1");
  GP.TEXT("txt2", "password", info.pwd);
  GP.BUTTON_MINI("btn2", "Send", "txt2");
  GP.NUMBER("txt3", "delay time", info.time);
  GP.BUTTON_MINI("btn3", "Send", "txt3");
  GP.BREAK();
  GP.AREA_LOG(5);
  GP.BUILD_END();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, info);
  
  try_to_connect();

  ui.start();
  ui.attachBuild(build);
  ui.attach(action);

  bot.attach(newMsg);
}

void action() {
  if (ui.click("btn1")) {
    info.ssid = ui.getString("btn1");
    Serial.println(info.ssid);
    // ui.log.print("ssid set: ");
    // ui.log.println(info.ssid);
  }
  if (ui.click("btn2")) {
    info.pwd = ui.getString("btn2");
    Serial.println(info.pwd);
    // ui.log.print("password set: ");
    // ui.log.println(info.pwd);
  }
  if (ui.click("btn3")) {
    info.time = ui.getInt("btn3");
    Serial.println(info.time);
    // ui.log.print("time set: ");
    // ui.log.println(info.time);
  }
  EEPROM.put(0, info);
  EEPROM.commit();
}

int last_command_time;
bool cur_state = false;

void loop() {
  // EEPROM.put(address, param); // write
  // EEPROM.get(address, readParam); // read
  // EEPROM.commit() // commit(after write)
  // address += sizeof(param);
  // 1 simbol == 0.5 byte of mem
  ui.tick();
  bot.tick();
  if (last_command == "/on") {
    bot.sendMessage("Включено", last_chat_id);
    last_command_time = millis();
    digitalWrite(LED_BUILTIN, 1);
    cur_state = true;
    last_command = "";
  } else if (last_command == "/off") {
    bot.sendMessage("Выключено", last_chat_id);
    last_command_time = millis() - info.time * 1001;
    digitalWrite(LED_BUILTIN, 0);
    cur_state = false;
    last_command = "";
  } else if (last_command == "/info") {
    bot.sendMessage("Wifi ssid: " + info.ssid, last_chat_id);
    bot.sendMessage("Current time: " + String(info.time), last_chat_id);
    bot.sendMessage("Local IP: " + IPtoString(WiFi.localIP()), last_chat_id);
    last_command = "";
  } else if (last_command.startsWith("/set")) {
    if (check_num(last_command)) {
      bot.sendMessage("Time set to " + String(info.time), last_chat_id);
    } else {
      bot.sendMessage("Invalid number", last_chat_id);
    }
    last_command = "";
  }
  if (cur_state && ((millis() - last_command_time) > info.time * 1000)) {
    digitalWrite(LED_BUILTIN, 0);
    bot.sendMessage("Выключено", last_chat_id);
    cur_state = false;
  }
}