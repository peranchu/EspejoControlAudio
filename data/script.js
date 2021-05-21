//____________________________________SCRIPT CONTROL________________________________

/*
============================INSTACIACIÓN ELEMENTOS DEL DOMM=================================
 */
const fileInput = document.getElementById('entrada'); //instancia a File_in en HTML
const emocion_val = document.getElementById('emocion_value');  //instancia de la emocion selecionada

var progress = document.getElementById('relleno'); //instancia a barra de progreso HTML

document.querySelector('#conexionWs_on').addEventListener('click', conexion_WS); //Boton conexion ON
document.querySelector('#conexionWs_off').addEventListener('click', desconectar); //Boton desconexion OFF

document.querySelector('#vol').addEventListener('change', envioVolumen); //Slider Volumen

//Desplegables directorios emociones
document.getElementById('Enfadado').addEventListener('click', Refresco);    //Enfadado
document.getElementById('Disgustado').addEventListener('click', Refresco);  //Disgustado
document.getElementById('Temeroso').addEventListener('click', Refresco);    //Temeroso
document.getElementById('Feliz').addEventListener('click', Refresco);       //Feliz
document.getElementById('Neutral').addEventListener('click', Refresco);     //Neutral
document.getElementById('Triste').addEventListener('click', Refresco);      //Triste
document.getElementById('Sorprendido').addEventListener('click', Refresco); //Sorprendido


//===================FIN ELEMENTOS INSTANCIACIÓN DOMM=======================


var emoval;  //Recoje del formulario la emocion
var Archivo_audio;   //Recoje del formulario el nombre del archivo de audio
var list_dir;  //Directorio al que se está accediendo

//Refresco y pintado de archivos de la sd según el directorio seleccionado en los desplegables
async function Refresco(){
  console.log(this.id);  //this devuelve la propiedad "id" del objeto seleccionado, directorio
  list_dir = this.id;
  var directorio = '/' + this.id;
  await Refresco_directorio(directorio);  //Refresca el directorio seleccionado

  traerJSON_SD();  //pide los nuevos ficheros del directorio
}
////////////////////// FIN /////////////////////////////////


/*
=============FUNCIONES RELACIONADAS CON EL FORMULARIO=================================
 */
//Limpia el campo de Texto del Formulario de Subida 
function limpiarCampoTexto() {
  document.getElementById("campoTexto").value = "";
}

//Borra la cache del formulario de subida
function limpiarFormSubida() {
  document.getElementById('form_datos').reset();
}
//=======================FIN FUNCIONES DE FORMULARIO===============================

/*
===============LEE EL JSON DEL ARCHIVO SPIFFS Y DIBUJA EN TABLA========================
*/
function traerJSON_SD(){ //Reacciona a los eventos del desplegable directorios
  list_dir = 'list_' + list_dir;
  console.log(list_dir);
  let res;
  fetch('dir.json')
    .then(res => res.json())
    .then(content =>{
      try {
        document.getElementById(list_dir).className = "center";  //Cambia la propiedad de la tabla para mostrarse

        res = document.getElementById(list_dir);
        res.innerHTML="";

        for (let item of content) { //Bucle rellena la tabla "Template"
          //Dibuja el HTML y llama desde el a la función "ReproducirSD", pasandole el nombre del fichero
          res.innerHTML += `
            <td>
              <a class="btn-floating waves-effect waves-light red lighten-1"
                href="javascript:ReproducirSD('${item.titulo}')">
                <i class="material-icons">play_arrow</i>
              </a>
            </td>

            <td>${item.titulo}</td>

            <td>
              <a class="btn-floating waves-effect waves-light purple lighten-3"
                href="javascript:BorrarDatos('${item.titulo}')">
                <i class="material-icons">clear</i>
              </a>
            </td>
          `
        }
        
      //===================== Solo un archivo en el directorio Seleccionado de la SD ===========
      } catch (error) {
          document.getElementById(list_dir).className = "center";

          res = document.getElementById(list_dir);
          res.innerHTML= "";

        //_____________Template String"________________
          //!Tabla dinámica que crea la lista con los archivos de la SD
          //Botón Reproductir, nombre fichero, botón eliminar
          res.innerHTML += `
            <td>
              <a class="btn-floating waves-effect waves-light red lighten-1"
                href="javascript:ReproducirSD('${content.titulo}')">
                <i class="material-icons">play_arrow</i>
              </a>
            </td>

            <td>${content.titulo}</td>

            <td>
              <a class="btn-floating waves-effect waves-light purple lighten-1"
                href="javascript:BorrarDatos('${content.titulo}')">
                <i class="material-icons">clear</i>
              </a>
            </td>
        `
      }
  })
  //=================== Si no hay Ficheros en el directorio ===================
  .catch(error =>{
    //console.log(error);
    document.getElementById(list_dir).className = "hide";
  });
}
//==================== FIN LLAMADA JSON ====================================



