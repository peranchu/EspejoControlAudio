#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <Audio.h>

//Ficheros Carpeta include
#include "audio.hpp"
#include "config.h"
#include "server.hpp"
#include "ESP32_utils.hpp"
#include "archivos.hpp"
#include "websocket.hpp"

//////////////////////////////////////

//Variables acceso Global Compartidas por "server.hpp", "websocket.hpp"
int emocion_val;                //Almacena la emocion seleccionada en el formulario del cliente
const char *Nombre_pista_audio; //Almacena el nombre de la pista de audio seleccionada en el formulario del cliente
const char *emoRefresh;         //Almacena el directorio a refrescar.
//=============================================================

bool SD_present;

void setup()
{
  Serial.begin(115200);

  //Inicio sistema de Archivos SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("Error SPIFFS");
    return;
  }
  //////////// FIN INICIO SPIFFS ///////////////////////////

  //Inicio Sistema de Archivos SD
  Serial.print(F("Iniciando Sd Card..."));
  SPI.begin(14, 2, 15, 13); //SCK, MISO, MOSI, CS
  if (!SD.begin(13))        //SD_CS
  {
    Serial.println(F("Error SD"));
    SD_present = false;
  }
  else
  {
    Serial.println(F("SD Iniciada..."));
    SD_present = true;
  }
  ////////// FIN INICIO SD //////////////////////////

  ConnectWiFi_STA(); //Inicia la conexión WiFi y MDNS, "ESP32_utils.hpp"

  InitServer(); //Inicia El Servidor, "server.hpp"

  InitWebSocket(); //Inicia los servicios websocket "websocket.hpp"
}

void loop()
{
  // put your main code here, to run repeatedly:
}