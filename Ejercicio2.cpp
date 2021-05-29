#include <Arduino.h>
#include <FS.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <HTTPClient.h>
#include <SD.h>
#include <AudioGeneratorAAC.h>
#include <AudioOutputI2S.h>
#include <AudioFileSourcePROGMEM.h>
#include "AudioFileSourceSD.h"

#define SPI_SPEED SD_SCK_MHZ(40)

File dir;

AudioFileSourceSD *in;
AudioGeneratorAAC *aac;
AudioOutputI2S *out;

void setup(){
    Serial.begin(115200);
    Serial.println();
    delay(1000);

    audioLogger = &Serial;  
    in = new AudioFileSourceSD();
    out = new AudioOutputI2S();
    aac = new AudioGeneratorAAC();

    out -> SetGain(0.125);
    out -> SetPinout(26, 25, 27);
    aac -> begin(in, out);
    #if defined(ESP32)
        SD.begin(SS, SPI_SPEED);
    #else 
        SD.begin();
    #endif
    dir = SD.open("/");
}

void loop(){
    if((aac) && (aac -> isRunning())){
        if(!aac->loop()){
            aac->stop();
        } 
    }
    else{
        File file = dir.openNextFile();
        if(file){
            if(String(file.name()).endsWith(".wav")){
                in->close();
                if(in->open(file.name())){
                    Serial.printf_P(PSTR("Playing '%s' from SD card...\n"), file.name());
                    aac->begin(in, out);
                }
                else{
                    Serial.printf_P(PSTR("Error opening '%s'\n"), file.name());
                }
            }
        }
        else{
            Serial.println(F("Playback from SD card done\n"));
            delay(1000);
        }
    }
}
