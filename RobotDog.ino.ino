#include <ArduinoJson.h>
#include "LedControl.h" 
#include "Servo.h"
#include "QueueList.h"

class EyeFrame
{
  public:
  char * left;
  char * right;
  int times;
  byte bright;
  bool used;
  EyeFrame(char * setLeft, char * setRight, byte setBright, int setTimes)
  {
    left = setLeft;
    right = setRight;
    times = setTimes;
    bright = setBright;
    used = false;
  }
  ~EyeFrame()
  {
    delete [] left;
    delete [] right;
  }
};
QueueList<EyeFrame *> queueEye;

LedControl lc = LedControl(12, 11, 10, 2);
//-------------以下DC馬達設定-------------------
int STBY = 8; //standby馬達開關
//Motor A
int PWMA = 3; //Speed control 
int AIN1 = 2; //Direction
int AIN2 = 4; //Direction
//Motor B
int PWMB = 5; //Speed control
int BIN1 = 6; //Direction
int BIN2 = 7; //Direction
//-------------以上DC馬達設定--------------------
//-------------以下servo設定---------------------
Servo servo;
//-------------以上servo設定---------------------

unsigned long time_now;
float leg_left, leg_right;
float leg_left_default, leg_right_default;
unsigned long time_left, time_right;
bool isTime_left, isTime_right;

bool eye_left[8][8], eye_right[8][8];
byte left_bright, right_bright;

float head;

