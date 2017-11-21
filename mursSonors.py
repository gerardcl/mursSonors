import RPi.GPIO as GPIO
import time
import pygame
from random import randint

pygame.mixer.init()

GPIO.setmode(GPIO.BOARD)
mypins =  range(11, 14) # 11,12,13

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
                trackPath = 'sonsMur/track00' + str(randint(1, 9)) + '.mp3'
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
