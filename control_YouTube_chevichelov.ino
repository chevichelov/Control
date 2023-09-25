#include <Adafruit_SSD1306.h>                                          //Подключаем библиотеку
#include <Wire.h>                                                      //Подключаем библиотеку

#define TFT_WIDTH   128                                                //Ширина Экрана
#define TFT_HEIGHT  64                                                 //Высота Экрана
#define TFT_RESET   -1 

Adafruit_SSD1306 TFT(TFT_WIDTH, TFT_HEIGHT, &Wire, TFT_RESET);        //Номер вывода сброса (или -1 при совместном использовании вывода сброса Arduino)


#define PIN_A0   A0                                                   //Назначаем пин A0
#define PIN_A1   A1                                                   //Назначаем пин A1

#define PIN_D2   2                                                    //Назначаем пин D2
#define PIN_D3   3                                                    //Назначаем пин D3
#define PIN_D4   4                                                    //Назначаем пин D4

#define PIN_D5   5                                                    //Назначаем пин D5
#define PIN_D6   6                                                    //Назначаем пин D6
#define PIN_D9   9                                                    //Назначаем пин D7

#define PIN_D10   10                                                  //Назначаем пин D10
#define PIN_D11   11                                                  //Назначаем пин D11

float DEFAULT_VOLT = 0;                                               //Напряжение по умолчанию

#define R1   98.9                                                     //Сопротивление резистора R1 в кОм
#define R2   9.7                                                      //Сопротивление резистора R2 в кОм


#define COLOR_TEXT WHITE                                              //Цвет текста
#define COLOR_DRAW WHITE                                              //Цвет графиков

#define MAX_VOLT_BATTERY 4.2                                          //Максимальное напряжение батареи
#define OFF_VOLT_BATTERY 2.4                                          //Минимальное напряжение батареи, при котором она отключится


#define LENGTH 200                                                    //Количество измерений
int VOLTS[LENGTH];                                                    //Массив измерений напряжения
float VOLT  = 0;                                                      //Напряжение по умолчанию

void setup() {
  pinMode(PIN_A0, INPUT);                                             //Пин установлен на вход
  pinMode(PIN_A1, INPUT);                                             //Пин установлен на вход
  pinMode(PIN_D2, INPUT_PULLUP);                                      //Пин установлен на вход с подтяжкой на внутреннее сопротивление
  pinMode(PIN_D3, INPUT_PULLUP);                                      //Пин установлен на вход с подтяжкой на внутреннее сопротивление
  pinMode(PIN_D4, INPUT_PULLUP);                                      //Пин установлен на вход с подтяжкой на внутреннее сопротивление
  
  pinMode(PIN_D5, OUTPUT);                                            //Пин установлен на выход
  pinMode(PIN_D6, OUTPUT);                                            //Пин установлен на выход
  pinMode(PIN_D9, OUTPUT);                                            //Пин установлен на выход

  pinMode(PIN_D10, OUTPUT);                                           //Пин установлен на выход
  pinMode(PIN_D11, OUTPUT);                                           //Пин установлен на выход
  
  TFT.begin(SSD1306_SWITCHCAPVCC, 0x3C);                              //Инициализируем экран
  TFT.setTextSize(1);                                                 //Устанавливаем размер шрифта
  TFT.setTextColor(COLOR_TEXT);                                       //Устанавливаем цвет

  DEFAULT_VOLT = GET_DEFAULT_VOLT();                                  //Измеряем внутреннее напряжение Arduino
}

