/*
===================== CONFIGURACIÓN SISTEMA DE AUDIO =========================
*/
Audio audio; //Instancia objeto Audio

bool estado;

//============ PINES AUDIO MAX98357A =============================
#define I2S_DOUT 25 //DIN
#define I2S_BCLK 27 //Bit CLOCK
#define I2S_LRC 26  //Left Right CLOCK

//Configuración inicial Audio
void audioConfig()
{
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(10); //0...21
}
// ======= FIN CONFIGURACIÓN INICIAL AUDIO ==============

// =================== VOLUMEN SD ==============
int GetVolume(int volumen)
{
    Serial.println(volumen);
    return volumen;
}
// =========== FIN VOLUMEN ===================

// =============== REPRODUCIR ARCHIVOS SD ==================
void PlayFileSD(String PlayFile)
{
    audio.connecttoFS(SD, PlayFile); //Reproduce el archivo especificado desde la SD
    estado = true;
}
// ============== FIN REPRODUCCIÓN SD ==================

// ============= STOP SD REPRODUCTOR ====================
void stop_Reproductor()
{
    audio.stopSong(); //Detine la reproducción actual, "Botón Stop"
    estado = false;
}
// ============= FIN STOP =====================