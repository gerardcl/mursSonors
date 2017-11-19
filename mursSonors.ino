/*
  MP3 Shield Trigger
  by: Jim Lindblom
      SparkFun Electronics
  date: September 23, 2013
  
  This is an example MP3 trigger sketch for the SparkFun MP3 Shield.
  Pins 0, 1, 5, 10, A0, A1, A2, A3, and A4 are setup to trigger tracks
  "track001.mp3", "track002.mp3", etc. on an SD card loaded into
  the shield. Whenever any of those pins are shorted to ground,
  their respective track will start playing.
  
  When a new pin is triggered, any track currently playing will
  stop, and the new one will start.
  
  A5 is setup to globally STOP playing a track when triggered.
  
  If you need more triggers, the shield's jumpers on pins 3 and 4 
  (MIDI-IN and GPIO1) can be cut open and used as additional
  trigger pins. Also, because pins 0 and 1 are used as triggers
  Serial is not available for debugging. Disable those as
  triggers if you want to use serial.
  
  Much of this code was grabbed from the FilePlayer example
  included with the SFEMP3Shield library. Major thanks to Bill
  Porter, again, for this amazing library!
*/

#include <SPI.h>           // SPI library
#include <SdFat.h>         // SDFat Library
#include <SFEMP3Shield.h>  // Mp3 Shield Library

SdFat sd; // Create object to handle SD functions

SFEMP3Shield MP3player; // Create Mp3 library object
// These variables are used in the MP3 initialization to set up
// some stereo options:
const uint8_t volume = 55; // MP3 Player volume 0=max, 255=lowest (off)
const uint16_t monoMode = 1;  // Mono setting 0=off, 3=max

//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 60;

/* Pin setup */
int numPins = 3;
int triggerPins[numPins] = {10, 11, 12};
int lastTrack = 0; // This variable keeps track of which tune is playing
int numTracks = 10; // This variable keeps track of which tune is playing
int currentTrack = 0;
bool playing = false;

void setup()
{
  Serial.begin(115200);
  /* Set up all trigger pins as inputs, with pull-ups activated: */
  for (int i=0; i<numPins; i++)
  {
    pinMode(triggerPins[i], INPUT);
    digitalWrite(triggerPins[i], LOW);
  }

  //give the sensor some time to calibrate
  Serial.print("calibrating sensors ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
  
  initSD();  // Initialize the SD card
  initMP3Player(); // Initialize the MP3 Shield
}

// All the loop does is continuously step through the trigger
//  pins to see if one is pulled low. If it is, it'll stop any
//  currently playing track, and start playing a new one.
void loop()
{
  for (int i=0; i<numPins; i++)
  {
    if ((digitalRead(triggerPins[i]) == HIGH) && ((MP3player.getState() == ready)))
    {
      
      while (currentTrack == lastTrack) {
        currentTrack = (int)random(1, numTracks);
      }
      
      lastTrack = currentTrack;
      
      Serial.print("DETECTION! Pin ");
      Serial.print(triggerPins[i]);
      Serial.print(" - playing random track ");
      Serial.println(currentTrack);

      /* If another track is playing, stop it: */
      if (MP3player.isPlaying())
        MP3player.stopTrack();
        
      /* Use the playTrack function to play a numbered track: */
      uint8_t result = MP3player.playTrack(currentTrack);
      
      if (result == 0)  // playTrack() returns 0 on success
      {
        Serial.println("playback started");
        playing = true;
      }
      else // Otherwise there's an error, check the code
      {
        Serial.print(F("Error code: "));
        Serial.print(result);
        Serial.println(F(" when trying to skip track"));
      }
      delay(1000);
    }
  }
  
  // After looping through and checking trigger pins, check to
  if((MP3player.getState() != playback) && millis() - lowIn > pause){  
    //makes sure this block of code is only executed again after 
    //a new motion sequence has been detected
    playing = false;                        
    Serial.print("motion ended at ");      //output
    Serial.print((millis() - pause)/1000);
    Serial.println(" sec in PIRB");
    delay(500);
  }
    
  

//  if ((digitalRead(stopPin) == HIGH) && ((MP3player.getState() == playback) || (MP3player.getState() == loading)))
//  {
//    lastTrigger = 0; // Reset lastTrigger
//    // If another track is playing, stop it.
//    if (MP3player.isPlaying())
//      MP3player.stopTrack();
//
//    delay(2000);
//
//    Serial.print("stop ");
//    Serial.println("PIR input received"); 
//  }
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