byte SELECT_OPTION = 0;                                               //Объявляем переменную для выбора опций
int MAX_VOLT = 0, MAX_NUMBER = 0;                                     //Объявляем переменные максимальное напряжение и максимальное число для графика
void loop() {
  LEFT();                                                             //Устанавливаем значение для положения экрана
  BATTERY();                                                          //Измеряем напряжение батареи
  
  
  if (BUTTON(PIN_D2))                                                 //Нажатием кнопки S1 выбираем нужную опцию
  {
    ++SELECT_OPTION;                                                  //Переключаем режимы
    if (SELECT_OPTION > 2)                                            //Если счётчик достиг максимума, обнуляем его
      SELECT_OPTION = 0;
  }
  //Прозвонка и вольтметр
  if (SELECT_OPTION == 0)                                              
  {
    FIND();                                                           //Функция прозвонки
  }
  //Поиск CAN шины
  if (SELECT_OPTION == 1)                                               
  {
    analogWrite(PIN_D5, 255);                                           //Включаем реле
    analogWrite(PIN_D9, LOW);                                           //Отключаем генератор

    analogWrite(PIN_D10, LOW);                                          //Отключаем светодиод
    analogWrite(PIN_D11, LOW);                                          //Отключаем светодиод
    MAX_VOLT      = 0;                                                  //Обнуляем максимальное значение напряжения
    analogWrite(PIN_D6, LOW);                                           //Отключаем спикер
    DISPLAY_FREQUENCY();                                                //Включаем функцию CAN шины
  }
  //Осциллограф
  if (SELECT_OPTION == 2)                                               
  {
    analogWrite(PIN_D5, 255);                                           //Включаем реле
    analogWrite(PIN_D9, LOW);                                           //Отключаем генератор

    analogWrite(PIN_D10, LOW);                                          //Отключаем светодиод
    analogWrite(PIN_D11, LOW);                                          //Отключаем светодиод
    MAX_VOLT      = 0;                                                  //Обнуляем максимальное значение напряжения
    MAX_NUMBER    = 0;                                                  //Обнуляем максимальное значение графики
    analogWrite(PIN_D6, LOW);                                           //Отключаем спикер
    BACKGROUND();                                                       //Рисуем сетку
    SELECT_BUTTOM_TIME();                                               //Выбираем время иследования сигнала
    DISPLAY_GRAPH();                                                    //Рисуем график
  }
  
  TFT.display();                                                          //Выводим данные на экран
}

unsigned long TIME_BATTERY  = 0;                                          //Объявляем переменную таймера измерения напряжения батареи
void BATTERY()                                                            //Функция выводит напряжение батареи в виде иконци батареи
{
  TFT.clearDisplay();                                                     //Очищаем экран
  TFT.fillRect(0, 1, 2, 3, COLOR_DRAW);                                   //Рисуем заполненный квадрат
  TFT.drawRect(2, 0, 10, 5, COLOR_DRAW);                                  //Рисуем квадрат
  if (millis() - TIME_BATTERY >= 60000)                                   //Измеряем раз в 1 минуту = 60 секунд
  {
    DEFAULT_VOLT      = GET_DEFAULT_VOLT();                               //Получаем внутренние напряжение
    TIME_BATTERY  = millis();                                             //Обновляем таймер
  }
  TFT.fillRect(map(DEFAULT_VOLT * 10, MAX_VOLT_BATTERY * 10, OFF_VOLT_BATTERY * 10, 20, 120) / 10 , 0, map(DEFAULT_VOLT * 10, MAX_VOLT_BATTERY * 10, OFF_VOLT_BATTERY * 10, 100, 0) / 10, 5, COLOR_DRAW); //Заполняем иконку батареи в зависимости от её заряда
}

unsigned long TIME_BUTTON = 0;                                            //Объявляем переменную таймера кнопки
bool SELECT_BUTTON        = true;                                         //Объявляем переменную статуса кнопки
bool BUTTON(byte PIN) {                                                   //Функция определяет нажата ли кнопка 
  if (SELECT_BUTTON)                                                      //Избавляемся от дребезга дещёвых кнопок
      if (!digitalRead(PIN) && millis() - TIME_BUTTON >= 400)             //Не позволяет засчитывать нажатие клавиши чаще, чем раз в 400 мс., тем самым защищает от дребезга дешёвых кнопок
      {
        SELECT_BUTTON   = false;                                          //Устанавливаем переменную в значение кнопка нажата
        TIME_BUTTON    = millis();                                        //Сохраняем время последнего нажатия
        return true;                                                      //Кнопка нажата
      }
    if (digitalRead(PIN))                                                 //Засчитываем отжатие кнопки
        SELECT_BUTTON = true;                                             //Устанавливаем переменную в значение кнопка отжата

    return false;                                                         //Кнопка отжата
}

