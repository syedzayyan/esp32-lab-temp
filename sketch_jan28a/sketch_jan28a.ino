//https://www.mischianti.org/2021/03/28/how-to-use-sd-card-with-esp32-2/
//https://github.com/mobizt/ESP-Mail-Client
//https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/
//https://techtutorialsx.com/2019/03/24/esp32-arduino-serving-a-react-js-app/
//https://github.com/martinius96/ESP32-eduroam/blob/master/2022/eduroam/eduroam.ino

//You need to create a temp_date.csv file and put in 'temp,tm_dt' and a new line after for the first time. Or else the web server won't work

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "DHT.h"
#include "time.h"
#include <string.h>
#include <ESP_Mail_Client.h>
#include "esp_wpa2.h"

//Wifi Stuff
//Credentials for Eduroam/WPA2_Enterprise.
#define EAP_IDENTITY ""
#define EAP_ANONYMOUS_IDENTITY "" //same as identity usually unless you have something else
#define EAP_PASSWORD "" // Password

const char* ssid_eduroam = "eduroam";
const char* ssid = "";
const char* password =  "";

#define DHTPIN 4 // PIN No. for Temperature Sensor
#define DHTTYPE DHT22 // Type of DHT sensor
DHT dht(DHTPIN, DHTTYPE);

//Time source for the logger. No daylight savings because it's easier that way. Somewhere near future will change
const char* ntpServer = "uk.pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;

float temperature_threshold = 0.00; // Threshold for temperature and needs decimal figures
int timerFlag = 600000; // Time to email if temperature drops to 
int csvOverWriteFlag = 172800000; // CSV holds data for 2 days. You can change this in milliseconds

//Global Declaration of classes used.
SMTPSession smtp;
ESP_Mail_Session session;
AsyncWebServer server(80);

void sendEmail(){
  //Please Consult the Reference Given Above. I used GMAIL which requires app passwords, other mail clients would have different configs.
  session.server.host_name = "smtp.gmail.com";
  session.server.port = 587;
  session.login.email = "";
  session.login.password = "";
  session.login.user_domain = "gmail.com";

  // No need to touch this
  session.time.ntp_server = "pool.ntp.org,time.nist.gov";
  session.time.gmt_offset = 0;
  session.time.day_light_offset = 0;

  SMTP_Message message;

  // Set the message headers
  message.sender.name = "My Mail";
  message.sender.email = "";
  message.subject = "High Alert!!!";
  message.addRecipient("name1", "");

  // Set the message content
  message.text.content = "The temperature of the freezer has risen to unacceptable levels. Please do something quickly before a PhD student needs more therapy!";

  // Connect to the server
  smtp.connect(&session /* session credentials */);

  // Start sending Email and close the session
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}

void initSDCard(){
    if(!SD.begin(5)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void initWifi(){
  WiFi.mode(WIFI_STA);
  //Uncomment for normal WPA2 wifi
  //WiFi.begin(ssid, password);

  //Uni Wifi I have uses eduroam so this is how I connect usually
  WiFi.disconnect(true);  //disconnect from WiFi to set new WiFi connection
  WiFi.mode(WIFI_STA); //init wifi mode
  //uncomment below to use a root ca. i didnt need it for qmul
  //WiFi.begin(ssid_eduroam, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD, test_root_ca);
  WiFi.begin(ssid_eduroam, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD);

  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());

}

float temperatureGet(){
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return 0.0;
  }
  return t;
}
void appendCSVTemp(fs::FS &fs, const char * path, const float temperature){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  char *currtm = asctime(&timeinfo);
  if (currtm[strlen(currtm)-1] == '\n') currtm[strlen(currtm)-1] = '\0';

  String buf;
  buf += F(currtm);
  buf += F(",");
  buf += String(temperature, 6);
  buf += F("\n");
  Serial.println(buf);
  
  if(file.print(buf)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return "Null";
  }

  Serial.print("Read from file: ");
  String textToSend;
  while(file.available()){
    textToSend += file.read();
  }
  file.close();
  return textToSend;
}
void setup() {
  Serial.begin(115200);
  initWifi();
  initSDCard();
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SD, "/index.html", "text/html");
  });
  server.on("/gettempdata", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SD, "/temp_date.csv", "text/csv");
  });   
  server.serveStatic("/", SD, "/");
  server.begin();
  dht.begin();
}

void loop() {
  delay(30000);
  float temp = temperatureGet();
  if (temp > temperature_threshold) {
   Serial.print(timerFlag);
   Serial.print("\n");
   timerFlag = timerFlag-30000;
   if (timerFlag == 0){
      Serial.print("something has happened");
      sendEmail();
    };
  };
  csvOverWriteFlag -= 30000;
  if (csvOverWriteFlag == 0){
    csvOverWriteFlag = 172800000;
    writeFile(SD, "/temp_date.csv", "temp,tm_dt\n");
  };
  appendCSVTemp(SD, "/temp_date.csv", temp);
}
