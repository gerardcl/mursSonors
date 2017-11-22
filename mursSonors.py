#!/usr/bin/python
import RPi.GPIO as GPIO
import time
import pygame
from random import randint

pygame.mixer.init(frequency=48000, size=-16, channels=1, buffer=4096)

GPIO.setmode(GPIO.BOARD)
mypins =  range(11, 14)
totalTracks = 21

print 'starting: mursSonors'
print 'gpio version is: ', GPIO.VERSION

try:
    time.sleep(2)
    while True:
        for PIR_PIN in mypins:
            #GPIO.setup(PIR_PIN, GPIO.IN)
            GPIO.setup(PIR_PIN, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)

            read_input = GPIO.input(PIR_PIN)
            
            if read_input:
                print 'Pin', PIR_PIN, 'is HIGH!'

                trackNum = randint(1,totalTracks)
                trackString = ''
                if trackNum > 9:
                    trackString = str(trackNum)
                else:
                    trackString = '0' + str(trackNum)

                pygame.mixer.music.load('/home/pi/mursSonors/sonsMur/intro-' + str(randint(1,2))  + '.mp3')
                pygame.mixer.music.play()
                while pygame.mixer.music.get_busy() == True:
                    continue

                time.sleep(2)

                trackPath = '/home/pi/mursSonors/sonsMur/track0' + trackString + '.mp3'
                print 'playing: ', trackPath
                pygame.mixer.music.load(trackPath)
                pygame.mixer.music.play()
                while pygame.mixer.music.get_busy() == True:
                    continue
                
                break

            else:
                pass
                print 'Pin', PIR_PIN, 'is LOW'    

        print '-----'
        time.sleep(2)

except KeyboardInterrupt:
    print 'cleaning up and exiting'
    GPIO.cleanup()

except:
    print 'cleaning up and exiting'
    GPIO.cleanup()