static char* DISPLAY_TEXT(float DATA, int COUNT, int FLOAT)               //Функция выводит числа на экран
{
  static char DATA_RESULT[20];                                            //Объявляем переменную 
  dtostrf(DATA, COUNT, FLOAT, DATA_RESULT);                               //Конвертируем число в тип char
  return DATA_RESULT;                                                     //Отдаём результат
}


bool SELECT_LEFT        = true;                                           //Объявляем переменную статуса экрана
void LEFT() {                                                             //Функция переворачивает экран
  if (BUTTON(PIN_D4))                                                     //При нажатии клавиши S3 мереворачиваем экран
    if (SELECT_LEFT)                                                      //Определяем текущее положение экрана
    {
      TFT.setRotation(2);                                                 //Отображаем экран вверх ногами
      SELECT_LEFT        = false;                                         //Устанавливаем переменную в значение экран вверх ногами
    }
    else
    {
      TFT.setRotation(0);                                                 //Отображаем экран в штатном режиме
      SELECT_LEFT        = true;                                          //Устанавливаем переменную в значение экран в штатном режиме
    }
}




bool SPEAKER        = true;                                             //Объявляем переменную статуса спикера
void FIND()                                                             //Функция прозвонки
{
  VOLT            =  analogRead(PIN_A0) * DEFAULT_VOLT / 1024.0f * (( R1 + R2 ) / R2);  //Измеряем напряжение на пине PIN_A0
  if (VOLT == 0)                                                        //Если напряжение 0, то отключаем спикер и светодиод
  {
    analogWrite(PIN_D6, LOW);                                           
    analogWrite(PIN_D10, LOW);
    analogWrite(PIN_D11, LOW);
  }

  if (BUTTON(PIN_D3))                                                   //Если нажата кнопка S2, то отключаем звук
  {
      if (!SPEAKER)                                                     //Узнаём статус спикера
        SPEAKER = true;                                                 //Устанавливаем переменную в значение спикер включён
      else
      {
        SPEAKER = false;                                                //Устанавливаем переменную в значение спикер отключён
        analogWrite(PIN_D6, LOW);                                       //Отключаем спикер
      }
  }
  VOLT = 0;                                                             //Обнуляем переменную VOLT
  for (byte i = 0; i < LENGTH; i++)                                     //Делаем несколько измерений напряжения
  {
    VOLT              +=  analogRead(PIN_A0);
  }
  VOLT = VOLT / LENGTH;                                                 //Получаем усреднённое значение

  float VOLT_DISPLAY  = VOLT * DEFAULT_VOLT / 1024.0f * (( R1 + R2 ) / R2); //Получаем значение напряжения 

  TFT.setTextSize(3, 4);                                                //Устанавливаем размер шрифта
  TFT.setCursor(10, 18);                                                //Устанавливаем позицию текста
  if (VOLT_DISPLAY < 0.4)                                               //Если напряжение меньше 0.4 вольта, то считаем, что прозваниваем массу 
  {
    analogWrite(PIN_D5, LOW);                                           //Отключаем реле
    analogWrite(PIN_D9, 255);                                           //Включаем генератор сигнала
    
    if (VOLT_DISPLAY  > 0.1)                                            //Перед нами масса
    {
      TFT.setCursor(30, 18);                                            //Устанавливаем позицию текста
      TFT.print("MASS");                                                //Выводим на экран текст
      if (SPEAKER)                                                      //Если спикер включён, но подаём на него сигнал
        analogWrite(PIN_D6, HIGH);
      
      analogWrite(PIN_D10, HIGH);                                       //Зажигаем зелёный светодиод
      analogWrite(PIN_D11, LOW);                                        //Отрубаем красный светодиод
    }
    else
      TFT.print(strcat(DISPLAY_TEXT(VOLT_DISPLAY, 5, 2), "V"));         //Иначе выводим значение напряжения
  }
  else                                                                  //Иначе выводим напряжение на экран
  {
    analogWrite(PIN_D5, 255);                                           //Включаем реле
    analogWrite(PIN_D9, LOW);                                           //Отключаем генератор сигнала
    if (SPEAKER && VOLT_DISPLAY > 0.1)                                  //Если спикер включён сигнал на него поступает через Герц
      if (MAX_VOLT % 2 == 0)
        analogWrite(PIN_D6, HIGH);                                      //Подаём сигнал на спикер
      else
        analogWrite(PIN_D6, LOW);                                       //Подаём сигнал на спикер

    analogWrite(PIN_D10, LOW);                                          //Отрубаем зелёный светодиод
    analogWrite(PIN_D11, HIGH);                                         //Отрубаем красный светодиод


    TFT.print(strcat(DISPLAY_TEXT(VOLT_DISPLAY, 5, 2), "V"));           //Выводим значение напряжения
    ++MAX_VOLT;                                                         //Считаем Герцы
    if (MAX_VOLT >= 50)                                                 //Если больше 50, обнуляем, что бы не переполнить стек
      MAX_VOLT = 0;
  }
}




