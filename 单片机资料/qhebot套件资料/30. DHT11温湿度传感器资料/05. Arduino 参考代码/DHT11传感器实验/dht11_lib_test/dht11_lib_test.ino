#include "dht.h"
      
dht DHT;  

#define DHT11_PIN 3//put the sensor in the digital pin 8  
      
      
void setup()  {  
    Serial.begin(9600);  
    Serial.println("init over");  
}  
      
void loop()  {    
      DHT.read11(DHT11_PIN);   
      Serial.print(DHT.humidity,1);  
      Serial.print(",\t");  
      Serial.println(DHT.temperature,1);  
      
      delay(1000);  
 }  
 
