#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include "claves.h"
#include "comunicacion.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdarg.h>


// Inicializa el servicio de almacenaje de tuplas <clave-valor1-valor2-valor3>
int init(){

    int sock_client_fd;
    struct sockaddr_in serv_addr;

    // Inicializamos a 0
    bzero((char *)&serv_addr, sizeof(serv_addr));

    // Dirección y puerto del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("localhost");
    serv_addr.sin_port = htons(8080);

    // Crear petición y respuesta
    char operacion = 'i';
    //char respuesta[3];
    int32_t respuesta;

    // Creación socket cliente
    if ((sock_client_fd = socket(AF_INET, SOCK_STREAM, 0))==-1) {
        perror("[CLIENTE][ERROR] No se pudo crear socket de cliente\n");
        return -1;
    }

    // Conectamos con servidor
    if (connect(sock_client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[CLIENTE][ERROR] No se pudo conectar con el shocket del servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    //Imprime mensaje a enviar
    printf("[CLIENTE]Mensaje a enviar: %c", operacion);
    
    // Mandamos la petición
    if (sendMessage(sock_client_fd, (char*) &operacion, sizeof(char)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la petición al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Recibimos la respuesta
    if (recvMessage(sock_client_fd, (char*) &respuesta, sizeof(int32_t)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo recibir la respuesta del servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    respuesta = ntohl(respuesta);
    //Imprime mensaje recibido
    //printf("[CLIENTE] Mensaje recibido: %s", respuesta);
    printf("[CLIENTE] Mensaje recibido: %d", respuesta);

    // Cerramos el socket
    if(close(sock_client_fd) == -1) {
        perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
        return -1;
    }

    // Comprobamos respuesta
    /*if (strcmp(respuesta,"0"))
        return 0;
    return -1;*/
    if (respuesta)
        return -1;
    return 0;

    /*
    mqd_t cola_cliente;
    struct mq_attr qattr_cliente;
    qattr_cliente.mq_maxmsg = MAX_MSG;
    qattr_cliente.mq_msgsize = sizeof(Service);

    mqd_t cola_servidor;

    Service respuesta;      // Servicio de respuesta recibido del servidor
    Request peticion;       // Solicitud del cliente al servidor
    peticion.op = 'i';
    */

    // Generar nombre de la cola cliente
    //sprintf(peticion.qclient, "/COLA_CLIENTE_%d", getpid());

    /*
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
    return 0;*/
}



// Insercion del elemento <clave-valor1-valor2-valor3>
int set_value(int key, char* value1, int value2, double value3){

    // PETICIÓN CON UNA SOLA STRING
    /*char *peticion;
    asprintf(&peticion, "%d%s%d%lf", key, value1, value2, value3);
    free(peticion)
    */

    int sock_client_fd;
    struct sockaddr_in serv_addr;

    // Inicializamos a 0
    bzero((char *)&serv_addr, sizeof(serv_addr));

    // Dirección y puerto del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("localhost");
    serv_addr.sin_port = htons(8080);

    // Creación socket cliente
    if ((sock_client_fd = socket(AF_INET, SOCK_STREAM, 0))==-1) {
        perror("[CLIENTE][ERROR] No se pudo crear socket de cliente\n");
        return -1;
    }

    // Conectamos con servidor
    if (connect(sock_client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[CLIENTE][ERROR] No se pudo conectar con el shocket del servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Crear elementos de la petición y respuesta
    char operacion = 's';
    
    //char *clave;
    int32_t clave = htonl(key);

    /*if (asprintf(&clave, "%d", key) == -1){
        perror("[CLIENTE][ERROR] No se pudo convertir a cadena la key\n");
        return -1; 
    }*/

    char valor1[256];
    strcpy(valor1, value1);

    //char *valor2;
    int32_t valor2 = htonl(value2);
    /*if (asprintf(&valor2, "%d", value2) == -1){
        perror("[CLIENTE][ERROR] No se pudo convertir a cadena el value2\n");
        free(clave);
        return -1; 
    }

    char *valor3;*/
    uint64_t value3_uint = *(uint64_t*) &value3;
    double valor3 = (double) ((uint64_t) htonl(value3_uint >> 32) << 32 | htonl(value3_uint & 0xFFFFFFFF));
   
    //double valor3 = (double)((uint64_t) htonl(value3 >> 32) << 32 | htonl(value3 & 0xFFFFFFFF));
    
    /*if (asprintf(&valor3, "%lf", value3) == -1){
        perror("[CLIENTE][ERROR] No se pudo convertir a cadena el value3\n");
        free(clave);
        free(valor2);
        return -1; 
    }
    
    char respuesta[3];*/
    int32_t respuesta;

    //Imprime mensaje a enviar
    printf("[CLIENTE]Mensaje a enviar: %c", operacion);

    // Mandamos la operacion
    if (sendMessage(sock_client_fd, (char*) &operacion, sizeof(char)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            /*free(clave);
            free(valor2);
            free(valor3);*/
            return -1;
        }
        /*free(clave);
        free(valor2);
        free(valor3);*/
        return -1;
    }
    //Imprime mensaje a enviar
    //printf("[CLIENTE]Mensaje a enviar: %s", valor2);
    printf("[CLIENTE]Mensaje a enviar: %d", key);

    // Mandamos la key
    /*if (sendMessage(sock_client_fd, clave, sizeof(clave)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            free(clave);
            free(valor2);
            free(valor3);
            return -1;
        }
        free(clave);
        free(valor2);
        free(valor3);
        return -1;
    }*/

    if(sendMessage(sock_client_fd, (char*) &clave, sizeof(int32_t)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    //free(clave);

    //Imprime mensaje a enviar
    printf("[CLIENTE]Mensaje a enviar: %s", valor1);

    // Mandamos el value1
    if (sendMessage(sock_client_fd, valor1, sizeof(valor1)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            /*free(valor2);
            free(valor3);*/
            return -1;
        }
        //free(valor2);
        //free(valor3);
        return -1;
    }

    //Imprime mensaje a enviar
    //printf("[CLIENTE]Mensaje a enviar: %s", valor2);
    printf("[CLIENTE]Mensaje a enviar: %d", value2);

    // Mandamos el value2
    /*if (sendMessage(sock_client_fd, valor2, sizeof(valor2)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            free(valor2);
            free(valor3);
            return -1;
        }
        free(valor2);
        free(valor3);
        return -1;
    }

    free(valor2);*/
    if(sendMessage(sock_client_fd, (char*) &valor2, sizeof(int32_t)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    //Imprime mensaje a enviar
    //printf("[CLIENTE]Mensaje a enviar: %s", valor3);
    printf("[CLIENTE]Mensaje a enviar: %lf", value3);

    // Mandamos el value3
    /*if (sendMessage(sock_client_fd, valor3, sizeof(valor3)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            free(valor3);
            return -1;
        }
        free(valor3);
        return -1;
    }

    free(valor3);*/
    if (sendMessage(sock_client_fd, (char*) &valor3, sizeof(uint64_t)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Recibimos la respuesta
    if (recvMessage(sock_client_fd, (char*) &respuesta, sizeof(respuesta)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo recibir la respuesta del servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    respuesta = ntohl(respuesta);
    //Imprime mensaje recibido
    //printf("[CLIENTE] Mensaje recibido: %s", respuesta);
    printf("[CLIENTE] Mensaje recibido: %d", respuesta);
    
    // Cerramos el socket
    if(close(sock_client_fd) == -1) {
        perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
        return -1;
    }

    // Comprobamos respuesta
    /*if (strcmp(respuesta,"0"))
        return 0;
    return -1;*/
    if (respuesta)
        return -1;
    return 0;

    /*
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
    return 0;*/
}

/*

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
}*/