unsigned long SIGNAL  = 0;                                              //Объявляем переменную для измерения времени сигнала
bool SIGNAL_PULSE     = false;                                          //Устанавливаем переменную в значение поиск сигнала
void DISPLAY_FREQUENCY()                                                //Функция поиска CAN шины и измерения частоты
{
  for (byte i = 0; i < LENGTH; i++)                                     //Делаем несколько измерений
  {
    VOLTS[i]  = analogRead(PIN_A0);                                     //Получаем значения
    MAX_VOLT  = max(MAX_VOLT,  VOLTS[i]);                               //Находим максимальное значение
    VOLT      +=  VOLTS[i];                                             //Суммируем полученное напряжение
  }

  VOLT                    = VOLT / LENGTH;                              //Получаем усреднённое значение напряжения
  float DISPLAY_MAX_VOLT  = MAX_VOLT * DEFAULT_VOLT / 1024.0f * (( R1 + R2 ) / R2); //Высчитываем максимальное напряжение
  float DISPLAY_VOLT      = VOLT * DEFAULT_VOLT / 1024.0f * (( R1 + R2 ) / R2); //Высчитываем текущее напряжение

  byte FILL   = DISPLAY_VOLT * 100 /  DISPLAY_MAX_VOLT;                 //Получаем процент скважности сигнала
  
  if (DISPLAY_VOLT == 0)                                                //Если текущее напряжение равно нулю, то и обнуляем максимальное напряжение
    MAX_VOLT = 0; 

  TFT.setTextSize(3, 4);                                                //Устанавливаем размер шрифта
  TFT.setCursor(10, 18);                                                //Устанавливаем позицию текста
  TFT.print(strcat(DISPLAY_TEXT(DISPLAY_VOLT, 5, 2), "V"));             //Выводим данные на экран

  TFT.setTextSize(1,2);                                                 //Устанавливаем размер шрифта
  TFT.setCursor(0, 50);                                                 //Устанавливаем позицию текста
  TFT.print(strcat(DISPLAY_TEXT(FILL, 3, 0), "%"));                     //Выводим данные на экран
  
 
  TFT.setCursor(50, 50);                                                //Устанавливаем позицию текста
  if (SIGNAL_PULSE)                                                     //Определяем режим поиска сигнала
    SIGNAL  = 1000000 / (pulseIn(PIN_A1, HIGH) + pulseIn(PIN_A1, LOW)); //Если сигнал найден получаем его точное значение
  else
    SIGNAL  = 1000000 / (pulseIn(PIN_A1, HIGH, 1000) + pulseIn(PIN_A1, LOW, 1000)); //Если сигнал не найден ищем его с интервалом в 1 секунду
  
  if (SIGNAL >= 100000)                                                 //Если сингал не найден
  {
    TFT.setCursor(50, 50);                                              //Устанавливаем позицию текста
    TFT.print("NULL");                                                  //Выводим данные на экран
    SIGNAL_PULSE = false;                                               //Устанавливаем переменную в значение поиск сигнала
    analogWrite(PIN_D6, LOW);
  }
  else if (SIGNAL > 1000)                                               //Если сингал найден
  {
    TFT.print(strcat(DISPLAY_TEXT((SIGNAL / 1000), 3, 0),"kHz"));       //Выводим данные на экран
    SIGNAL_PULSE = true;                                                //Устанавливаем переменную в значение исследования сигнала
  }
  else
  {
    TFT.print(strcat(DISPLAY_TEXT(SIGNAL, 3, 0),"Hz"));                 //Выводим данные на экран
    SIGNAL_PULSE = true;                                                //Устанавливаем переменную в значение исследования сигнала
  }

  TFT.setTextSize(2, 2);                                                //Устанавливаем размер шрифта
  TFT.setCursor(40, 0);                                                 //Устанавливаем позицию текста
  
  TFT.print("CAN ");                                                    //Выводим данные на экран
  if (SIGNAL_PULSE)                                                     //Если на проводе присутствует сигнал
  {
    if (DISPLAY_VOLT > 2.6)                                             //Если напряжение выше 2.6 вольт, то перед нами CAN HIGH иначе CAN LOW
      TFT.print("H");                                                   //Выводим данные на экран
    else
      TFT.print("L");                                                   //Выводим данные на экран
  }

  TFT.setCursor(90, 50);                                                //Устанавливаем позицию текста
  TFT.print(strcat(DISPLAY_TEXT(DISPLAY_MAX_VOLT, 3, 2), "V"));         //Выводим данные на экран
  TFT.setTextSize(1);                                                   //Устанавливаем размер текста
}


                                                                        //Осциллограф
                                                                        //В тексте я сказал, что осциллограф похож на работу зарубежного блогера inventor, однако при комментировании кода выяснил, что он спёр эту работу у другого блогера CiferTech
                                                                        //Прежде чем обвинять меня в том, что я позаимствовал код у них, сравните код и работу осциллографа, всё что перенял проект, это схожий стиль и дизайн, так как он мне очень понравился.
