#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include "claves.h"
#include "comunicacion.h"


// Inicializa el servicio de almacenaje de tuplas <clave-valor1-valor2-valor3>
int init(){
    
    mqd_t cola_cliente;
    struct mq_attr qattr_cliente;
    qattr_cliente.mq_maxmsg = MAX_MSG;
    qattr_cliente.mq_msgsize = sizeof(Service);

    mqd_t cola_servidor;

    Service respuesta;      // Servicio de respuesta recibido del servidor
    Request peticion;       // Solicitud del cliente al servidor
    peticion.op = 'i';

    // Generar nombre de la cola cliente
    sprintf(peticion.qclient, "/COLA_CLIENTE_%d", getpid());

    // Abrir cola del servidor
    if ((cola_servidor = mq_open("/COLA_SERVIDOR", O_WRONLY)) == -1){
            perror("[CLIENTE][ERROR] Cola del servidor no pudo abrirse\n");
            return -1;
    }

    // Crear cola cliente
    if ((cola_cliente = mq_open(peticion.qclient, O_CREAT|O_RDONLY, 0700, &qattr_cliente)) == -1){
        perror("[CLIENTE][ERROR] Cola del cliente no pudo crearse\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        return -1;
    }

    // Mandar peticion al servidor
    if (mq_send(cola_servidor, (const char*) &peticion, sizeof(Request), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo enviarse la peticion\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Recepcion de respuesta del servidor
    if (mq_receive(cola_cliente, (char*) &respuesta, sizeof(Service), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo recibirse la respuesta\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Cerrar cola del servidor
    if (mq_close(cola_servidor) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }

    // Cerrar cola del cliente
    if (mq_close(cola_cliente) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }
    
    // Desvincular la cola del cliente
    if (mq_unlink(peticion.qclient) == -1){
        perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }
    
    if (respuesta.status)
        return -1;
    return 0;
}



// Insercion del elemento <clave-valor1-valor2-valor3>
int set_value(int key, char* value1, int value2, double value3){

    mqd_t cola_cliente;
    struct mq_attr qattr_cliente;
    qattr_cliente.mq_maxmsg = MAX_MSG;
    qattr_cliente.mq_msgsize = sizeof(Service);

    mqd_t cola_servidor;
    Service respuesta;          // Servicio de respuesta recibido del servidor
    Tuple contenido;            // Contenido de tupla a enviar en la peticion

    // Revisar si value1 de tupla cumple con el requesito de maximo 255 caracteres (excluido '\0')
    if (strlen(value1) > 256 || (strlen(value1) == 256 && value1[255] != '\0')){
        perror("[CLIENTE][ERROR] La longitud maxima de value1 es 256 caracteres, siendo el nº 256 exclusivo para \'\\0\'\n");
        return -1;
    }

    contenido.key = key;
    strcpy(contenido.value1, value1);
    contenido.value2 = value2;
    contenido.value3 = value3;

    Request peticion;       // Solicitud del cliente al servidor
    
    peticion.op = 's';
    peticion.content = contenido;

    // Generar nombre de la cola cliente
    sprintf(peticion.qclient, "/COLA_CLIENTE_%d", getpid());

    // Abrir cola del servidor
    if ((cola_servidor = mq_open("/COLA_SERVIDOR", O_WRONLY)) == -1){
            perror("[CLIENTE][ERROR] Cola del servidor no pudo abrirse\n");
            return -1;
    }

    // Crear cola cliente
    if ((cola_cliente = mq_open(peticion.qclient, O_CREAT|O_RDONLY, 0700, &qattr_cliente)) == -1){
        perror("[CLIENTE][ERROR] Cola del cliente no pudo crearse\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        return -1;
    }

    // Mandar peticion al servidor
    if (mq_send(cola_servidor, (const char*) &peticion, sizeof(Request), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo enviarse la peticion\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Recepcion de respuesta del servidor
    if (mq_receive(cola_cliente, (char*) &respuesta, sizeof(Service), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo recibirse la respuesta\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Cerrar cola del servidor
    if (mq_close(cola_servidor) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }

    // Cerrar cola del cliente
    if (mq_close(cola_cliente) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }
    
    // Desvincular la cola del cliente
    if (mq_unlink(peticion.qclient) == -1){
        perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }

    if (respuesta.status)
        return -1;
    return 0;
}



// Obtencion de los valores asociados a la clave proporcionada
int get_value(int key, char* value1, int* value2, double* value3){

    mqd_t cola_cliente;
    struct mq_attr qattr_cliente;
    qattr_cliente.mq_maxmsg = MAX_MSG;
    qattr_cliente.mq_msgsize = sizeof(Service);

    mqd_t cola_servidor;
    Service respuesta;          // Servicio de respuesta recibido del servidor
    Request peticion;           // Solicitud del cliente al servidor

    peticion.op = 'g';
    peticion.content.key = key;
    
    // Generar nombre de la cola cliente
    sprintf(peticion.qclient, "/COLA_CLIENTE_%d", getpid());

    // Abrir cola del servidor
    if ((cola_servidor = mq_open("/COLA_SERVIDOR", O_WRONLY)) == -1){
            perror("[CLIENTE][ERROR] Cola del servidor no pudo abrirse\n");
            return -1;
    }

    // Crear cola cliente
    if ((cola_cliente = mq_open(peticion.qclient, O_CREAT|O_RDONLY, 0700, &qattr_cliente)) == -1){
        perror("[CLIENTE][ERROR] Cola del cliente no pudo crearse\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        return -1;
    }

    // Mandar peticion al servidor
    if (mq_send(cola_servidor, (const char*) &peticion, sizeof(Request), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo enviarse la peticion\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Recepcion de respuesta del servidor
    if (mq_receive(cola_cliente, (char*) &respuesta, sizeof(Service), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo recibirse la respuesta\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Cerrar cola del servidor
    if (mq_close(cola_servidor) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }

    // Cerrar cola del cliente
    if (mq_close(cola_cliente) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }
    
    // Desvincular la cola del cliente
    if (mq_unlink(peticion.qclient) == -1){
        perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }

    if (respuesta.status)
        return -1;

    strcpy(value1, respuesta.content.value1);
    *value2 = respuesta.content.value2;
    *value3 = respuesta.content.value3;

    return 0;
}



// Modificacion de los valores asociados a la clave proporcionada
int modify_value(int key, char* value1, int value2, double value3){
    
    mqd_t cola_cliente;
    struct mq_attr qattr_cliente;
    qattr_cliente.mq_maxmsg = MAX_MSG;
    qattr_cliente.mq_msgsize = sizeof(Service);

    mqd_t cola_servidor;
    Service respuesta;          // Servicio de respuesta recibido del servidor            
    Tuple contenido;            // Contenido de tupla a enviar en la peticion
    
    // Revisar si value1 de tupla cumple con el requesito de maximo 255 caracteres (excluido '\0')
    if (strlen(value1) > 256 || (strlen(value1) == 256 && value1[255] != '\0')){
        perror("[CLIENTE][ERROR] La longitud maxima de value1 es 256 caracteres, siendo el nº 256 exclusivo para \'\\0\'\n");
        return -1;
    }
    
    contenido.key = key;
    strcpy(contenido.value1, value1);
    contenido.value2 = value2;
    contenido.value3 = value3; 

    Request peticion;           // Solicitud del cliente al servidor

    peticion.op = 'm';
    peticion.content = contenido;
    
    // Generar nombre de la cola cliente
    sprintf(peticion.qclient, "/COLA_CLIENTE_%d", getpid());

    // Abrir cola del servidor
    if ((cola_servidor = mq_open("/COLA_SERVIDOR", O_WRONLY)) == -1){
            perror("[CLIENTE][ERROR] Cola del servidor no pudo abrirse\n");
            return -1;
    }

    // Crear cola cliente
    if ((cola_cliente = mq_open(peticion.qclient, O_CREAT|O_RDONLY, 0700, &qattr_cliente)) == -1){
        perror("[CLIENTE][ERROR] Cola del cliente no pudo crearse\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        return -1;
    }

    // Mandar peticion al servidor
    if (mq_send(cola_servidor, (const char*) &peticion, sizeof(Request), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo enviarse la peticion\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Recepcion de respuesta del servidor
    if (mq_receive(cola_cliente, (char*) &respuesta, sizeof(Service), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo recibirse la respuesta\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Cerrar cola del servidor
    if (mq_close(cola_servidor) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }

    // Cerrar cola del cliente
    if (mq_close(cola_cliente) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }
    
    // Desvincular la cola del cliente
    if (mq_unlink(peticion.qclient) == -1){
        perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }

    if (respuesta.status)
        return -1;
    return 0;
}



// Eliminacion de la tupla asociada a la clave proporcionada
int delete_key(int key){

    mqd_t cola_cliente;
    struct mq_attr qattr_cliente;
    qattr_cliente.mq_maxmsg = MAX_MSG;
    qattr_cliente.mq_msgsize = sizeof(Service);

    mqd_t cola_servidor;

    Service respuesta;      // Servicio de respuesta recibido del servidor
    Request peticion;       // Solicitud del cliente al servidor

    peticion.op = 'd';
    peticion.content.key = key;

    // Generar nombre de la cola cliente
    sprintf(peticion.qclient, "/COLA_CLIENTE_%d", getpid());

    // Abrir cola del servidor
    if ((cola_servidor = mq_open("/COLA_SERVIDOR", O_WRONLY)) == -1){
            perror("[CLIENTE][ERROR] Cola del servidor no pudo abrirse\n");
            return -1;
    }

    // Crear cola cliente
    if ((cola_cliente = mq_open(peticion.qclient, O_CREAT|O_RDONLY, 0700, &qattr_cliente)) == -1){
        perror("[CLIENTE][ERROR] Cola del cliente no pudo crearse\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        return -1;
    }

    // Mandar peticion al servidor
    if (mq_send(cola_servidor, (const char*) &peticion, sizeof(Request), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo enviarse la peticion\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Recepcion de respuesta del servidor
    if (mq_receive(cola_cliente, (char*) &respuesta, sizeof(Service), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo recibirse la respuesta\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Cerrar cola del servidor
    if (mq_close(cola_servidor) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }

    // Cerrar cola del cliente
    if (mq_close(cola_cliente) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }
    
    // Desvincular la cola del cliente
    if (mq_unlink(peticion.qclient) == -1){
        perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }

    if (respuesta.status == -1)
        return -1;
    return 0;
}



// Comprobacion de la existencia de algun elemento asociado a la clave proporcionada
int exist(int key){

    mqd_t cola_cliente;
    struct mq_attr qattr_cliente;
    qattr_cliente.mq_maxmsg = MAX_MSG;
    qattr_cliente.mq_msgsize = sizeof(Service);
    
    mqd_t cola_servidor;

    Service respuesta;      // Servicio de respuesta recibido del servidor
    Request peticion;       // Solicitud del cliente al servidor

    peticion.op = 'e';
    peticion.content.key = key;

    // Generar nombre de la cola cliente
    sprintf(peticion.qclient, "/COLA_CLIENTE_%d", getpid());

    // Abrir cola del servidor
    if ((cola_servidor = mq_open("/COLA_SERVIDOR", O_WRONLY)) == -1){
            perror("[CLIENTE][ERROR] Cola del servidor no pudo abrirse\n");
            return -1;
    }

    // Crear cola cliente
    if ((cola_cliente = mq_open(peticion.qclient, O_CREAT|O_RDONLY, 0700, &qattr_cliente)) == -1){
        perror("[CLIENTE][ERROR] Cola del cliente no pudo crearse\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        return -1;
    }

    // Mandar peticion al servidor
    if (mq_send(cola_servidor, (const char*) &peticion, sizeof(Request), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo enviarse la peticion\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Recepcion de respuesta del servidor
    if (mq_receive(cola_cliente, (char*) &respuesta, sizeof(Service), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo recibirse la respuesta\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Cerrar cola del servidor
    if (mq_close(cola_servidor) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }

    // Cerrar cola del cliente
    if (mq_close(cola_cliente) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }
    
    // Desvincular la cola del cliente
    if (mq_unlink(peticion.qclient) == -1){
        perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }

    switch(respuesta.status){
        case 1:
            return 1;
        
        case 0:
            return 0;

        default:
            return -1;
    }
}


// Creacion e insercion de un nuevo elemento con la segunda clave proporcionada, copiando los valores de la primera
int copy_key(int key1, int key2){

    mqd_t cola_cliente;
    struct mq_attr qattr_cliente;
    qattr_cliente.mq_maxmsg = MAX_MSG;
    qattr_cliente.mq_msgsize = sizeof(Service);

    mqd_t cola_servidor;

    Service respuesta;      // Servicio de respuesta recibido del servidor
    Request peticion;       // Solicitud del cliente al servidor

    peticion.op = 'c';
    peticion.content.key = key1;
    peticion.second_key = key2;

    // Generar nombre de la cola cliente
    sprintf(peticion.qclient, "/COLA_CLIENTE_%d", getpid());

    // Abrir cola del servidor
    if ((cola_servidor = mq_open("/COLA_SERVIDOR", O_WRONLY)) == -1){
            perror("[CLIENTE][ERROR] Cola del servidor no pudo abrirse\n");
            return -1;
    }

    // Crear cola cliente
    if ((cola_cliente = mq_open(peticion.qclient, O_CREAT|O_RDONLY, 0700, &qattr_cliente)) == -1){
        perror("[CLIENTE][ERROR] Cola del cliente no pudo crearse\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        return -1;
    }

    // Mandar peticion al servidor
    if (mq_send(cola_servidor, (const char*) &peticion, sizeof(Request), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo enviarse la peticion\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Recepcion de respuesta del servidor
    if (mq_receive(cola_cliente, (char*) &respuesta, sizeof(Service), 0) == -1){
        perror("[CLIENTE][ERROR] No pudo recibirse la respuesta\n");
        if (mq_close(cola_servidor) == -1)
            perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1; 
    }

    // Cerrar cola del servidor
    if (mq_close(cola_servidor) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_close(cola_cliente) == -1)
            perror("[CLIENTE][ERROR] Cola del cliente no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }

    // Cerrar cola del cliente
    if (mq_close(cola_cliente) == -1){
        perror("[CLIENTE][ERROR] Cola del servidor no pudo cerrarse\n");
        if (mq_unlink(peticion.qclient) == -1)
            perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }
    
    // Desvincular la cola del cliente
    if (mq_unlink(peticion.qclient) == -1){
        perror("[CLIENTE][ERROR] Cola cliente no pudo desvincularse\n");
        return -1;
    }

    if (respuesta.status) 
        return -1;
    return 0;
}