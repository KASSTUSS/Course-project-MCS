#include <SPI.h> //SPI библиотека для SD карты
#include <SD.h> //библиотека чтобы считывать информацию с SD карты
#include "TMRpcm.h" //библиотека чтобы проигрывать аудио файлы
#define SD_CHIP_SELECT 4 // вывод, подключенный к линии выбора чипа на SD карте
#define BUTTON_PIN 2 // пин, к которому подключена кнопка
#define AUDIO_PIN 9 // пин, на который вызодит аудио сигнал
TMRpcm music; // объявление объекта библиотеки для проигрываения аудио файлов
boolean button = false; // Логическая переменная для хранения состояние кнопки
boolean press_flag = false; // Флажок нажатия кнопки
boolean long_press_flag = false; // Флажок долгого нажатия на кнопку
unsigned long last_press = 0; // Хранит время с последнего нажатия на кнопку
String song_name_str = ""; // переменная для хранения имени аудио файла
char buf[13]; // переменная для приведения к типу char[]
boolean play_pause = true; // переменная для хранения состояние play/pause
File root; // объявление объекта файла для чтения данных с SD-карты
void setup() {
music.speakerPin = AUDIO_PIN; // аудио выход на контакте 9
pinMode(BUTTON_PIN, INPUT_PULLUP); // подключение кнопки с внутренним
подтягивающим резистором
if (!SD.begin(SD_ChipSelect)) { // инициализация и проверка SD-карты
while (true);
}
root = SD.open("/music"); // открыть директорию для работы
music.setVolume(4); // задать необходимый уровень громкости
music.quality(2); // задать необходимый уровень обработки аудио файлов
playNextMusic(); // вызываем функуции для включения проигрываения аудио файла,
при включении проигрывателя
}
void loop() {
button = !digitalRead(BUTTON_PIN); // считывает текущее состояние кнопки
// обработка короткого нажатия и устранение дребезга
if (button == true && press_flag == false && millis() - last_press > 30) {
press_flag = !press_flag;
last_press = millis();
}
//обработка длительного нажатия
if (button == true && press_flag == true && millis() - last_press > 850) {
long_press_flag = !long_press_flag;
last_press = millis();
isButtonHold();
}
if (button == false && press_flag == true && long_press_flag == true) {
press_flag = !press_flag;
long_press_flag = !long_press_flag;
}
if (button == false && press_flag == true && long_press_flag == false) {
press_flag = !press_flag;
isButtonSingle();
}
// если заканчивается воспроизвдение файла, воспроизвести следующего файла
if (!music.isPlaying() && play_pause) {
delay(100);
playNextMusic();
}
}
// функция вызывающаяся при коротом нажатии на кнопку
void isButtonSingle() {
play_pause = !play_pause;
music.pause();
}
// функция вызывающаяся при продолжительном нажатии на кнопку
void isButtonHold() {
playNextMusic();
}
void playNextMusic() {
File next_song = root.openNextFile(); // открываем следующий файл в директории
if (!next_song) { // проверяем наличие этого файла
root.rewindDirectory(); // если файла нет, возврщаемся в начало директории
next_song = root.openNextFile(); // открываем файл в директории
}
song_name_str = "/music/" + next_song.name();
song_name_str.toCharArray(buf, song_name_str.length() + 1);
next_song.close();
music.play(buf); // воспроизводим файл
}