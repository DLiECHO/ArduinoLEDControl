#include <Arduino.h>
#line 1 "d:\\2021上\\无线传感\\实验二\\code\\code.ino"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <TimerOne.h>
#include <SoftwareSerial.h>

SoftwareSerial BT(0, 1);
const int relayPin = 7;
const int irReceiverPin = 3;
const int led = 13;
String val;
int ledflag = 0;

IRrecv irrecv(irReceiverPin);
decode_results results;
LiquidCrystal_I2C lcd(0x27, 16, 2);

//int s = 0,m = 0,h = 0,w = 1;
int minute = 0, second = 0, hour = 0, week = 1; //当前时间
int flag = 0;
int ONminute = 25, ONsecond = 0, ONhour = 10, ONweek = 8;     //定时开灯时间
int OFFminute = 26, OFFsecond = 0, OFFhour = 10, OFFweek = 8; //定时关灯时间

void led_OFF();
void led_ON();
void Display();
void check();
void DisplayCursor(int rol, int row);
void Set_Clock();
#line 30 "d:\\2021上\\无线传感\\实验二\\code\\code.ino"
void setup();
#line 44 "d:\\2021上\\无线传感\\实验二\\code\\code.ino"
void loop();
#line 91 "d:\\2021上\\无线传感\\实验二\\code\\code.ino"
void showLed();
#line 106 "d:\\2021上\\无线传感\\实验二\\code\\code.ino"
void set(int h, int m, int s, int w);
#line 304 "d:\\2021上\\无线传感\\实验二\\code\\code.ino"
void FormatDisplay(int col, int row, int num);
#line 315 "d:\\2021上\\无线传感\\实验二\\code\\code.ino"
void showWeek(int w);
#line 381 "d:\\2021上\\无线传感\\实验二\\code\\code.ino"
void Display_ON_Time();
#line 392 "d:\\2021上\\无线传感\\实验二\\code\\code.ino"
void Display_OFF_Time();
#line 403 "d:\\2021上\\无线传感\\实验二\\code\\code.ino"
void olock();
#line 416 "d:\\2021上\\无线传感\\实验二\\code\\code.ino"
void timerIsr();
#line 420 "d:\\2021上\\无线传感\\实验二\\code\\code.ino"
void blink();
#line 30 "d:\\2021上\\无线传感\\实验二\\code\\code.ino"
void setup()
{
    lcd.init();
    lcd.backlight();
    pinMode(relayPin, OUTPUT);
    Serial.begin(9600);
    Serial.println("BTLED is ready!");
    BT.begin(9600);
    pinMode(led, OUTPUT);
    irrecv.enableIRIn();
    Timer1.initialize(1000000);       //初始化定时器为1s
    Timer1.attachInterrupt(timerIsr); //设置中断回调函数
    set(10, 24, 0, 1);
}
void loop()
{
    led_OFF();
    delay(1000);
    led_ON();
    delay(1000);
    led_OFF();
    Serial.println("command tips: ST(set now time), ON(set ON time), OFF(set OFF time)");
    Serial.println("input example: ON 7 23 59");
    while (1)
    {
        Display();
        Set_Clock();
        olock();
    }
}
void check()
{
    if (irrecv.decode(&results))
    {
        //Serial.print("irCode: ");
        //Serial.println(results.value, HEX);
        irrecv.resume();
    }
    if (BT.available())
    {
        val = BT.readString();
        Serial.print(val);
    }
    delay(50);
}
void led_OFF()
{
    digitalWrite(relayPin, LOW);
    lcd.setCursor(0, 0);
    lcd.print("LED: OFF          ");
    val = "";
    ledflag = 0;
}
void led_ON()
{
    digitalWrite(relayPin, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("LED: ON            ");
    val = "";
    ledflag = 1;
}
void showLed()
{
    if (ledflag == 0)
    {
        lcd.setCursor(0, 0);
        lcd.print("LED: OFF          ");
    }
    else
    {
        lcd.setCursor(0, 0);
        lcd.print("LED: ON            ");
    }
}

//设置初始时间
void set(int h, int m, int s, int w)
{
    hour = h;
    minute = m;
    second = s;
    week = w;
}

void Set_Clock()
{
    check();
    delay(50);

    if (val.startsWith("ST"))
    {
        //格式：ST 7 23 59
        week = val.charAt(3) - 48;
        hour = (val.charAt(5) - 48) * 10 + (val.charAt(6) - 48);
        minute = (val.charAt(8) - 48) * 10 + (val.charAt(9) - 48);
        second = (val.charAt(11) - 48) * 10 + (val.charAt(12) - 48);
        Display();
        val = "";
    }
    else if (val.startsWith("ON"))
    {
        //格式：ST 7 23 59
        ONweek = val.charAt(3) - 48;
        ONhour = (val.charAt(5) - 48) * 10 + (val.charAt(6) - 48);
        ONminute = (val.charAt(8) - 48) * 10 + (val.charAt(9) - 48);
        Display_ON_Time();
        lcd.setCursor(0, 0);
        lcd.print("Led light time");
        delay(2000);
        showLed();
        val = "";
    }
    else if (val.startsWith("OFF"))
    {
        //格式：ST 7 23 59
        OFFweek = val.charAt(4) - 48;
        OFFhour = (val.charAt(6) - 48) * 10 + (val.charAt(7) - 48);
        OFFminute = (val.charAt(9) - 48) * 10 + (val.charAt(10) - 48);
        Display_OFF_Time();
        lcd.setCursor(0, 0);
        lcd.print("Led quench time");
        delay(2000);
        showLed();
        val = "";
    }
    else if (val == "0")
    {
        digitalWrite(led, LOW);
        led_OFF();
        val = "";
    }
    else if (val == "1")
    {
        digitalWrite(led, HIGH);
        led_ON();
        val = "";
    }
    else if (val.startsWith("Blink"))
    {
        int num = val.charAt(5)-48;
        while (num)
        {
            Serial.println(num);
            blink();
            num = num - 1;
        }
    }
    else if (val.startsWith("key")){
        String key = val.substring(3);
        while (1)
        {
            led_OFF();
            val = BT.readString();
            if (val == key)
            {
                led_ON();
                break;
            }
        }
        
    }

    if (results.value == 0XFF22DD)
    {
        lcd.setCursor(9, 1);
        lcd.print("SetTime");
        Display();
        flag = 1;
    }
    else if (results.value == 0XFF30CF && flag == 1)
    {
        hour++;
        hour %= 24;
        Display();
        results.value = 0;
    }
    else if (results.value == 0XFF18E7 && flag == 1)
    {
        minute++;
        if (minute > 59)
        {
            minute = 0;
            hour++;
            hour %= 24;
        }
        Display();
        results.value = 0;
    }
    else if (results.value == 0XFF7A85 && flag == 1)
    {
        second = 0;
        Display();
        results.value = 0;
    }
    else if (results.value == 0XFFC23D)
    {
        lcd.setCursor(9, 1);
        lcd.print("Set_ON_T");
        Display_ON_Time();
        flag = 2;
    }
    else if (results.value == 0XFF30CF && flag == 2)
    {
        ONhour++;
        ONhour %= 24;
        results.value = 0;
    }
    else if (results.value == 0XFF18E7 && flag == 2)
    {
        ONminute++;
        ONminute %= 60;
        results.value = 0;
    }
    else if (results.value == 0XFF7A85 && flag == 2)
    {
        ONsecond++;
        ONsecond %= 60;
        results.value = 0;
    }
    else if (results.value == 0XFF02FD)
    {
        lcd.setCursor(9, 1);
        lcd.print("Set_OFF_T");
        Display_OFF_Time();
        flag = 3;
    }
    else if (results.value == 0XFF30CF && flag == 3)
    {
        OFFhour++;
        OFFhour %= 24;
        results.value = 0;
    }
    else if (results.value == 0XFF18E7 && flag == 3)
    {
        OFFminute++;
        OFFminute %= 60;
        results.value = 0;
    }
    else if (results.value == 0XFF7A85 && flag == 3)
    {
        OFFsecond++;
        ONsecond %= 60;
        results.value = 0;
    }
    else if (results.value == 0XFF629D)
    {
        led_ON();
        lcd.setCursor(9, 1);
        lcd.print("          ");
        results.value = 0;
        flag = 0;
    }
    else if (results.value == 0XFFE21D)
    {
        led_OFF();
        lcd.setCursor(9, 1);
        lcd.print("           ");
        results.value = 0;
        flag = 0;
    }
    else if (results.value == 0XFF10EF)
    {
        blink();
    }
    if (flag == 1 || flag == 0)
        //Display();
        true;
    else if (flag == 2)
        Display_ON_Time();
    else if (flag == 3)
        Display_OFF_Time();
}

// 格式化输出
void FormatDisplay(int col, int row, int num)
{
    lcd.setCursor(col, row);
    if (num < 10)
    {
        lcd.print("0");
    }
    lcd.print(num);
}

//格式化输出星期值
void showWeek(int w)
{
    switch (w)
    {
    case 1:
        lcd.setCursor(9, 1);
        lcd.print("Mon.  ");
        break;
    case 2:
        lcd.setCursor(9, 1);
        lcd.print("Tues.  ");
        break;
    case 3:
        lcd.setCursor(9, 1);
        lcd.print("Wed.  ");
        break;
    case 4:
        lcd.setCursor(9, 1);
        lcd.print("Thur.  ");
        break;
    case 5:
        lcd.setCursor(9, 1);
        lcd.print("Fri.  ");
        break;
    case 6:
        lcd.setCursor(9, 1);
        lcd.print("Sat.  ");
        break;
    case 7:
        lcd.setCursor(9, 1);
        lcd.print("Sun.  ");
        break;
    default:
        lcd.setCursor(9, 1);
        lcd.print("Error!");
        break;
    }
}

void Display()
{
    if (second > 59)
    {
        second = 0;
        minute++;
        if (minute > 59)
        {
            minute = 0;
            hour++;
            if (hour > 23)
            {
                hour = 0;
                week++;
                week %= 7;
            }
        }
    }
    FormatDisplay(6, 1, second);
    FormatDisplay(3, 1, minute);
    FormatDisplay(0, 1, hour);
    lcd.setCursor(2, 1);
    lcd.print(":");
    lcd.setCursor(5, 1);
    lcd.print(":");
    showWeek(week);
}
void Display_ON_Time()
{
    FormatDisplay(6, 1, ONsecond);
    FormatDisplay(3, 1, ONminute);
    FormatDisplay(0, 1, ONhour);
    lcd.setCursor(2, 1);
    lcd.print(":");
    lcd.setCursor(5, 1);
    lcd.print(":");
    showWeek(ONweek);
}
void Display_OFF_Time()
{
    FormatDisplay(6, 1, OFFsecond);
    FormatDisplay(3, 1, OFFminute);
    FormatDisplay(0, 1, OFFhour);
    lcd.setCursor(2, 1);
    lcd.print(":");
    lcd.setCursor(5, 1);
    lcd.print(":");
    showWeek(OFFweek);
}
void olock()
{
    if (second == ONsecond && minute == ONminute && ONhour == hour && ONweek == week)
    {
        //Serial.println("Light ON");
        led_ON();
    }
    if (second == OFFsecond && minute == OFFminute && OFFhour == hour && OFFweek == week)
    {
        //Serial.println("Light OFF");
        led_OFF();
    }
}
void timerIsr() //定时器中断处理函数
{
    second++;
}
void blink()
{
    led_ON();
    delay(250);
    led_OFF();
    delay(250);
}
