#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

//Rx=2 & Tx=3
SoftwareSerial adnSerial(2, 3);

const int oneWireBus = 4;          
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
float temp;
const int PH_pin = A0;
const int TBD_pin = A1;

///////////////////////////////////////////////////////////////

//PH Var
float PH_step;
int PH_analog_val;
float PH_vol_val;
float PH_liquid;

//PH Voltage calibrate
float PH4 = 3.1;
float PH7 = 2.5;

///////////////////////////////////////////////////////////////

//TBD Var
int TBD_analog_val;
float TBD_val;
float TBD_vol_val;
//TBD Voltage Calibrate
float MaxTBD = 3.5;
float MinTBD = 0.5;
String str;

///////////////////////////////////////////////////////////////

void setup() {
  sensors.begin();
  pinMode(PH_pin, INPUT);
  pinMode(TBD_pin, INPUT);
  Serial.begin(9600);
  adnSerial.begin(9600);
  delay(2000);
 
}
void loop() {
  StaticJsonDocument<1000> doc;

  sensor();

  doc["Temp"] = temp;
  doc["pH"] = PH_liquid;
  doc["TBD"] = TBD_val;

  serializeJson(doc, adnSerial);
  doc.clear();
  
  ///////////////////////////////////////////////////////////////

//  adnSerial.print(temp);
//  adnSerial.print(',');
//  adnSerial.print(PH_liquid);
//  adnSerial.print(',');
//  adnSerial.print(TBD_val);
//  adnSerial.print("\n");
//  str =String(PH_liquid)+String(" ")+String(TBD_val);
//  espSerial.println(str);
  delay(1000);
}

void sensor(){
  //temp config
  sensors.requestTemperatures(); 
  temp = sensors.getTempCByIndex(0);
  Serial.print ("Nilai Temperature: ");
  Serial.println (temp);
  Serial.println();
  
  //ph config
  PH_analog_val = analogRead(PH_pin);
  PH_vol_val = PH_analog_val * (5.0 / 1024.0);
  PH_step = (PH4 - PH7) /3;
  PH_liquid = 7.00 + ((PH7 - PH_vol_val) / PH_step);
  Serial.print ("pH V output (V): ");
  Serial.println (PH_vol_val);
  Serial.print ("Nilai pH: ");
  Serial.println (PH_liquid);
  Serial.println();   
  
  //tbd config
  TBD_analog_val = analogRead(TBD_pin);
  TBD_vol_val = TBD_analog_val * (5.0 / 1024.0);
  TBD_val = map(TBD_vol_val, MinTBD, MaxTBD, 4, 0);
  Serial.print ("Turbidity V output (V): ");
  Serial.println (TBD_vol_val);
  Serial.print ("Nilai Kekeruhan: ");
  Serial.println (TBD_val);
  Serial.println("-----------------------------------");
}