byte SELECT_TIME  = 0;                                                  //Устанавливаем переменную в значение режима исследования сигнала 0
                                                                        //Здесь посложнее, для увеличения FPS я решил вместо вычисления напряжения высчитать значение для напряжений:
                                                                        //50В. - 1023
                                                                        //40В. - 819
                                                                        //30В. - 614
                                                                        //20В. - 410
                                                                        //10В. - 204
                                                                        //5В. - 103
                                                                        //2В. - 41
                                                                        //1В. - 21
                                                                        //Однако данные верны для питания Arduino в 5В. Если напряжение ниже 5В. (4.2В. и 3.7В. для батареек), то необходимо пересчитать значения с помощью пропорции
const int VOLT_LINE[9] = {21, 41, 103, 205, 410, 614, 819, 1023};       //Максимальноя значение для графиков
const char TIME_LINE[8][6] = {"200us", "500us", "  1ms", "  2ms", "  5ms", " 10ms", " 20ms", " 50ms"};  //Режимы измерения
void SELECT_BUTTOM_TIME() {                                             //Функция получения данных для графика
  if (BUTTON(PIN_D3))                                                   //Нажатием кнопки переключаем режимы исследования сигнала
  {
      ++SELECT_TIME;                                                    //Переключаем на следующий режим
      if (SELECT_TIME > 7)                                              //После режима 6 снова переходим на режим 0
        SELECT_TIME = 0;
  }
  
  if (SELECT_TIME == 0)                                                 //Выбор режима
  {  
    ADCSRA = ADCSRA & 0xf8;                                             //очищает младшие 3 бита перед операцией
    ADCSRA = ADCSRA | 0x02;                                             //Устанавливаем предделитель 4 (0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
    for (byte i = 0; i < LENGTH; i++)                                   //Делаем несколько измерений напряжения
    {
      VOLTS[i]    = analogRead(PIN_A0);                                 //Получаем значения
      MAX_VOLT    = max(MAX_VOLT,  VOLTS[i]);                           //Находим максимальное значение
      VOLT        +=  VOLTS[i];                                         //Суммируем полученное напряжение
      asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); //Обеспечивает микрозадержку
      asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); //Обеспечивает микрозадержку
      asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); //Обеспечивает микрозадержку
    }
  } 
  else if (SELECT_TIME == 1)                                            //Выбор режима
  {
    ADCSRA = ADCSRA & 0xf8;                                             //очищает младшие 3 бита перед операцией
    ADCSRA = ADCSRA | 0x04;                                             //Устанавливаем предделитель 16 (0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
    for (byte i = 0; i < LENGTH; i++)                                   //Делаем несколько измерений напряжения
    {    
      VOLTS[i]    = analogRead(PIN_A0);                                 //Получаем значения
      MAX_VOLT    = max(MAX_VOLT,  VOLTS[i]);                           //Находим максимальное значение
      VOLT        +=  VOLTS[i];                                         //Суммируем полученное напряжение
      delayMicroseconds(4);                                             //Задержка в 4 микросекунды
      asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); //Обеспечивает микрозадержку
      asm("nop"); asm("nop"); asm("nop");                                                                                     //Обеспечивает микрозадержку
    }
  }
  else if (SELECT_TIME == 2)                                            //Выбор режима
  {
    ADCSRA = ADCSRA & 0xf8;                                             //очищает младшие 3 бита перед операцией
    ADCSRA = ADCSRA | 0x05;                                             //Устанавливаем предделитель 32 (0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
    for (byte i = 0; i < LENGTH; i++)                                   //Делаем несколько измерений напряжения
    {   
      VOLTS[i]    = analogRead(PIN_A0);                                 //Получаем значения
      MAX_VOLT    = max(MAX_VOLT,  VOLTS[i]);                           //Находим максимальное значение
      VOLT        +=  VOLTS[i];                                         //Суммируем полученное напряжение
      delayMicroseconds(12);                                            //Задержка в 12 микросекунд
    }
  }
  else if (SELECT_TIME == 3)                                            //Выбор режима
  {
    ADCSRA = ADCSRA & 0xf8;                                             //очищает младшие 3 бита перед операцией
    ADCSRA = ADCSRA | 0x06;                                             //Устанавливаем предделитель 64 (0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
    for (byte i = 0; i < LENGTH; i++)                                   //Делаем несколько измерений напряжения
    {    
      VOLTS[i]    = analogRead(PIN_A0);                                 //Получаем значения
      VOLT        +=  VOLTS[i];                                         //Суммируем полученное напряжение
      MAX_VOLT    = max(MAX_VOLT,  VOLTS[i]);                           //Находим максимальное значение
      delayMicroseconds(24);                                            //Задержка в 24 микросекунды
    }
  }
  else if (SELECT_TIME == 4)                                            //Выбор режима
  {
    ADCSRA = ADCSRA & 0xf8;                                             //очищает младшие 3 бита перед операцией
    ADCSRA = ADCSRA | 0x07;                                             //Устанавливаем предделитель 128 (0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
    for (byte i = 0; i < LENGTH; i++)                                   //Делаем несколько измерений напряжения
    {
      VOLTS[i]    = analogRead(PIN_A0);                                 //Получаем значения
      VOLT        +=  VOLTS[i];                                         //Суммируем полученное напряжение
      MAX_VOLT    = max(MAX_VOLT,  VOLTS[i]);                           //Находим максимальное значение
      delayMicroseconds(88);                                            //Задержка в 88 микросекунд
    }
  }
  else if (SELECT_TIME == 5)                                            //Выбор режима
  {
    ADCSRA = ADCSRA & 0xf8;                                             //очищает младшие 3 бита перед операцией
    ADCSRA = ADCSRA | 0x07;                                             //Устанавливаем предделитель 128 (0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
    for (byte i = 0; i < LENGTH; i++)                                   //Делаем несколько измерений напряжения
    {     
      VOLTS[i]    = analogRead(PIN_A0);                                 //Получаем значения
      VOLT        +=  VOLTS[i];                                         //Суммируем полученное напряжение
      MAX_VOLT    = max(MAX_VOLT,  VOLTS[i]);                           //Находим максимальное значение
      delayMicroseconds(288);                                           //Задержка в 288 микросекунд
    }
  }
  else if (SELECT_TIME == 6)                                            //Выбор режима
  {
    ADCSRA = ADCSRA & 0xf8;                                             //очищает младшие 3 бита перед операцией
    ADCSRA = ADCSRA | 0x07;                                             //Устанавливаем предделитель 128 (0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
    for (byte i = 0; i < LENGTH; i++)                                   //Делаем несколько измерений напряжения
    {     
      VOLTS[i]    = analogRead(PIN_A0);                                 //Получаем значения
      VOLT        +=  VOLTS[i];                                         //Суммируем полученное напряжение
      MAX_VOLT    = max(MAX_VOLT,  VOLTS[i]);                           //Находим максимальное значение
      delayMicroseconds(688);                                           //Задержка в 688 микросекунд
    }
  }
  else                                                                  //Выбор режима
  {
    ADCSRA = ADCSRA & 0xf8;                                             //очищает младшие 3 бита перед операцией
    ADCSRA = ADCSRA | 0x07;                                             //Устанавливаем предделитель 128 (0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
    for (byte i = 0; i < LENGTH; i++)                                   //Делаем несколько измерений напряжения
    {    
      VOLTS[i]    = analogRead(PIN_A0);                                 //Получаем значения
      VOLT        +=  VOLTS[i];                                         //Суммируем полученное напряжение
      MAX_VOLT    = max(MAX_VOLT,  VOLTS[i]);                           //Находим максимальное значение
      delayMicroseconds(1888);                                          //Задержка в 1888 микросекунд
    }
  }
 

  VOLT                  = VOLT / LENGTH;                                //Получаем усреднённое значение напряжения
  byte COUNT            = sizeof(VOLT_LINE);                            //Определяем размер массива
  bool MAX_VOLT_SELECT  = true;                                         //Устанавливаем переменную в значение режима поиска самого подходящего режима измерения
  VOLT                  = VOLT * DEFAULT_VOLT / 1024.0f;                //Высчитываем напряжение
  if (VOLT == 0)                                                        //Если текущее напряжение равно нулю, то и обнуляем максимальное напряжение
    MAX_VOLT  = 0;

  for (byte i = 0; i < COUNT; ++i)                                      //Проходим по массиву исследования
    if (MAX_VOLT < VOLT_LINE[i] && MAX_VOLT_SELECT)                     //Находим самое подходящее значения для исследования
    {
      MAX_NUMBER      = VOLT_LINE[i];                                   //Сохраняем его
      MAX_VOLT_SELECT = false;                                          //Устанавливаем переменную в значение поиск самого подходящего режима завершён
    }

  TFT.setCursor(92, 0);                                                 //Устанавливаем позицию текста
  TFT.print(strcat(DISPLAY_TEXT(VOLT * (( R1 + R2 ) / R2), 5, 2), "V"));  //Выводим данные на экран

  TFT.setCursor(26, 0);                                                 //Устанавливаем позицию текста
  TFT.print(TIME_LINE[SELECT_TIME]);                                    //Выводим данные на экран
}



