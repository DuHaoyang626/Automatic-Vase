/*
 * Stepper_Motor
 * 步进电机驱动，实现正反转
 * 接线  A -- D2 B -- D3 C -- D4  D -- D5 
 * 沁和智能--qhebot
 */
void setup() {
  // put your setup code here, to run once:
  for (int i = 2; i < 6; i++) {
    pinMode(i, OUTPUT);
  }
}

void loop() {
  
  clockwise(512);
  delay(10);
  anticlockwise(512);
  delay(10);
}


void clockwise(int num)
{
  for (int count = 0; count < num; count++)
  {
    for (int i = 2; i < 6; i++)
    {
      digitalWrite(i, HIGH);
      delay(3);
      digitalWrite(i, LOW);
    }
  }
}

void anticlockwise(int num)
{
  for (int count = 0; count < num; count++)
  {
    for (int i = 5; i > 1; i--)
    {
      digitalWrite(i, HIGH);
      delay(3);
      digitalWrite(i, LOW);
    }
  }
}
