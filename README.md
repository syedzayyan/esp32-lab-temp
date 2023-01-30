# ESP32 Temperature Reporter

I know this is the most intuitive name and this is were I have peaked in naming things. This project is the idea of Dr. Jonathan Ho whom I met doing my 3rd BSc Research Project at JVSC, WHRI, QMUL. He wanted an ESP32 board to email him when the temperature rose to uncomfortable levels for the lab specimens in the freezers.

I am attaching the references at the top because they do a way better job of explaining things I will in the next paragraphs.

- [SD Card Module Tutorial with ESP32](https://www.mischianti.org/2021/03/28/how-to-use-sd-card-with-esp32-2/)
- [Code and Docs for Email Library used](https://github.com/mobizt/ESP-Mail-Client)
- [Tutorial to connect the DHT 22 sensor used in my prototype](https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/)
- [It's a tutorial to serve a react app which is not really necessary but worth a read if you'd like to contribute to the code](https://techtutorialsx.com/2019/03/24/esp32-arduino-serving-a-react-js-app/)
- [WPA2-Enterprise code example, because eduroam is used by QMUL](https://github.com/martinius96/ESP32-eduroam/blob/master/2022/eduroam/eduroam.ino)

## Outline

The program does a few things when configured properly:

- Logs temperature (from one sensor for the time being)
- Provides Web interface to monitor last two days worth of logs (can be customized)
- Email when threshold temperature is reached. Logic is there so that it checks 10 minutes after a spike is read in case the first one is a false positive
- Stores all data in CSV files in the SD Card

## Parts Required

- (ESP32 (The computer))[https://www.amazon.co.uk/ESP-32S-Development-Bluetooth-Microcontroller-ESP-WROOM-32/dp/B08DR5T897/ref=sr_1_3?crid=3FH1A1HFC7NGQ&keywords=esp32&qid=1675079312&sprefix=esp32%2Caps%2C72&sr=8-3]
- (SD Card module)[https://www.amazon.co.uk/dp/B06XHJTGGC/ref=redir_mobile_desktop?_encoding=UTF8&psc=1&ref=ppx_pop_mob_b_asin_title]
- (SD Card)[https://www.amazon.co.uk/Gigastone-5-Pack-Surveillance-Security-Action/dp/B0876H3YBQ/ref=sr_1_1_sspa?crid=OK9ZXARIV712&keywords=8+gb+micro+sd+cards&qid=1675079457&s=computers&sprefix=8+gb+micro+sd+cards%2Ccomputers%2C66&sr=1-1-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&psc=1]
- (Temperature sensor)[https://www.amazon.co.uk/Humidity-and-Temperature-Sensor-DHT22/dp/B005A9KJ4I]
- A USB cable and a power supply

## Software

- Arduino IDE

## Steps

##### Work in Progress

- Copy this github repo
- Install all the libraries in the Arduino IDE
- Copy `index.html` to the SD card
- Wire everything up. Using the references.
- Configure the sketch to your needs. Provide email details. For Gmail, follow (this)[https://randomnerdtutorials.com/esp32-send-email-smtp-server-arduino-ide/] Add WiFi details. The code is decently commented
- Upload the code
- Everything should work

# Feel free to contribute!