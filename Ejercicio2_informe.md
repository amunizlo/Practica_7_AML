# Práctica 7.2 Buses de comunicación III (I2S)
###### Andrea Muñiz
<p></p>

## Programa + explicación

> Declaramos las librerías necesarias

```
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
```

> Definimos la velocidad a la que trabajará la tarjeta SD.

```
#define SPI_SPEED SD_SCK_MHZ(40)
```

> Declaramos la variable dir que será como identifiquemos al archivo de música.

```
File dir;
```

> Creamos punteros de los tipos:
- _AudioFileSourcePROGMEM_
- _AudioGeneratorAAC_
- _AudioOutputI2S_

```
AudioFileSourceSD *in;
AudioGeneratorAAC *aac;
AudioOutputI2S *out;
```

> Función setup()

En esta función inicializamos el Serial a una velocidad de 115200. A continuación programamos los distintos punteros. En el caso de _in_ será de tipo _AudioFileSourcePROGMEM_, _aac_ será de tipo _AudioGeneratorAAC_ y _out_ de tipo _AudioOutputI2S_. Para _out_ establecemos la ganancia y los pines a los que se conecta el dispositivo MAX98357A para _LRC_, _BCLK_ y _DIN_, _aac_ se inicializa pasándole las variables de entrada y salida.

```
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
```

> Función bucle

Dentro de esta función creamos un bucle if en el que si el puntero aac está funcionando y no se entra en el loop() entonces se para aac. En caso contrario se abre el archivo de música, si se puede abrir el archivo, se informa por el monitor, en caso contrario se informa de que ha habido un error al abrir el archivo. Una vez terminado de reproducirse el archivo se informa por pantalla.

```
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
```

***

## Salida de la impresión serie

El programa no funciona, por lo que no se puede ver lo que muestra por pantalla.