/*
   Example sketch sending a stream of data to a python script to graph.
   Colby Rome 4-6-16
*/
#include "SDI.h"
#include <Wire.h>
#include <SD.h>`
//#include <sensirion.h>
#include <Narcoleptic.h>
#include <SoftwareSerial.h>
#define SERIAL_OUTPUT_PIN 1
#define FLOW_CONTROL_PIN A3
#define SD_PIN A4
#define dataPin  9
#define clockPin 8

int SLEEP=30000;//sleep time
SDIBusController *SDIBus;
char addr;//water sensor
unsigned long lastSecond;//time stamp
float totaltime=0;//in minutes
//sensirion sht(dataPin, clockPin);//temperature sensor
SoftwareSerial mySerial(clockPin, dataPin);//xbee wireless communication
File dataFile;//output file

void powerSDIMiddlePort(){
    // Powers the middle port on the Hydrosense Datalogger 2.1.2
    pinMode(5, OUTPUT);
    digitalWrite(5, HIGH);
    Wire.begin();
    // address 0b1110000 (dec 112) refers to the port expander
    Wire.beginTransmission(0b1110000);
    Wire.write(byte(0x03)); // turns on lights
    Wire.write(0b00000000);  //Sets all pins to output
    Wire.endTransmission();
    Wire.beginTransmission(0b1110000);
    Wire.write(byte(0x01));
    Wire.write(0b11111111);
    Wire.endTransmission();
}

void setup(){
    powerSDIMiddlePort();//power sdi and lights
    
    // instantiate SDISerial instance with hardware Serial1
    pinMode(FLOW_CONTROL_PIN, OUTPUT);
    SDISerial *mySDISerial = new SDISerial(Serial1, SERIAL_OUTPUT_PIN, FLOW_CONTROL_PIN);
    // instantiate SDIBus controller, passing in hardware Serial1 as argument
    SDIBus = new SDIBusController(*mySDISerial);
    // Address of Decagon CTD
    addr = '0';
    // For debugging to the computer
    Serial.begin(9600);
    mySerial.begin(38400);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
  Serial.println("Initializing SD");
  if(!SD.begin(SD_PIN)){
    Serial.println("failed!");
  }
}
void loop(){
   // if(millis() - lastSecond >= 1000){
        dataFile = SD.open("datalog.txt", FILE_WRITE);

        int altno = -1; // 'regular' refreshh function

        // The following will be populated by the call to refresh
        int waitTime;
        int numExpected;


        int res = SDIBus->refresh(addr, altno, &waitTime, &numExpected);
        if(res != 0){
          Serial.print("Received res = ");
          Serial.println(res);
        }
        else{
            delay(500); // theoretically we should wait 1 second.
            float buffer[numExpected];
            res = SDIBus->getData(addr, buffer, numExpected);
            if(res != 0){
              Serial.print("Error during getData. Received: ");
              Serial.println(res);
            }
            else{
              for(int i=0; i<numExpected; i++){
                if(buffer[i] >= 0){
                  dataFile.print('+');
                  mySerial.print('+');
                }
                dataFile.println(buffer[i]);
                mySerial.print(buffer[i]);
              }
              //Serial.print("\r\n");
            }
            
        }
        /*
  //      Wire.requestFrom(4, 100); 
  //      delay(100);
        Wire.requestFrom(4, 100); // address 4
        lastSecond = millis();
        while(0 == Wire.available() && (millis() - lastSecond < 1000)); // wait until a response

        if(0 != Wire.available()){
            char c = Wire.read();
            while(c != '\n'){
                if(c != (char)-1){
                dataFile.print(c);
                mySerial.print(c);
                c = Wire.read();
                }
                else{
                  break;
                }
            }
            // should be newline character
            dataFile.println(c);
            mySerial.print(c);
        }

        print_temp_and_hum(dataFile);
      */
        /*
        delay(1000);
        totaltime+= float(millis()-lastSecond)/1000;
        lastSecond = millis();
        dataFile.println(totaltime);
        */
    dataFile.close();
   // Narcoleptic.delay(SLEEP);
  
 //   delay(100);
 //   }
 //   else delay(10);
}
/*
void print_temp_and_hum(File file){
        float temp_c;
        float temp_f;
        float humidity;
        char temp[6];
        char hum[6];
        char buffer[100];
        uint16_t status;
        if((status=sht.readStatus())==0){
           humidity = sht.readHumidity();
           temp_f = sht.readTemperatureF();
           dtostrf(temp_f, 3, 2, temp);
           dtostrf(humidity, 3, 2, hum);
           pinMode(8,OUTPUT);
           pinMode(9,OUTPUT);
           file.println(hum);
           file.print("+");
           file.print(temp);
           sprintf(buffer, "humidity=%s&tempf=%s\n", hum,temp);
           mySerial.print(buffer);
        }
        else{
          Serial.print("Temp Status = ");
          Serial.println(status);
        }
}
*/


