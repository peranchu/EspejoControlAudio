//
//____________CONFIGURACIÓN SERVIDOR//_______________________________

File archivo; //Utilizado para escribir en la SD los archivos que se envian

AsyncWebServer server(80); // INSTANCIA Y PUERTO SERVIDOR

//Variables Globales compartidas con "websocket.hpp"
extern int emocion_val;                //Almacena la emocion seleccionada en el formulario del cliente
extern const char *Nombre_pista_audio; //Almacena el nombre de la pista de audio seleccionada en el formulario del cliente

//===================MANEJO CARGA DE ARCHIVOS A LA SD===============================

//Función llamada desde el botón de envío de Formulario
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index)
    {
        switch (emocion_val) //Evalua el tipo de emoción para guardar el archivo en su carpeta correpondiente
        {
        case 1:
            if (SD.mkdir("/Enfadado"))
            {
                Serial.println("Directorio creado");
                filename = "/Enfadado/" + filename;
            }
            else
            {
                Serial.println("No se ha podido crear el directorio");
            }
            break;

        case 2:
            if (SD.mkdir("/Disgustado"))
            {
                Serial.println("Directorio creado");
                filename = "/Disgustado/" + filename;
            }
            else
            {
                Serial.println("No se ha podido crear el directorio");
            }
            break;

        case 3:
            if (SD.mkdir("/Temeroso"))
            {
                Serial.println("Directorio creado");
                filename = "/Temeroso/" + filename;
            }
            else
            {
                Serial.println("No se ha podido crear el directorio");
            }
            break;

        case 4:
            if (SD.mkdir("/Feliz"))
            {
                Serial.println("Directorio creado");
                filename = "/Feliz/" + filename;
            }
            else
            {
                Serial.println("No se ha podido crear el directorio");
            }
            break;

        case 5:
            if (SD.mkdir("/Neutral"))
            {
                Serial.println("Directorio creado");
                filename = "/Neutral/" + filename;
            }
            else
            {
                Serial.println("No se ha podido crear el directorio");
            }
            break;

        case 6:
            if (SD.mkdir("/Triste"))
            {
                Serial.println("Directorio creado");
                filename = "/Triste/" + filename;
            }
            else
            {
                Serial.println("No se ha podido crear el directorio");
            }
            break;

        case 7:
            if (SD.mkdir("/Sorprendido"))
            {
                Serial.println("Directorio creado");
                filename = "/Sorprendido/" + filename;
            }
            else
            {
                Serial.println("No se ha podido crear el directorio");
            }
            break;
        }

        Serial.println("UploadStart: " + filename);

        archivo = SD.open(filename, FILE_WRITE); //prepara el archvio para ser escrito
    }
    if (len)
    {
        //Serial.println("escribiendo...");
        archivo.write(data, len); //Escribe el archivo en la SD
    }

    if (final)
    {
        Serial.println("UploadEnd: " + filename + "," + index + "," + len);
        archivo.close();
        request->send(200, "text/plain", "Archivo Subido"); // Devuelve al cliente "OK"
    }
}
//========================FIN MANEJO CARGA DE ARCHIVOS A LA SD=================================

//========================ARCHIVOS SERVIDOS=============================
void InitServer()
{

    //ARCHIVOS DEL FRAMEWORK MATERIALIZE
    server.on("/materialize.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/materialize.min.css", "text/css"); });

    server.on("/icon.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/icon.css", "text/css"); });

    server.on("/materialize.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/materialize.min.js", "text/javascript"); });

    //ARCHIVOS DE LAS PETICIONES DEL CLIENTE///////////////////////////////
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/favicon.png", "image/png"); });

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });

    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/script.js", "text/javascript"); });

    server.on("/dir.json", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/dir.json", "text/json"); });

    server.on(
        "/subida", HTTP_POST, [](AsyncWebServerRequest *request)
        { request->send(200); },
        handleUpload); // Manejo de las peticiones de subida de archivos al servidor

    server.onNotFound([](AsyncWebServerRequest *request)
                      { request->send(400, "text/plain", "Not found"); });

    //Inicio Servidor
    server.begin();
    Serial.println("HTTP Server Iniciado");
}
//=========================FIN ARCHIVOS SERVIDOS=============================