/*
==========================SUBIDA ARCHIVOS AL SD=================================
 */
//Recoge información del campo Emociones
emocion_val.addEventListener('change', (event)=>{
  emoval = event.target.value;
  console.log(emoval);
});

//Validaciones Subida Archivo al SD
fileInput.addEventListener('change', (event) => { //detecta cuando se sube un archivo botón carpeta
  Archivo_audio = (event.target.files[0]); //Accede a la informacion del fichero 
  console.log(Archivo_audio.name);

  var extensiones_p = ['mpeg', 'wav']; //Extensiones permitidas de los archivos

  //Funcion que convierte el tamaño a MEGAS
  var tamano_p = function (mega) {
    return Math.pow(2, 20) * mega;
  }

  var extension = Archivo_audio.type.split('/').pop(); //Separa / y Elimina del tipo "type: audio/mpeg"


  if (extensiones_p.indexOf(extension) != -1) { //Si encuentra un archivo con las extensiones admitidas...
    console.log('encontrado: ' + extension + ' ' + Archivo_audio.size);

    if (Archivo_audio.size < tamano_p(10)) { //Si su tamaño...
      console.log("ok");
    } else {
      M.toast({ //Avisos de ALERTA
        html: 'Archivo demasiado Grande',
        displayLength: 1500,
        classes: 'red lighten-1 rounded'
      });

      limpiarCampoTexto();
      limpiarFormSubida();
    }

  } else {
    M.toast({
      html: 'Archivo incorrecto: ' + extensiones_p.toString(),
      displayLength: 1500,
      classes: 'red lighten-1 rounded'
    });

    limpiarCampoTexto();
    limpiarFormSubida();
  }

  var Tam_nombre = Archivo_audio.name.split('.').shift(); //Toma el array del nombre, y lo separa de la extensión
  //console.log(Tam_nombre);

  if (Tam_nombre.length > 8) { //Si tiene más de 8 caracteres el nombre
    console.log("Nombre Largo");

    M.toast({
      html: 'Nombre Archivo demasiado Largo',
      displayLength: 1500,
      classes: 'red lighten-1 rounded'
    });

    limpiarCampoTexto();
    limpiarFormSubida();
  }

  if (Tam_nombre.indexOf(" ") !== -1) {  //No debe contener espacios
    console.log("No debe tener Espacios");

    M.toast({
      html: 'El Nombre del Archivo no puede contener Espacios',
      displayLength: 1500,
      classes: 'red lighten-1 rounded'
    });

    limpiarCampoTexto();
    limpiarFormSubida();
  }
});
//======================== FIN VALIDACIÓN ARCHIVOS =================================



//========================== SUBIDA DE ARCHIVOS AL SERVER ============================
const formulario = document.getElementById('form_datos');