void setup()
{
  //--------LED--------------------------
  lc.shutdown(0,false);
  lc.shutdown(1,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  lc.setIntensity(1,8);
  /* and clear the display */
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  //---------DC---------------------------
  pinMode(STBY, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  //--------------------------------------
  servo.attach(9);//pin 9
  //poi.attach(pin角,servo最低脈衝,servo最高脈衝); 調整servo的脈衝寬度

  
  Serial.begin(57600);
  
  servotime(108);

  delay(5000);
  Serial.println("system setup");
  Serial.println("begin to runloop");

  char * e1 = new char[17]{'P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','\0'};
  char * e2 = new char[17]{'P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','P','\0'};
  EyeFrame * eye = new EyeFrame(e1, e2, 15, 1000);
  queueEye.push(eye);
  e1 = new char[17]{'A','P','A','P','P','P','P','P','P','P','P','P','P','P','P','P','\0'};
  e2 = new char[17]{'A','P','A','P','P','P','P','P','P','P','P','P','P','P','P','P','\0'};
  eye = new EyeFrame(e1, e2, 15, 1000);
  queueEye.push(eye);
  e1 = new char[17]{'A','A','A','A','P','P','P','P','P','P','P','P','P','P','P','P','\0'};
  e2 = new char[17]{'A','A','A','A','P','P','P','P','P','P','P','P','P','P','P','P','\0'};
  eye = new EyeFrame(e1, e2, 15, 1000);
  queueEye.push(eye);
  e1 = new char[17]{'A','A','A','A','P','D','P','D','P','P','P','P','P','P','P','P','\0'};
  e2 = new char[17]{'A','A','A','A','P','D','P','D','P','P','P','P','P','P','P','P','\0'};
  eye = new EyeFrame(e1, e2, 15, 1000);
  queueEye.push(eye);
  e1 = new char[17]{'A','A','A','A','P','D','P','D','P','D','P','D','P','P','P','P','\0'};
  e2 = new char[17]{'A','A','A','A','P','D','P','D','P','D','P','D','P','P','P','P','\0'};
  eye = new EyeFrame(e1, e2, 15, 1000);
  queueEye.push(eye);
  e1 = new char[17]{'A','A','A','A','P','D','P','D','P','D','P','D','A','A','A','A','\0'};
  e2 = new char[17]{'A','A','A','A','P','D','P','D','P','D','P','D','A','A','A','A','\0'};
  eye = new EyeFrame(e1, e2, 15, 1000);
  queueEye.push(eye);
  e1 = new char[17]{'A','A','A','A','M','D','M','D','P','D','P','D','A','A','A','A','\0'};
  e2 = new char[17]{'A','A','A','A','M','D','M','D','P','D','P','D','A','A','A','A','\0'};
  eye = new EyeFrame(e1, e2, 15, 1000);
  queueEye.push(eye);
  e1 = new char[17]{'A','A','A','A','M','D','M','D','M','D','M','D','A','A','A','A','\0'};
  e2 = new char[17]{'A','A','A','A','M','D','M','D','M','D','M','D','A','A','A','A','\0'};
  eye = new EyeFrame(e1, e2, 15, 1000);
  queueEye.push(eye);
  e1 = new char[17]{'A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','\0'};
  e2 = new char[17]{'A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','\0'};
  eye = new EyeFrame(e1, e2, 15, 1000);
}

void loop()
{
  time_now = millis();
  if (Serial.available())
  {
    
    String temp = Serial.readString();
    Serial.println(temp);
    /*
    while (Serial.peek() > 0)
    {
      String jsonString = Serial.readStringUntil('\n');
      Serial.println(jsonString);
      
      int lenData = jsonString.length() + 1;
      char data[lenData];
      jsonString.toCharArray(data, lenData);
      StaticJsonBuffer<500> jsonBuffer;
      JsonObject & json = jsonBuffer.parseObject(data);
      json.prettyPrintTo(Serial);

      if (!json.containsKey("key"))
      {
        Serial.println("there are not key");
      }
      else
      {
        const char * key = json["key"];
        if (strcmp(key, "legs") == 0)
        {
          check_Legs(json);
        }
        else if (strcmp(key, "eyes") == 0)
        {
          check_Eye(json);
        }
        else if (strcmp(key, "head") == 0)
        {
          check_Head(json);
        }
      }
    }*/
  }

  if (isTime_left)
  {
    if (time_now >= time_left)
    {
      isTime_left = false;
      leg_left = leg_left_default;
      setMove(1, leg_left);
      done_Legs("left_leg", leg_left);
    }
  }
  if (isTime_right)
  {
    if (time_now >= time_right)
    {
      isTime_right = false;
      leg_right = leg_right_default;
      setMove(2, leg_right);
      done_Legs("right_leg", leg_right);
    }
  }

  bool empty = queueEye.isEmpty();
  if (!empty)
  {
    EyeFrame * eye = queueEye.peek();
    if (time_now >= eye->times)
    {
      char * leftEye = eye->left;
      char * rightEye = eye->right;
      parseEyeData(leftEye, eye_left);
      parseEyeData(rightEye, eye_right);
      int bright = eye->bright;
      setSprite(0, eye_left, bright);
      setSprite(1, eye_right, bright);
      
      eye = queueEye.pop();
      delete(eye);
      
      if (!queueEye.isEmpty())
      {
        eye = queueEye.peek();
        if (!eye->used)
        {
          eye->times = time_now + eye->times;
          eye->used = true;
        }
        Serial.println(eye->left);
        Serial.println(eye->right);
        Serial.println(String(eye->times));
        Serial.println(String(eye->bright));
      }
    }
  }
  
  delay(10);
}
void check_Legs(JsonObject & json)
{
  if (json.containsKey("left") && json.containsKey("right"))
  {
    float left = json["left"];
    float right = json["right"];
    if ((left < -1 || left > 1) && (right < -1 || right > 1))
    {
      errorLegsWrongValue("legs");
    }
    else
    {
      leg_left = left;
      leg_right = right;
      setMove(1, left);
      setMove(2, right);
      if (json.containsKey("times"))
      {
        unsigned long millisecond = json["times"];
        time_left = millisecond + time_now;
        time_right = millisecond + time_now;
        isTime_left = true;
        isTime_right = true;
      }
      else
      {
        leg_left_default = left;
        leg_right_default = right;
      }
      
      if (isTime_right)
        ack_Legs("legs", time_right - time_now);
      else
        ack_Legs("legs");
    }
  }
  else
  {
    if (leg_left == leg_right && isTime_left == isTime_right && time_left == time_right)
    {
      if (isTime_right)
        doing_Legs("legs", leg_right, time_right - time_now);
      else
        doing_Legs("legs", leg_right);
    }
    else
    {
      if (isTime_left)
        doing_Legs("left_leg", leg_left, time_left - time_now);
      else
        doing_Legs("left_leg", leg_left);
      if (isTime_right)
        doing_Legs("right_leg", leg_right, time_right - time_now);
      else
        doing_Legs("right_leg", leg_right);
    }
  }
}
void FromByte(char c, bool b[4])
{
    for (int i = 0; i < 4; ++i)
        b[i] = (c & (1 << i)) != 0;
}

void check_Eye(JsonObject & json)
{
  Serial.println("eye event");
  if (json.containsKey("left") && json.containsKey("right"))
  {
    const char * leftEyeTemp = json["left"];
    const char * rightEyeTemp = json["right"];
    char * leftEye = new char[17];
    char * rightEye = new char[17];
    strncpy(leftEye, leftEyeTemp, 17);
    strncpy(rightEye, rightEyeTemp, 17);
    //strncpy(leftEye, leftEyeTemp, 17);
    //strncpy(rightEye, rightEyeTemp, 17);
    byte bright = json["bright"];
    int times = json["times"];
    if (queueEye.isEmpty())
    {
      times += time_now;
    }
    
    EyeFrame * eye = new EyeFrame(leftEye, rightEye, bright, times);
    queueEye.push(eye);
  }
}
void parseEyeData(char * eyeData, bool eyeArr[8][8])
{
  int x = 7, y = 7;
  for (int i = 0; i < 16; i++)
  {
    char c = eyeData[i] - 65;
    bool sourceData[4];
    FromByte(c, sourceData);
    for (int j = 0; j < 4; j++)
    {
      eyeArr[x][y] = sourceData[j];
      x--;
      if (x < 0)
      {
        x = 7;
        y--;
      }
    }
  }
}

void check_Head(JsonObject & json)
{
  if (json.containsKey("value"))
  {
    float value = json["value"];
    if (value < 0 || value > 1)
    {
      errorHeadWrongValue();
    }
    else
    {
      head = value;
      servotime(value * 180);
      ack_Legs("head", head);
    }
  }
  else
  {
    doing_Legs("head", head);
  }
}
/*
JsonObject & GetJson()
{
  String originData = Serial.readStringUntil('\n');
  Serial.println(originData);
  int lenData = originData.length() + 1;
  char data[lenData];
  originData.toCharArray(data, lenData);
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject & json = jsonBuffer.parseObject(data);
  return json;
}
*/
void errorCommand(String key, String errorContent)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & feedback = jsonBuffer.createObject();
  feedback["key"] = key;
  feedback["status"] = "error";
  feedback["value"] = errorContent;
  feedback.prettyPrintTo(Serial);
}

void errorLegsWrongValue(String key)
{
  errorCommand(key, "Wrong input value, must between -1 ~ 1");
}
void errorHeadWrongValue()
{
  errorCommand("head", "Wrong input value, must between 0 ~ 1");
}

void ack_Legs(String key)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & feedback = jsonBuffer.createObject();
  feedback["key"] = key;
  feedback["status"] = "ack";
  feedback["left"] = leg_left;
  feedback["right"] = leg_right;
  feedback.prettyPrintTo(Serial);
}
void ack_Legs(String key, unsigned long continued)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & feedback = jsonBuffer.createObject();
  feedback["key"] = key;
  feedback["status"] = "ack";
  feedback["left"] = leg_left;
  feedback["right"] = leg_right;
  feedback["times"] = continued;
  feedback.prettyPrintTo(Serial);
}
void doing_Legs(String key, float value)
{
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject & feedback = jsonBuffer.createObject();
  feedback["key"] = key;
  feedback["status"] = "doing";
  feedback["value"] = value;
  feedback.prettyPrintTo(Serial);
}
void doing_Legs(String key, float value, unsigned long continued)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & feedback = jsonBuffer.createObject();
  feedback["key"] = key;
  feedback["status"] = "doing";
  feedback["value"] = value;
  feedback["times"] = continued;
  feedback.prettyPrintTo(Serial);
}
void done_Legs(String key, float value)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & feedback = jsonBuffer.createObject();
  feedback["key"] = key;
  feedback["status"] = "done";
  feedback["value"] = value;
  feedback.prettyPrintTo(Serial);
}