void DISPLAY_GRAPH() {                                                  //Рисуем график напряжения
  byte y1, y2, LENGH = sizeof(VOLTS);                                   //Объявляем переменные

  for (int x = 0; x <= LENGH + 1; ++x) {                                //Определяем длину графика
    y1 = map(VOLTS[x], 0, MAX_NUMBER, 63, 9);                           //Определяем значение точки Y1
    y2 = map(VOLTS[x + 1], 0, MAX_NUMBER, 63, 9);                       //Определяем значение точки Y2
    TFT.drawLine(x + 27, y1, x + 28, y2, COLOR_DRAW);                   //Рисуем линии
  }

  float DISPLAY_MAX_VOLT = MAX_NUMBER * 50 / 1023;                      //Получаем максимальное напряжение

                                                                        //Рисуем слева числа для сетки напряжения
  TFT.setCursor(0, 9);                                                  //Устанавливаем позицию текста
  TFT.print(DISPLAY_TEXT(DISPLAY_MAX_VOLT, 4, DISPLAY_MAX_VOLT >= 10 ? 1 : 2)); //Выводим максимальное значение напряжения

  TFT.setCursor(0, 33);                                                 //Устанавливаем позицию текста
  TFT.print(DISPLAY_TEXT(DISPLAY_MAX_VOLT / 2, 4, DISPLAY_MAX_VOLT / 2 >= 10 ? 1 : 2));   //Выводим среднее значение напряжения

  TFT.setCursor(0, 57);                                                 //Устанавливаем позицию текста
  TFT.print(0.0f);                                                      //Выводим 0
}


