/*
/
=============================== MANEJO DE ARCHIVOS DE LA SD =======================
*/
//variables Generales
File root;
File directorios;
File single;
File multi;

int numero_ficheros;
int total_ficheros = 0;

extern const char *emoRefresh; //Almacena el nombre del directorio a refrescar
//======================================================

//Contador número de ficheros en los directorios
int constador_archivos()
{
    root = SD.open(emoRefresh);
    File cuenta;
    numero_ficheros = 0;
    while (true)
    {
        cuenta = root.openNextFile();
        if (!cuenta)
        {
            //No hay más ficheros
            cuenta.close();
            return numero_ficheros;
        }
        numero_ficheros++;
    }
}
//======================== FIN CONTADOR ARCHIVOS SD =======================

/*
====================== BORRA FICHEROS DE LA SD =======================
*/
void BorradoArchivosSD(String nombre_borrar)
{
    SD.remove(nombre_borrar);
    Serial.println("");
    Serial.print("Archivo Borrado: ");
    Serial.println(nombre_borrar);
}
//======================== FIN BORRADO FICHEROS SD ======================

/*
======================= EXPLORACIÓN ARCHIVOS SD ===================
*/
void explorar_ficheros()
{
    numero_ficheros = constador_archivos(); //Cuenta el número de archivos en el directorio de la tarjeta SD
    root = SD.open(emoRefresh);

    SPIFFS.remove("/dir.json"); //Limpia el archivo de la memoria SPIFFS para ser leido

    //Prepara la memoria SPIFFS para escribir
    directorios = SPIFFS.open("/dir.json", "a");
    if (!directorios)
    {
        Serial.println("No se puede abrir el archivo");
    }

    //===========Si existe sólo un Fichero en el directorio de la tajeta SD seleccionado ====
    if (numero_ficheros == 1) //Cuando hay sólo un archivo en el directorio
    {
        single = root.openNextFile();

        //Formatea el mensaje en JSON
        directorios.print("{\"titulo\":");
        directorios.printf("\"%s\"", single.name());
        directorios.print("}");

        /////////////DEPURACIÓN//////////////////
        Serial.print("{\"titulo\":");
        Serial.printf("\"%s\"", single.name());
        Serial.print("}");

        single.close();
        directorios.close();
        return;
    }

    //======================== SI EXISTEN MÁS DE UN FICHERO EN EL DIRECTORIO ===============
    if (numero_ficheros > 1) //cuando hay más de un fichero
    {
        Serial.print("[");
        directorios.print("[");

        for (int i = 0; i < numero_ficheros - 1; i++)
        {
            multi = root.openNextFile();

            if (!multi) //Si no hay más archivos
            {
                Serial.println("Sin archivos");
                return;
            }

            //Formatea el mensaje JSON
            directorios.print("{\"titulo\":");
            directorios.printf("\"%s\"", multi.name());
            directorios.print("},");

            //====DEPURACIÓN====================
            Serial.print("{\"titulo:\":");
            Serial.printf("\"%s\"", multi.name());
            Serial.print("},");
        }
        //Imprime el último para cerrar el JSON
        multi = root.openNextFile();

        directorios.print("{\"titulo\":");
        directorios.printf("\"%s\"", multi.name());
        directorios.print("}");
        directorios.print("]");

        //=========DEPURACIÓN=============
        Serial.print("{\"titulo\":");
        Serial.printf("\"%s\"", multi.name());
        Serial.print("}");
        Serial.print("]");
    }
    directorios.close();
    multi.close();
}
//================== FIN EXPLORACIÓN TARJETA SD ====================