void feedback_Eye(String key, String stat, byte bright, int times)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & feedback = jsonBuffer.createObject();
  feedback["key"] = key;
  feedback["status"] = stat;
  feedback["bright"] = bright;
  feedback["times"] = times;
  feedback.prettyPrintTo(Serial);
}

//---------------------------------------------------------------------------------------------
//LED函式
void setSprite(int x, bool eyeArr[8][8], int bright)
{
  //LED顯示(左眼或右眼,表情,亮度)
  //左右眼 0 和 1 表示，亮度0~15
  
  byte motion[8] = { ToByte(eyeArr[0]), 
                     ToByte(eyeArr[1]), 
                     ToByte(eyeArr[2]), 
                     ToByte(eyeArr[3]), 
                     ToByte(eyeArr[4]), 
                     ToByte(eyeArr[5]), 
                     ToByte(eyeArr[6]), 
                     ToByte(eyeArr[7]) };
  lc.setIntensity(x, bright); // LED亮度(左眼或右眼,多亮) 
  
  for (int r = 0; r < 8; r++)
        lc.setRow(x, r, motion[r]);
  
}

//DC馬達函式
void move(int motor, int speed, int direction)
{
//Move specific motor at speed and direction
//motor: 0 for B 1 for A
//speed: 0 is off, and 255 is full speed
//direction: 0 clockwise, 1 counter-clockwise

  digitalWrite(STBY, HIGH); //disable standby啟動DC馬達
  //digitalWrite(STBY, LOW); 關閉馬達

  boolean inPin1 = LOW;
  boolean inPin2 = HIGH;

  if(direction == 1){
    inPin1 = HIGH;
    inPin2 = LOW;
  }

  if(motor == 1){
    digitalWrite(AIN1, inPin1);
    digitalWrite(AIN2, inPin2);
    analogWrite(PWMA, speed);
  }else{
    digitalWrite(BIN1, inPin1);
    digitalWrite(BIN2, inPin2);
    analogWrite(PWMB, speed);
  }
}
void setMove(int motor, float value)
{
  if (value < 0)
    move(motor, -value * 255, 0);
  else
    move(motor, value * 255, 1);
}

//servo函式
void servotime(int r) 
{
  servo.write(r); //r為角度 0~180度
}

byte ToByte(boolean b[8])
{
    byte c = 0;
    for (int i = 0; i < 8; ++i)
        if (b[i])
            c |= 1 << i;
    return c;
}

