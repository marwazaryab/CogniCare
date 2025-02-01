#include <Arduino.h>

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"


MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for moreaveraging.4isgood.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred


float beatsPerMinute; //Current BPM value
int beatAvg; //Average BPMvalue


void setup() {

  Serial.begin(9600);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //UsedefaultI2Cport, 400kHz speed
  {
    Serial.println("MAX3 0102 was not found. Please check wiring/power. ");
    while (1) ; //Infinite loop to stop the program
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");
 
  particleSensor.setup(); //Configuresensorwith default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //TurnRedLEDtolow to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);  // Turnoff GreenLED
}


long lastPrintTime = 0;


void loop() {
  long irValue = particleSensor.getIR();  


  if (checkForBeat(irValue) == true) {
    //Calculate beatsPerMinute
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);


    if (beatsPerMinute < 255 && beatsPerMinute >20) { //Checkifthe BPM value is within a valid range
      rates[rateSpot++] = (byte)beatsPerMinute; //Storethisreading in the array
      rateSpot %= RATE_SIZE; //Wrapvariable
     
      //Take average of readings
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
      }
  }


//Print the IR value, current BPM value, and averageBPMvaluetothe serial monitor
  if (millis() - lastPrintTime >= 1000) {  
    lastPrintTime = millis();  // Update timestamp


    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print(", BPM=");
    Serial.print(beatsPerMinute);
    Serial.print(", Avg BPM=");
    Serial.print(beatAvg);


    if (irValue < 50000)
      Serial.print(" No finger?");
     
    Serial.println();
  }
}
