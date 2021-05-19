//
//_______________CONFIGURACIÓN DE LOS SOCKETS ASÍNCRONOS DEL SERVIDOR//______________________

AsyncWebSocket ws("/ws"); //Objeto AsyncWebSocket "ws" y ruta de escucha de las peticiones del socket

AsyncWebSocketClient *globalClient = NULL;

const char *NombreBorrado; //Almacena el nombre del Fichero a borrar;

//Variables de acceso global, compartidas con "server.hpp"
extern int emocion_val;                //Almacena la emocion seleccionada en el formulario del cliente
extern const char *Nombre_pista_audio; //Almacena el nombre de la pista de audio seleccionada en el formulario del cliente
extern const char *emoRefresh;         //Almacena el nombre del directorio a refrescar
//===========================================================================

/*
=========================ENVIO DATOS DESDE EL SERVIDOR AL CLIENTE===================
*/
//Envio Petición Inicial de estado de conexión
void EnvioInicial()
{
    if (globalClient != NULL && globalClient->status() == WS_CONNECTED)
    {                                    //Si hay algún cliente y esta conectado...
        StaticJsonDocument<300> Jsondoc; //Se crea el documento JSON

        String response; //Almacenará el JSON

        //Crea el Formato con los datos requeridos
        Jsondoc["estado"] = WiFi.status();
        Jsondoc["IP"] = WiFi.localIP().toString();
        Jsondoc["MDNS"] = hostname;

        serializeJson(Jsondoc, response); //crea el documento JSON

        globalClient->text(response); //Envía el Json al Cliente
    }
}
//====================FIN PETICIÓN ESTADO DE CONEXIÓN========================================

/*
========ANÁLISIS DE DATOS DE LAS CADENAS DE ENTRADA=======================================
*/
//Datos que llegan desde el cliente al Servidor
void CadenaEntrada(String datosEntrada)
{
    //Serial.println(datosEntrada);
    DynamicJsonDocument doc(200); //creación del objeto JSON

    DeserializationError error = deserializeJson(doc, datosEntrada); //comprueba que el JSON entrante es correcto
    if (error)
    {
        Serial.print("Deserializacion JSON Fallida: ");
        Serial.println(error.c_str());
        return;
    }

    //==================== MENSAJES ENTRADA DESDE EL CLIENTE ======================
    //****************************************************************************//
    //Recoje el nombre del fichero de audio seleccionado en el formulario
    int recvNameEmo = datosEntrada.indexOf("NOMBRE_AUDIO_FORM");
    if (recvNameEmo >= 0)
    {
        Nombre_pista_audio = doc["NOMBRE_AUDIO_FORM"];
        Serial.println(Nombre_pista_audio);
    }

    //Recoje la emocion seleccionada en el formulario
    int recvEmocion = datosEntrada.indexOf("EMO_FORM");
    if (recvEmocion >= 0)
    {
        emocion_val = doc["EMO_FORM"];
        Serial.println(emocion_val);
    }
    ///////////////////////////////////////////////////////

    //Recoje Refresco de directorios
    int Refresh_dir = datosEntrada.indexOf("REFRESH");
    if (Refresh_dir >= 0)
    {
        emoRefresh = doc["REFRESH"];
        //Serial.println(emoRefresh);

        explorar_ficheros(); //En "archivos.hpp"
    }
    /////////////////////////////////////////////////////

    //Recoje la solicitud de Borrado de Ficheros de la SD
    int posDelete = datosEntrada.indexOf("ERASE");
    if (posDelete >= 0)
    {
        NombreBorrado = doc["ERASE"];
        BorradoArchivosSD(NombreBorrado); //En "archivos.hpp"
    }
    //////////////////////////////////////////////////
}
/*
====================FIN ANÁLISIS CADENA DE ENTRADA=============================
*/

/*
====================FUNCIÓN QUE RECOGE LOS EVENTOS ENTRANTES A TRAVÉS DEL SOCKET===================
*/
//_ESCUCHA LOS EVENTOS DEL SOCKET_______________________

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT)
    { //Cliente conectado
        Serial.println("");
        Serial.println("Conexión Cliente al WebSocket Recibida");
        globalClient = client;

        EnvioInicial(); //Envio del estado de la conexión al cliente "websocket.hpp"
    }
    else if (type == WS_EVT_DISCONNECT)
    { //Cliente desconectado
        Serial.println("");
        Serial.println("Ciente Desconectado");
        Serial.println("-----------------------");
        globalClient = NULL;
    }
    else if (type == WS_EVT_DATA)
    { //Llegada de algún tipo de dato...
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        String msg = "";
        if (info->final && info->index == 0 && info->len == len)
        {
            //the whole message is in a single frame and we got all of it's data
            //Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
            if (info->opcode == WS_TEXT)
            {
                //Serial.printf("[%u] texto: %s\n", client->id(), data);
                for (size_t i = 0; i < info->len; i++)
                {
                    msg += (char)data[i];
                }
                CadenaEntrada(msg); //Envia la cadena de Entrada para su análisis "websocket.hpp"
            }
        }
    }
}
/*
================FIN ESCUCHA DE EVENTOS DEL SOCKET===========================
*/

//__________________INICIALIZACIÓN DEL SERVICIO DE SOCKETS//_______________________________
void InitWebSocket()
{
    ws.onEvent(onWsEvent);  //Se Ejecuta cuando ocurre un evento a través del Socket
    server.addHandler(&ws); //Registro del objeto AsyncWebserver en el servidor http asíncrono

    Serial.println("WebSocket Server Iniciado");
}