void BACKGROUND() {                                                     //Функция рисует сетку для осциллографа
  for (int x = 26; x <= TFT_WIDTH; x += 5) {                            //Определяем шаг в 5 пикселей между горизонтальными линиями
    TFT.drawFastHLine(x, 36, 2, COLOR_DRAW);
    if ((x - 1) % 25 == 0)                                              //Рисуем чёрточки раз в 25 пикселей (для красоты)
    {
      TFT.drawFastHLine((x - 1 == 25 ? x : (x - 1 == 125 ? x - 2 : x - 1)), 9, 3, COLOR_DRAW);  //Для красоты добавляем чёрточки сверху
      TFT.drawFastHLine((x - 1 == 25 ? x : (x - 1 == 125 ? x - 2 : x - 1)), 63, 3, COLOR_DRAW); //Для красоты добавляем чёрточки снизу
    }
  }
  for (int x = 26; x <= TFT_WIDTH; x += 25) {                           //Определяем шаг в 25 пикселей между вертикальными линиями
    for (int y = 10; y < 63; y += 5) {                                  //Определяем значение координаты Y
      TFT.drawFastVLine(x, y, 2, COLOR_DRAW);                           //Рисуем вертикальные линии
    }
  }
}
	

float GET_DEFAULT_VOLT() {                                             //Функция измеряет внутреннее напряжение Arduino
  float RESULT        = 0.0f;                                          //Определяем переменную для получения результата.
  byte  COUNT_RESULT  = 100;                                           //Определяем сколько значений АЦП требуется получить для усреднения результата.
  #if (defined( __AVR_ATmega640__ ) || defined( __AVR_ATmega1280__ ) || defined( __AVR_ATmega1281__ ) || defined( __AVR_ATmega2560__ ) || defined( __AVR_ATmega2561__ ) || defined( __AVR_ATmega32U4__ ) || defined( __AVR_ATmega32U4__ ))   //Устанавливаем Uвх АЦП с источника ИОН на 1,1В, а в качестве ИОН АЦП используем Vcc ИОН - источник опорного напряжения.
  ADCSRB &= ~(1<<MUX5);	                                               //	У микроконтроллера «ATmega328» действительно нет такого бита, но он зарерервирован для совместимости
  #endif
                                                                        //Для Arduino Mega, Leonardo и Micro, сбрасываем бит «MUX5» регистра «ADCSRB», так как «MUX[5-0]» должно быть равно 011110 (см. регистр «ADMUX»).
  ADMUX   = (0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(1<<4)|(1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(0<<MUX0);	//	Устанавливаем биты регистра «ADMUX»: «REFS»=01 (ИОН=VCC), «ADLAR»=0 (выравнивание результата по правому краю), «MUX[4-0]»=11110 или «MUX[3-0]»=1110 (источником сигнала для АЦП является напряжение ИОН на 1,1 В).                                                                   
  for(byte i=0; i<COUNT_RESULT; i++)                                    //Получаем несколько значений АЦП
  {
    ADCSRA |= (1<<ADEN )|(1<<ADSC );                                    //Запускаем преобразования АЦП:Устанавливаем биты регистра «ADCSRA»: «ADEN»=1  (вкл АЦП), «ADSC» =1 (запускаем новое преобразование).                                                                   	
    while(bit_is_set(ADCSRA,ADSC)){};                                   //Получаем данные АЦП:
    RESULT += ADC;													                            //Ждём завершения преобразования: о готовности результата свидетельствует сброс бита «ADSC» регистра «ADCSRA» и добавляем результат «ADC» к переменной «i».
  }	
  RESULT /= COUNT_RESULT;							                                  //Делим результат «i» на «j», так как мы получили его «j» раз.                                                            
  return (1.1f/RESULT) * 1024.0f;			                                  //Рассчитываем напряжение питания:	//	АЦП = (Uвх/Vcc)*1023.	Напряжение Uвх мы брали с внутреннего ИОН на 1.1 В, значение которого возвращает функция analogSave_1V1(0).
}