formulario.addEventListener('submit', (event) => {
  event.preventDefault();

  if(fileInput.files.length == 0){  //Si intenta subir sin seleccionar archivo
    M.toast({
      html: 'selecciona un Archivo de Audio',
      displayLength: 1500,
      classes: 'red lighten-1 rounded'
    });
    return;
  }

  if(emocion_val.value == 0){  // Si intenta subir sin seeccionar Emoción
    M.toast({
      html: 'Debe seleccionar una Emoción',
      displayLength: 1500,
      classes: 'red lighten-1 rounded'
    });
    return;
  }
  
  //Envía el nombre del Archivo y la emocion al Server
  Audio_emo(Archivo_audio.name, emoval);

  const dataForm = new FormData(formulario);

  //Petición envio AJAX --Sube archivo de audio a la SD
  var xhr = new XMLHttpRequest();

  xhr.open('POST', '/subida');

  //Muestra la barra de Estado de la subida, cambia atributo "hide"
  document.getElementById('barra').className = 'center';
  document.getElementById('Btn_subida').className = 'hide';  //Desabilita el boton de subida
  //Progreso de subida del archivo
  xhr.upload.addEventListener("progress", (e)=>{  //Devuelve el porcentaje de subida del archivo
    const percent = e.lengthComputable ? (e.loaded / e.total) * 100 : 0; //Cálculo porcentaje de subida

    progress.style.width = percent.toFixed(2) + "%"; //Dibuja el porcentaje en la barra, propiedad "width"
  });

  //Archivo Subido
  xhr.onload = ()=>{
    if(xhr.status === 200){
      document.getElementById('barra').className = 'hide';  //Oculta la barra de subida
      document.getElementById('Btn_subida').className = "btn waves-effect waves-light right";  //Habilita el botón de subida
      limpiarCampoTexto();                //Borra el campo de texto del formulario
      limpiarFormSubida();

      M.toast({    //Archivo subido
        html: 'Archivo Subido',
        displayLength: 1500,
        classes: "purple lighten-3 rounded"
      });
    }else{
      M.toast({
        html: 'Imposible subir Archivo, Revise conexión',
        displayLength: 1500,
        classes: "red lighten-1 rounded"
      });
      limpiarCampoTexto();
      limpiarFormSubida();
      document.getElementById('barra').className = 'hide'; //oculta barra de progreso
    }
  }
  xhr.send(dataForm);
});

//============================FIN SUBIDA ARCHIVOS AL SD==================================


/* 
==================MANEJO CONEXIONES WEBSOCKET===================
 */
//____________CREACIÓN SOCKET DE CONEXIÓN__________________

//Se ejecuta al crear una conexión
function conexion_WS() {
  conexionWs = new WebSocket('ws://' + location.hostname + '/ws', ['arduino']);

  conexionWs.onopen = function () { //cuando se establece la conexión
    console.log("conexión abierta");
    M.toast({
      html: 'Conexión Abierta',
      displayLength: 2500,
      classes: "purple lighten-3 rounded"
    });

    //Cambio estado de color botones de conexión
    document.getElementById('conexionWs_off').className = 'btn waves-effect waves-light red lighten-1';
    document.getElementById('conexionWs_on').className = 'btn waves-effect waves-light purple lighten-1';
  };

  conexionWs.onerror = function (error) { //Error en la conexión
    console.log("WebSocket Error", error);
    M.toast({
      html: 'Imposible Conectarse',
      displayLength: 2500,
      classes: "red lighten-1 rounded"
    });
  };

  //!----------Aquí llegan los mensajes desde el SERVIDOR--------------------------------
  conexionWs.onmessage = function (event) {  //Cuando se recibe algún mensaje desde el Servidor
    console.log("Server: ", event.data);

    var datosINServidor = event.data;         //Reecoje los datos que llegan del Servidor
    dataJson = JSON.parse(datosINServidor);   //Variable global almacena el mensaje del Servidor y lo convierte a JSON

    //*-------COMPROBACIÓN DEL TIPO DE MENSAJE QUE LLEGA DESDE EL SERVIDOR----------------------
    //Datos sobre estado de conexión recibidos desde el Servidor
    if ("estado" in dataJson) { //Si encuentra la cadena "estado" dentro del documento JSON
      EstadoConexion();        //Recibe los parametros de estado e IP de la conexión
    }

    //Datos del estado de Reproducción recibidos desde el Servidor
    if("EREPRO" in dataJson){  //Si encuenta la cadena "EREPRO" dentro del JSON
      EstadoReproduccion();    //Recive el estado de reproducción de la SD
    }
  };
  //!---------------------------------------------------------------------------------

  conexionWs.onclose = function () { //Cuando se cierra la conexión
    console.log("conexion Cerrada");
    M.toast({
      html: 'Conexión Cerrada',
      displayLength: 2500,
      classes: "red lighten-1 rounded"
    });

    document.getElementById('conex').innerHTML = '';

    //Cambio Estado de color Botones Conexión
    document.getElementById('conexionWs_off').className = 'btn disabled';
    document.getElementById('conexionWs_on').className = 'btn waves-effect waves-light purple lighten-4';
  };
}

