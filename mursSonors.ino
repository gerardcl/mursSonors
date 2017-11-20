/*
  Author: Gerard Castillo
  date: Nov. 19 2017
*/

#include <SPI.h>           // SPI library
#include <SdFat.h>         // SDFat Library
#include <SFEMP3Shield.h>  // Mp3 Shield Library

// Below is not needed if interrupt driven. Safe to remove if not using.
#if defined(USE_MP3_REFILL_MEANS) && USE_MP3_REFILL_MEANS == USE_MP3_Timer1
  #include <TimerOne.h>
#elif defined(USE_MP3_REFILL_MEANS) && USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer
  #include <SimpleTimer.h>
#endif

SdFat sd; // Create object to handle SD functions

SFEMP3Shield MP3player; // Create Mp3 library object
// These variables are used in the MP3 initialization to set up
// some stereo options:
const uint8_t volume = 55; // MP3 Player volume 0=max, 255=lowest (off)
const uint16_t monoMode = 1;  // Mono setting 0=off, 3=max

/////////////////////////////
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 5;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         
long unsigned int startTrackTime;         
long unsigned int stopTrackTime = 0;         
long unsigned int pauseFromPlay = 5000;  
long unsigned int pauseFromStop = 5000;  

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLowA = true;
boolean lockLowB = true;
boolean lockLowC = true;
boolean takeLowTimeA;  
boolean takeLowTimeB;  
boolean takeLowTimeC;  

int pirPinA = 10;    //the digital pin connected to the PIR sensor's output
int pirPinB = 11;    //the digital pin connected to the PIR sensor's output
int pirPinC = 12;    //the digital pin connected to the PIR sensor's output
int ledPin = 13;

int lastTrack = 0; // This variable keeps track of which tune is playing
int numTracks = 9; // This variable keeps track of which tune is playing
int currentTrack = 0;
bool playing = false;
/////////////////////////////

//SETUP
void setup(){
  Serial.begin(9600);
  pinMode(pirPinA, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pirPinA, LOW);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensors ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);

#if (0)
  // Typically not used by most shields, hence commented out.
  Serial.println(F("Applying ADMixer patch."));
  if(MP3player.ADMixerLoad("admxster.053") == 0) {
    Serial.println(F("Setting ADMixer Volume."));
    MP3player.ADMixerVol(-3);
  }
#endif

  delay(1000);

  initSD();  // Initialize the SD card
  initMP3Player(); // Initialize the MP3 Shield
}

////////////////////////////
//LOOP
void loop(){
// Below is only needed if not interrupt driven. Safe to remove if not using.
#if defined(USE_MP3_REFILL_MEANS) \
    && ( (USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer) \
    ||   (USE_MP3_REFILL_MEANS == USE_MP3_Polled)      )

  MP3player.available();
#endif

  if(digitalRead(pirPinA) == HIGH && !MP3player.isPlaying()){
    digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
    if(lockLowA){  
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLowA = false;            
      Serial.println("---");
      Serial.print("motion detected at ");
      Serial.print(millis()/1000);
      Serial.println(" sec in PIRA"); 
      delay(500);
      
      while (currentTrack == lastTrack) {
        Serial.print("traying change track ");
        Serial.println(currentTrack);
        currentTrack = (int)random(1, numTracks);
      }
      lastTrack = currentTrack;
      
      Serial.print("DETECTION! Pin ");
      Serial.print(pirPinA);
      Serial.print(" - playing random track ");
      Serial.println(currentTrack);
      
      MP3player.setVolume(volume, volume);

      /* If another track is playing, stop it: */
      if (MP3player.isPlaying())
        MP3player.stopTrack();
        
      /* Use the playTrack function to play a numbered track: */
      uint8_t result = MP3player.playTrack(currentTrack);
      if (result == 0)  // playTrack() returns 0 on success
      {
        Serial.println("playback started");
        playing = true;
        startTrackTime = millis();
      }
      else // Otherwise there's an error, check the code
      {
        Serial.print(F("Error code: "));
        Serial.print(result);
        Serial.println(F(" when trying to skip track"));
      }
    }         
    takeLowTimeA = true;
  }
  
  if(digitalRead(pirPinA) == LOW){       
    digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state
    
    if(takeLowTimeA){
      lowIn = millis();          //save the time of the transition from high to LOW
      takeLowTimeA = false;       //make sure this is only done at the start of a LOW phase
    }
    //if the sensor is low for more than the given pause, 
    //we assume that no more motion is going to happen
    if(!lockLowA && millis() - lowIn > pause){  
      //makes sure this block of code is only executed again after 
      //a new motion sequence has been detected
      lockLowA = true;                        
      Serial.print("motion ended at ");      //output
      Serial.print((millis() - pause)/1000);
      Serial.println(" sec in PIRA");
      delay(500);
    }
  }

}


// initSD() initializes the SD card and checks for an error.
void initSD()
{
  //Initialize the SdCard.
  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) 
    sd.initErrorHalt();
  if(!sd.chdir("/")) 
    sd.errorHalt("sd.chdir");
}

// initMP3Player() sets up all of the initialization for the
// MP3 Player Shield. It runs the begin() function, checks
// for errors, applies a patch if found, and sets the volume/
// stero mode.
void initMP3Player()
{
  uint8_t result = MP3player.begin(); // init the mp3 player shield
  if(result != 0) // check result, see readme for error codes.
  {
    // Error checking can go here!
  }
  MP3player.setVolume(volume, volume);
  MP3player.setMonoMode(monoMode);
}
