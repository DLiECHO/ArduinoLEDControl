#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <TimerOne.h>
#include <SoftwareSerial.h>

SoftwareSerial BT(0, 1);
const int relayPin = 7;
const int irReceiverPin = 3;
//const int led = 13;
String val;
int ledflag = 0;
int cursorC = 0;
int cursorR = 1;

IRrecv irrecv(irReceiverPin);
decode_results results;
LiquidCrystal_I2C lcd(0x27, 16, 2);

//int s = 0,m = 0,h = 0,w = 1;
int minute = 0, second = 0, hour = 0, week = 0; //当前时间
//int flag = 0;
int ONminute = 25, ONsecond = 0, ONhour = 10, ONweek = 7;     //定时开灯时间
int OFFminute = 26, OFFsecond = 0, OFFhour = 10, OFFweek = 7; //定时关灯时间

void led_OFF();
void led_ON();
void Display();
void check();
void DisplayCursor(int rol, int row);
void Set_Clock();
void setup()
{
    lcd.init();
    lcd.backlight();
    pinMode(relayPin, OUTPUT);
    Serial.begin(9600);
    Serial.println("BTLED is ready!");
    BT.begin(9600);
    //pinMode(led, OUTPUT);
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
    Serial.println("command tips 1: ST(set now time), ON(set ON time), OFF(set OFF time)");
    Serial.println("input example: ON 7 23 59");
    Serial.println("command tips 2: key(set the key to light LED)");
    Serial.println("input example: key123456");
    Serial.println("command tips 3: Blink(let LED flash within ten times and protect the LED)");
    Serial.println("input example: Blink9");
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
    Serial.println("  Turn off LED successfully!");
    val = "";
    ledflag = 0;
}
void led_ON()
{
    digitalWrite(relayPin, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("LED: ON            ");
    Serial.println("  Light LED successfully!");
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

void timeAdd()
{
    if (second > 59)
    {
        second %= 60;
        minute = minute + 1;
    }
    if (second < 0)
    {
        minute = minute - 1;
        second = 60 + second;
    }
    if (minute > 59)
    {
        minute %= 60;
        hour = hour + 1;
    }
    if (minute < 0)
    {
        hour = hour - 1;
        minute = 60 + minute;
    }
    if (hour > 23)
    {
        hour %= 24;
        week = week + 1;
    }
    if (hour < 0)
    {
        week = week - 1;
        hour = 24 + hour;
    }
    if (week < 0)
    {
        week = 7 + week;
    }
    week %= 7;

    if (ONsecond > 59)
    {
        ONsecond %= 60;
        ONminute = ONminute + 1;
    }
    if (ONsecond < 0)
    {
        ONminute = ONminute - 1;
        ONsecond = 60 + ONsecond;
    }
    if (ONminute > 59)
    {
        ONminute %= 60;
        ONhour = ONhour + 1;
    }
    if (ONminute < 0)
    {
        ONhour = ONhour - 1;
        ONminute = 60 + ONminute;
    }
    if (ONhour > 23)
    {
        ONhour %= 24;
        ONweek = ONweek + 1;
    }
    if (ONhour < 0)
    {
        ONweek = ONweek - 1;
        ONhour = ONhour + 24;
    }
    if (ONweek < 0)
    {
        ONweek = 7 + ONweek;
    }
    ONweek %= 7;

    if (OFFsecond > 59)
    {
        OFFsecond %= 60;
        OFFminute = OFFminute + 1;
    }
    if (OFFsecond < 0)
    {
        OFFminute = OFFminute - 1;
        OFFsecond = OFFsecond + 60;
    }
    if (OFFminute > 59)
    {
        OFFminute %= 60;
        OFFhour = OFFhour + 1;
    }
    if (OFFminute < 0)
    {
        OFFhour = OFFhour - 1;
        OFFminute = OFFminute + 60;
    }
    if (OFFhour > 23)
    {
        OFFhour %= 24;
        OFFweek = OFFweek + 1;
    }
    if (OFFhour < 0)
    {
        OFFweek = OFFweek - 1;
        OFFhour = OFFhour + 24;
    }
    if (OFFweek < 0)
    {
        OFFweek = 7 + OFFweek;
    }
    OFFweek %= 7;
}

void buttonSetNOW(String s)
{
    if (s == "+1")
    {
        switch (cursorC)
        {
        case 0:
            hour = hour + 10;
            timeAdd();
            Display();
            break;
        case 1:
            hour = hour + 1;
            timeAdd();
            Display();
            break;
        case 3:
            minute = minute + 10;
            timeAdd();
            break;
        case 4:
            minute = minute + 1;
            timeAdd();
            break;
        case 6:
            second = second + 10;
            timeAdd();
            break;
        case 7:
            second = second + 1;
            timeAdd();
            break;
        case 9:
            week = week + 1;
            timeAdd();
            break;
        case 10:
            week = week + 1;
            timeAdd();
            break;
        case 11:
            week = week + 1;
            timeAdd();
            break;
        case 12:
            week = week + 1;
            timeAdd();
            break;
        default:
            lcd.setCursor(9, 1);
            lcd.print("Error!");
            break;
        }
    }

    if (s == "-1")
    {
        switch (cursorC)
        {
        case 0:
            hour = hour - 10;
            timeAdd();
            Display();
            break;
        case 1:
            hour = hour - 1;
            timeAdd();
            Display();
            break;
        case 3:
            minute = minute - 10;
            timeAdd();
            break;
        case 4:
            minute = minute - 1;
            timeAdd();
            break;
        case 6:
            second = second - 10;
            timeAdd();
            break;
        case 7:
            second = second - 1;
            timeAdd();
            break;
        case 9:
            week = week - 1;
            timeAdd();
            break;
        case 10:
            week = week - 1;
            timeAdd();
            break;
        case 11:
            week = week - 1;
            timeAdd();
            break;
        case 12:
            week = week - 1;
            timeAdd();
            break;
        default:
            lcd.setCursor(9, 1);
            lcd.print("Error!");
            break;
        }
    }
}

void buttonSetON(String s)
{
    if (s == "+1")
    {
        switch (cursorC)
        {
        case 0:
            ONhour = ONhour + 10;
            timeAdd();
            Display();
            break;
        case 1:
            ONhour = ONhour + 1;
            timeAdd();
            Display();
            break;
        case 3:
            ONminute = ONminute + 10;
            timeAdd();
            break;
        case 4:
            ONminute = ONminute + 1;
            timeAdd();
            break;
        case 6:
            ONsecond = ONsecond + 10;
            timeAdd();
            break;
        case 7:
            ONsecond = ONsecond + 1;
            timeAdd();
            break;
        case 9:
            ONweek = ONweek + 1;
            timeAdd();
            break;
        case 10:
            ONweek = ONweek + 1;
            timeAdd();
            break;
        case 11:
            ONweek = ONweek + 1;
            timeAdd();
            break;
        case 12:
            ONweek = ONweek + 1;
            timeAdd();
            break;
        default:
            lcd.setCursor(9, 1);
            lcd.print("Error!");
            break;
        }
    }

    if (s == "-1")
    {
        switch (cursorC)
        {
        case 0:
            ONhour = ONhour - 10;
            timeAdd();
            Display();
            break;
        case 1:
            ONhour = ONhour - 1;
            timeAdd();
            Display();
            break;
        case 3:
            ONminute = ONminute - 10;
            timeAdd();
            break;
        case 4:
            ONminute = ONminute - 1;
            timeAdd();
            break;
        case 6:
            ONsecond = ONsecond - 10;
            timeAdd();
            break;
        case 7:
            ONsecond = ONsecond - 1;
            timeAdd();
            break;
        case 9:
            ONweek = ONweek - 1;
            timeAdd();
            break;
        case 10:
            ONweek = ONweek - 1;
            timeAdd();
            break;
        case 11:
            ONweek = ONweek - 1;
            timeAdd();
            break;
        case 12:
            ONweek = ONweek - 1;
            timeAdd();
            break;
        default:
            lcd.setCursor(9, 1);
            lcd.print("Error!");
            break;
        }
    }
}

void buttonSetOFF(String s)
{
    if (s == "+1")
    {
        switch (cursorC)
        {
        case 0:
            OFFhour = OFFhour + 10;
            timeAdd();
            Display();
            break;
        case 1:
            OFFhour = OFFhour + 1;
            timeAdd();
            Display();
            break;
        case 3:
            OFFminute = OFFminute + 10;
            timeAdd();
            break;
        case 4:
            OFFminute = OFFminute + 1;
            timeAdd();
            break;
        case 6:
            OFFsecond = OFFsecond + 10;
            timeAdd();
            break;
        case 7:
            OFFsecond = OFFsecond + 1;
            timeAdd();
            break;
        case 9:
            OFFweek = OFFweek + 1;
            timeAdd();
            break;
        case 10:
            OFFweek = OFFweek + 1;
            timeAdd();
            break;
        case 11:
            OFFweek = OFFweek + 1;
            timeAdd();
            break;
        case 12:
            OFFweek = OFFweek + 1;
            timeAdd();
            break;
        default:
            lcd.setCursor(9, 1);
            lcd.print("Error!");
            break;
        }
    }

    if (s == "-1")
    {
        switch (cursorC)
        {
        case 0:
            OFFhour = OFFhour - 10;
            timeAdd();
            Display();
            break;
        case 1:
            OFFhour = OFFhour - 1;
            timeAdd();
            Display();
            break;
        case 3:
            OFFminute = OFFminute - 10;
            timeAdd();
            break;
        case 4:
            OFFminute = OFFminute - 1;
            timeAdd();
            break;
        case 6:
            OFFsecond = OFFsecond - 10;
            timeAdd();
            break;
        case 7:
            OFFsecond = OFFsecond - 1;
            timeAdd();
            break;
        case 9:
            OFFweek = OFFweek - 1;
            timeAdd();
            break;
        case 10:
            OFFweek = OFFweek - 1;
            timeAdd();
            break;
        case 11:
            OFFweek = OFFweek - 1;
            timeAdd();
            break;
        case 12:
            OFFweek = OFFweek - 1;
            timeAdd();
            break;
        default:
            lcd.setCursor(9, 1);
            lcd.print("Error!");
            break;
        }
    }
}

void Set_Clock()
{
    check();
    delay(50);

    if (val.startsWith("ST"))
    {
        if (val == "STBT")
        {
            lcd.setCursor(0, 0);
            lcd.print("Set now time         ");
            lcd.setCursor(cursorC, cursorR);
            lcd.cursor();
            while (1)
            {
                val = BT.readString();
                Serial.println(val);
                if (val == "start")
                {
                    //Serial.println(val);
                    val = "";
                    showLed();
                    Display();
                    lcd.noCursor();
                    //Serial.println(week);
                    break;
                }
                else if (val == "cleft")
                {
                    val = "";
                    cursorC = cursorC - 1;
                    if (cursorC < 0)
                    {
                        cursorC = 16 + cursorC;
                    }
                    lcd.setCursor(cursorC, cursorR);
                    lcd.cursor();
                    Serial.print("cursorC: ");
                    Serial.println(cursorC);
                }
                else if (val == "cright")
                {
                    val = "";
                    cursorC = cursorC + 1;
                    if (cursorC > 15)
                    {
                        cursorC = cursorC - 16;
                    }
                    lcd.setCursor(cursorC, cursorR);
                    lcd.cursor();
                    Serial.print("cursorC: ");
                    Serial.println(cursorC);
                }
                else if (val == "-1" || val == "+1")
                {
                    buttonSetNOW(val);
                    val = "";
                }
                //Serial.println(week);
                Display();
                lcd.setCursor(cursorC, cursorR);
                lcd.cursor();
            }
        }
//        else
//        {
//            //格式：ST 7 23 59
//            week = val.charAt(3) - 48;
//            hour = (val.charAt(5) - 48) * 10 + (val.charAt(6) - 48);
//            minute = (val.charAt(8) - 48) * 10 + (val.charAt(9) - 48);
//            second = (val.charAt(11) - 48) * 10 + (val.charAt(12) - 48);
//            Display();
//            val = "";
//        }
    }
    else if (val.startsWith("ON"))
    {
        if (val == "ONBT")
        {
            Display_ON_Time();
            lcd.setCursor(0, 0);
            lcd.print("Set ON time         ");
            lcd.setCursor(cursorC, cursorR);
            lcd.cursor();
            while (1)
            {
                val = BT.readString();
                Serial.println(val);
                if (val == "start")
                {
                    //Serial.println(val);
                    val = "";
                    showLed();
                    lcd.noCursor();
                    lcd.setCursor(0, 0);
                    lcd.print("LED light time");
                    Display_ON_Time();
                    delay(2000);
                    lcd.noCursor();
                    //Serial.println(week);
                    break;
                }
                else if (val == "cleft")
                {
                    val = "";
                    cursorC = cursorC - 1;
                    if (cursorC < 0)
                    {
                        cursorC = 16 + cursorC;
                    }
                    lcd.setCursor(cursorC, cursorR);
                    lcd.cursor();
                    Serial.print("cursorC: ");
                    Serial.println(cursorC);
                }
                else if (val == "cright")
                {
                    val = "";
                    cursorC = cursorC + 1;
                    if (cursorC > 15)
                    {
                        cursorC = cursorC - 16;
                    }
                    lcd.setCursor(cursorC, cursorR);
                    lcd.cursor();
                    Serial.print("cursorC: ");
                    Serial.println(cursorC);
                }
                else if (val == "-1" || val == "+1")
                {
                    buttonSetON(val);
                    val = "";
                }
                Display_ON_Time();
                lcd.setCursor(cursorC, cursorR);
                lcd.cursor();
            }
        }
//        else
//        {
//            //格式：ON 7 23 59
//            ONweek = val.charAt(3) - 48;
//            ONhour = (val.charAt(5) - 48) * 10 + (val.charAt(6) - 48);
//            ONminute = (val.charAt(8) - 48) * 10 + (val.charAt(9) - 48);
//            Display_ON_Time();
//            lcd.setCursor(0, 0);
//            lcd.print("LED light time");
//            delay(2000);
//            showLed();
//            val = "";
//        }
    }
    else if (val.startsWith("OFF"))
    {
        if (val == "OFFBT")
        {
            lcd.setCursor(0, 0);
            lcd.print("Set OFF time         ");
            lcd.setCursor(cursorC, cursorR);
            lcd.cursor();
            while (1)
            {
                val = BT.readString();
                Serial.println(val);
                if (val == "start")
                {
                    //Serial.println(val);
                    val = "";
                    showLed();
                    lcd.noCursor();
                    lcd.setCursor(0, 0);
                    lcd.print("Led quench time");
                    Display_OFF_Time();
                    delay(2000);
                    //Serial.println(week);
                    break;
                }
                else if (val == "cleft")
                {
                    val = "";
                    cursorC = cursorC - 1;
                    if (cursorC < 0)
                    {
                        cursorC = 16 + cursorC;
                    }
                    lcd.setCursor(cursorC, cursorR);
                    lcd.cursor();
                    Serial.print("cursorC: ");
                    Serial.println(cursorC);
                }
                else if (val == "cright")
                {
                    val = "";
                    cursorC = cursorC + 1;
                    if (cursorC > 15)
                    {
                        cursorC = cursorC - 16;
                    }
                    lcd.setCursor(cursorC, cursorR);
                    lcd.cursor();
                    Serial.print("cursorC: ");
                    Serial.println(cursorC);
                }
                else if (val == "-1" || val == "+1")
                {
                    buttonSetOFF(val);
                    val = "";
                }
                Display_OFF_Time();
                lcd.setCursor(cursorC, cursorR);
                lcd.cursor();
            }
        }
//        else
//        {
//            //格式：OFF 7 23 59
//            OFFweek = val.charAt(4) - 48;
//            OFFhour = (val.charAt(6) - 48) * 10 + (val.charAt(7) - 48);
//            OFFminute = (val.charAt(9) - 48) * 10 + (val.charAt(10) - 48);
//            Display_OFF_Time();
//            lcd.setCursor(0, 0);
//            lcd.print("Led quench time");
//            delay(2000);
//            showLed();
//            val = "";
//        }
    }
    else if (val == "0")
    {
        //digitalWrite(led, LOW);
        led_OFF();
        val = "";
    }
    else if (val == "1")
    {
        //digitalWrite(led, HIGH);
        led_ON();
        val = "";
    }
    else if (val.startsWith("Blink"))
    {
        Serial.println();
        int num = val.charAt(5) - 48;
        Serial.print("default flash times: ");
        Serial.println(num);
        Serial.println("start it or change the times");
        lcd.setCursor(0, 0);
        lcd.print("Please start       ");
        while (1)
        {
            Display();
            val = BT.readString();
            Serial.println(val);
            if (val == "start")
            {
                lcd.setCursor(0, 0);
                lcd.print("Led flashing!    ");
                val = "";
                break;
            }
            else if (val == "-1")
            {
                num = num - 1;
                lcd.setCursor(0, 0);
                lcd.print("flash times:       ");
                lcd.setCursor(13, 0);
                lcd.print(num);
            }
            else if (val == "+1")
            {
                num = num + 1;
                lcd.setCursor(0, 0);
                lcd.print("flash times:       ");
                lcd.setCursor(13, 0);
                lcd.print(num);
            }
        }
        Serial.println();
        while (num > 0)
        {
            Serial.print("remain light number(s): ");
            Serial.println(num);
            blink();
            num = num - 1;
        }
        showLed();
    }
    else if (val.startsWith("key"))
    {
        lcd.setCursor(0, 0);
        lcd.print("Set the key:       ");
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
    else if (val.startsWith("BTkey"))
     {
         Serial.println();
         lcd.setCursor(0, 0);
         lcd.print("**Set the key**       ");
         String key = "";
         Serial.println("key: ");
         while (1)
         {
             if (irrecv.decode(&results))
             {
                 if (results.value == 0xFF906F)
                 {
                     lcd.setCursor(0, 0);
                     lcd.print("##Got the key!##       ");
//                     Serial.println();
//                     Serial.println("————————————————————————————————");
//                     Serial.print("PASSWORD: ");
//                     Serial.println(key);
//                     Serial.println("————————————————————————————————");
//                     Serial.println();
                     delay(2000);
                     led_OFF();
                     String Akey = "";
                     irrecv.resume();
                     while (1)
                     {
                         digitalWrite(relayPin, LOW);
                         //lcd.setCursor(0, 0);
                         //lcd.print("LED: OFF          ");
                         val = "";
                         ledflag = 0;
                         lcd.setCursor(0, 0);
                         lcd.print("Try the key...       ");
                         if (irrecv.decode(&results))
                         {
//                             Serial.println();
//                             Serial.println("————————————————————————————————");
//                             Serial.print("YOUR TOKEN: ");
//                             Serial.println(Akey);
//                             Serial.println("————————————————————————————————");
//                             Serial.println();
                             if (results.value == 0xFF906F)
                             {
                                 if (Akey == key)
                                 {
                                     led_ON();
                                     lcd.setCursor(0, 0);
                                     lcd.print("^^Lighting!^^        ");
                                     delay(2000);
                                     showLed();
                                     Display();
                                     break;
                                 }
                                 else
                                 {
                                     lcd.setCursor(0, 0);
                                     lcd.print("@@Wrong key!@@        ");
                                     delay(2000);
                                     lcd.setCursor(0, 0);
                                     lcd.print("Try it again...       ");
                                     Akey = "";
                                     results.value = "";
                                     irrecv.resume();
                                     Serial.print("irCode: ");
                                     Serial.println(results.value, HEX);
                                     Akey = Akey + results.value;
                                     if(Akey == "351")
                                     {
                                          Akey = "";
                                     }
                                     Serial.println("Akey: ");
                                     Serial.println(Akey);
                                     continue;
                                 }
                             }
                             Serial.print("irCode: ");
                             Serial.println(results.value, HEX);
                             Akey = Akey + results.value;
                             Serial.println("Akey: ");
                             Serial.println(Akey);
                             delay(300);
                             irrecv.resume();
                         }
                     }
                     break;
                 }
                 Serial.print("irCode: ");
                 Serial.println(results.value, HEX);
                 key = key + results.value;
                 Serial.println("key: ");
                 Serial.println(key);
                 delay(300);
                 irrecv.resume();
             }
         }
     }
    showLed();
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
    case 0:
        lcd.setCursor(9, 1);
        lcd.print("Mon.  ");
        break;
    case 1:
        lcd.setCursor(9, 1);
        lcd.print("Tues.  ");
        break;
    case 2:
        lcd.setCursor(9, 1);
        lcd.print("Wed.  ");
        break;
    case 3:
        lcd.setCursor(9, 1);
        lcd.print("Thur.  ");
        break;
    case 4:
        lcd.setCursor(9, 1);
        lcd.print("Fri.  ");
        break;
    case 5:
        lcd.setCursor(9, 1);
        lcd.print("Sat.  ");
        break;
    case 6:
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