//Funcion de dexconexión del Socket
function desconectar() { //Llamada desde le botón Conexión OFF
  conexionWs.close();
}
/*
=============FIN FUNCIONES CONEXIÓN SOCKET=====================
 */


 /*
===============FUNCIONES ENVIO MENSAJES SOCKET==============================
*/
//Envía el nombre del fichero de audio y la emoción seleccionada en el formulario
function Audio_emo(audio, emocion){
  var audioEmo = {                //Objeto que recoje el nombre de la pista                    
    NOMBRE_AUDIO_FORM: audio,     //y la emoción seleccionadas en el formulario
    EMO_FORM: emocion
  };

  var envioAudioEmo = JSON.stringify(audioEmo);  //Convertido a JSON
  console.log(envioAudioEmo);
  conexionWs.send(envioAudioEmo);  //Enviado al server
}
//==============================

//Envio peticón de refresco listado de Archivos según el directorio de la SD
async function Refresco_directorio(directorio){
  var Refresh_directorio = {
    REFRESH: directorio
  };

  var jsonrefresh = JSON.stringify(Refresh_directorio);
  console.log(jsonrefresh);
  conexionWs.send(jsonrefresh);
}
//==============================

//Reproducir sd
function ReproducirSD(nombreFichero){
  //Pinta en Pantalla el Reproductor
  document.getElementById('t_repro').className = "centered";
  var activo = document.querySelector('#oculta_repro');
  activo.innerHTML = `
    <span style="color:#2196f3">${nombreFichero}</span>
  `
  //Envía el server la orden de reproducción del fichero seleccionado
  var objeto_repro = {
    PLAY: nombreFichero
  };
  //Convierte a JSON y envía
  var envioPlay = JSON.stringify(objeto_repro);
  conexionWs.send(envioPlay);
  console.log(nombreFichero);
}
//===================== FIN REPRODUCCIÓN SD ===================


//================= Borrado Ficheros SD ==========================
//Parámetro recibido desde los botones construidos en el template string
function BorrarDatos(nombreFichero){
  //crea un objeto para almacenar el nombre de fichero a borrar
  var obj_clear = {
    ERASE: nombreFichero
  };

  var envioDelete = JSON.stringify(obj_clear);  //convierte el objeto en JSON
  conexionWs.send(envioDelete);  //envia el json al servidor
}
//==================== FIN BORRADO ARCHIVOS SD =======================


//=================== BOTÓN STOP =====================================
function StopSD(){
  console.log('stop');
  document.getElementById('t_repro').className = "hide";
  var SD_stop = '{"STOP":' + 1 + '}';
  conexionWs.send(SD_stop);
}
//====================== FIN BOTÓN STOP ===========================

//=================== CONTROL VOLUMEN ===============
function envioVolumen(){
  const volumenSD = document.querySelector('#vol').value;  //Recoje el Valor del Slider
  console.log(volumenSD);
  var envioVolumenSD = '{"VOL":'+ volumenSD +'}';
  conexionWs.send(envioVolumenSD);
}
/////////////////////////////////


/*
=======================PETICIONES DATOS AL SERVIDOR==================================
*/
//Estado de la Conexión, enviado desde el Servidor
function EstadoConexion() {   //Captura los datos desde el JSON que el servidor envía
  var Estado = dataJson.estado;
  var ip = dataJson.IP;
  var hostname = dataJson.MDNS;

  if (Estado === 3) {   //Estado 3 = conectado
    console.log("conectado");
    console.log(ip);
    document.getElementById('conex').innerHTML =
                                                `
                                                <h6>Conectado a: 
                                                  ${hostname}
                                                </h6>
                                                <p>IP: ${ip}</p>
                                                `
  }
}
//=======================FIN ESTADO CONEXIÓN==========================================

//Estado de Reproducción
function EstadoReproduccion(){
  var EstadoRepro = dataJson.EREPRO;
  console.log(EstadoRepro);
  document.getElementById('t_repro').className = "hide";  //Borra el reproductor
}
//fin Estado de Reproducción
//===================== FIN PETICIONES DATOS AL SERVIDOR ========================