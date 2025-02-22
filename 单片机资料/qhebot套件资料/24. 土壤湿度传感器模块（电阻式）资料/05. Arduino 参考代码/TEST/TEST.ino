float VALUE = 0;


void setup() {
Serial.begin(115200); 
Serial.println(F("reading VALUEerature begin. \n")); 

}

void loop() {
 
  static unsigned long sensortStamp = 0;
  
  if(millis() - sensortStamp > 100){
    sensortStamp = millis();
    VALUE = analogRead(0); //注意到我们是把输出端连接到了A0，所以这里是analogRead(0)
    Serial.print(F("Real Time VALUE: ")); 
    Serial.println(VALUE); 
  }
  
}
