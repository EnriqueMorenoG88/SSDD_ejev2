#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include "claves.h"
#include "comunicacion.h"
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdarg.h>


// Inicializa el servicio de almacenaje de tuplas <clave-valor1-valor2-valor3>
int init(){

    char *ip_tuplas;
    ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable IP_TUPLAS no definida\n");
        return -1;
    }

    char *port_tuplas;
    port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable PORT_TUPLAS no definida\n");
        return -1;
    }

    int sock_client_fd;
    struct sockaddr_in serv_addr;
    short puerto = (short) atoi(port_tuplas);

    // Inicializamos a 0
    bzero((char *)&serv_addr, sizeof(serv_addr));


    // Dirección y puerto del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = strtoul(ip_tuplas, NULL, 10);
    serv_addr.sin_port = htons(puerto);

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
    if (connect(sock_client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo conectar con el socket del servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    //Imprime mensaje a enviar
    
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

    // Cerramos el socket
    if(close(sock_client_fd) == -1) {
        perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
        return -1;
    }

    if (respuesta)
        return -1;
    return 0;

}



// Insercion del elemento <clave-valor1-valor2-valor3>
int set_value(int key, char* value1, int value2, double value3){
    

    char *ip_tuplas;
    ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable IP_TUPLAS no definida\n");
        return -1;
    }

    char *port_tuplas;
    port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable PORT_TUPLAS no definida\n");
        return -1;
    }

    int sock_client_fd;
    struct sockaddr_in serv_addr;
    short puerto = (short) atoi(port_tuplas);

    // Inicializamos a 0
    bzero((char *)&serv_addr, sizeof(serv_addr));


    // Dirección y puerto del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = strtoul(ip_tuplas, NULL, 10);
    serv_addr.sin_port = htons(puerto);

     // Crear elementos de la petición y respuesta
    char operacion = 's';
    
    //char *clave;
    int32_t clave = htonl(key);

    char valor1[256], valor3[512];

    strcpy(valor1, value1);
    sprintf(valor3, "%lf", value3);

    int32_t valor2 = htonl(value2);
    int32_t respuesta;


    // Revisar si value1 de tupla cumple con el requesito de maximo 255 caracteres (excluido '\0')
    if (strlen(value1) > 256 || (strlen(value1) == 256 && value1[255] != '\0')){
        perror("[CLIENTE][ERROR] La longitud maxima de value1 es 256 caracteres, siendo el nº 256 exclusivo para \'\\0\'\n");
        return -1;
    }

    // Creación socket cliente
    if ((sock_client_fd = socket(AF_INET, SOCK_STREAM, 0))==-1) {
        perror("[CLIENTE][ERROR] No se pudo crear socket de cliente\n");
        return -1;
    }

    // Conectamos con servidor
    if (connect(sock_client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[CLIENTE][ERROR] No se pudo conectar con el socket del servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos la operacion
    if (sendMessage(sock_client_fd, (char*) &operacion, sizeof(char)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos la key
    if(sendMessage(sock_client_fd, (char*) &clave, sizeof(int32_t)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos el value1
    if (sendMessage(sock_client_fd, valor1, strlen(valor1)+1) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos el value2
    if(sendMessage(sock_client_fd, (char*) &valor2, sizeof(int32_t)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos el value3
    if (sendMessage(sock_client_fd, valor3, strlen(valor3)+1) == -1) {
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
    
    // Cerramos el socket
    if(close(sock_client_fd) == -1) {
        perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
        return -1;
    }

    // Comprobamos respuesta
    if (respuesta)
        return -1;
    return 0;
}

// Obtencion de los valores asociados a la clave proporcionada
int get_value(int key, char* value1, int* value2, double* value3){

    char *ip_tuplas;
    ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable IP_TUPLAS no definida\n");
        return -1;
    }

    char *port_tuplas;
    port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable PORT_TUPLAS no definida\n");
        return -1;
    }

    int sock_client_fd;
    struct sockaddr_in serv_addr;
    short puerto = (short) atoi(port_tuplas);

    // Inicializamos a 0
    bzero((char *)&serv_addr, sizeof(serv_addr));


    // Dirección y puerto del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = strtoul(ip_tuplas, NULL, 10);
    serv_addr.sin_port = htons(puerto);

    // Creación socket cliente
    if ((sock_client_fd = socket(AF_INET, SOCK_STREAM, 0))==-1) {
        perror("[CLIENTE][ERROR] No se pudo crear socket de cliente\n");
        return -1;
    }

    // Conectamos con servidor
    if (connect(sock_client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[CLIENTE][ERROR] No se pudo conectar con el socket del servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Crear elementos de la petición y respuesta
    char operacion = 'g';
    
    int32_t clave = htonl(key);

    // Mandamos la operacion
    if (sendMessage(sock_client_fd, (char*) &operacion, sizeof(char)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos la key
    if(sendMessage(sock_client_fd, (char*) &clave, sizeof(int32_t)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    int32_t respuesta, valor2;
    char valor1[256], valor3[512];

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

    if (respuesta == 0){
        
        if (readLine(sock_client_fd, valor1, sizeof(valor1)) == -1) {
            perror("[CLIENTE][ERROR] No se pudo recibir la respuesta del servidor\n");
            if(close(sock_client_fd) == -1) {
                perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
                return -1;
            }
            return -1;
        }
        
        strcpy(value1, valor1);

        if (recvMessage(sock_client_fd, (char*) &valor2, sizeof(int32_t)) == -1) {
            perror("[CLIENTE][ERROR] No se pudo recibir la respuesta del servidor\n");
            if(close(sock_client_fd) == -1) {
                perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
                return -1;
            }
            return -1;
        }

        *value2 = ntohl(valor2);

        if (readLine(sock_client_fd, valor3, sizeof(valor3)) == -1) {
            perror("[CLIENTE][ERROR] No se pudo recibir la respuesta del servidor\n");
            if(close(sock_client_fd) == -1) {
                perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
                return -1;
            }
            return -1;
        }

        sscanf(valor3, "%lf", value3);
    }
    
    // Cerramos el socket
    if(close(sock_client_fd) == -1) {
        perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
        return -1;
    }

    // Comprobamos respuesta
    if (respuesta)
        return -1;
    return 0;
}


// Modificacion de los valores asociados a la clave proporcionada
int modify_value(int key, char* value1, int value2, double value3){

    char *ip_tuplas;
    ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable IP_TUPLAS no definida\n");
        return -1;
    }

    char *port_tuplas;
    port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable PORT_TUPLAS no definida\n");
        return -1;
    }

    int sock_client_fd;
    struct sockaddr_in serv_addr;
    short puerto = (short) atoi(port_tuplas);

    // Inicializamos a 0
    bzero((char *)&serv_addr, sizeof(serv_addr));


    // Dirección y puerto del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = strtoul(ip_tuplas, NULL, 10);
    serv_addr.sin_port = htons(puerto);


    char operacion = 'm';
    
    //char *clave;
    int32_t clave = htonl(key);

    char valor1[256], valor3[512];

    strcpy(valor1, value1);
    sprintf(valor3, "%lf", value3);

    int32_t valor2 = htonl(value2);
    int32_t respuesta;

    // Revisar si value1 de tupla cumple con el requesito de maximo 255 caracteres (excluido '\0')
    if (strlen(value1) > 256 || (strlen(value1) == 256 && value1[255] != '\0')){
        perror("[CLIENTE][ERROR] La longitud maxima de value1 es 256 caracteres, siendo el nº 256 exclusivo para \'\\0\'\n");
        return -1;
    }

    // Creación socket cliente
    if ((sock_client_fd = socket(AF_INET, SOCK_STREAM, 0))==-1) {
        perror("[CLIENTE][ERROR] No se pudo crear socket de cliente\n");
        return -1;
    }

    // Conectamos con servidor
    if (connect(sock_client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[CLIENTE][ERROR] No se pudo conectar con el socket del servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos la operacion
    if (sendMessage(sock_client_fd, (char*) &operacion, sizeof(char)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos la key
    if(sendMessage(sock_client_fd, (char*) &clave, sizeof(int32_t)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos el value1
    if (sendMessage(sock_client_fd, valor1, strlen(valor1)+1) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos el value2
    if(sendMessage(sock_client_fd, (char*) &valor2, sizeof(int32_t)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos el value3
    if (sendMessage(sock_client_fd, valor3, strlen(valor3)+1) == -1) {
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
    
    // Cerramos el socket
    if(close(sock_client_fd) == -1) {
        perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
        return -1;
    }

    // Comprobamos respuesta
    if (respuesta)
        return -1;
    return 0;
}
    
// Eliminacion de la tupla asociada a la clave proporcionada
int delete_key(int key){

    char *ip_tuplas;
    ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable IP_TUPLAS no definida\n");
        return -1;
    }

    char *port_tuplas;
    port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable PORT_TUPLAS no definida\n");
        return -1;
    }

    int sock_client_fd;
    struct sockaddr_in serv_addr;
    short puerto = (short) atoi(port_tuplas);

    // Inicializamos a 0
    bzero((char *)&serv_addr, sizeof(serv_addr));


    // Dirección y puerto del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = strtoul(ip_tuplas, NULL, 10);
    serv_addr.sin_port = htons(puerto);

    // Creación socket cliente
    if ((sock_client_fd = socket(AF_INET, SOCK_STREAM, 0))==-1) {
        perror("[CLIENTE][ERROR] No se pudo crear socket de cliente\n");
        return -1;
    }

    // Conectamos con servidor
    if (connect(sock_client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[CLIENTE][ERROR] No se pudo conectar con el socket del servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Crear elementos de la petición y respuesta
    char operacion = 'd';
    
    int32_t clave = htonl(key);

    int32_t respuesta;

    // Mandamos la operacion
    if (sendMessage(sock_client_fd, (char*) &operacion, sizeof(char)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos la key
    if(sendMessage(sock_client_fd, (char*) &clave, sizeof(int32_t)) == -1) {
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
    
    // Cerramos el socket
    if(close(sock_client_fd) == -1) {
        perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
        return -1;
    }

    // Comprobamos respuesta
    if (respuesta)
        return -1;
    return 0;
}



// Comprobacion de la existencia de algun elemento asociado a la clave proporcionada
int exist(int key){
    char *ip_tuplas;
    ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable IP_TUPLAS no definida\n");
        return -1;
    }

    char *port_tuplas;
    port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable PORT_TUPLAS no definida\n");
        return -1;
    }

    int sock_client_fd;
    struct sockaddr_in serv_addr;
    short puerto = (short) atoi(port_tuplas);

    // Inicializamos a 0
    bzero((char *)&serv_addr, sizeof(serv_addr));


    // Dirección y puerto del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = strtoul(ip_tuplas, NULL, 10);
    serv_addr.sin_port = htons(puerto);

    // Creación socket cliente
    if ((sock_client_fd = socket(AF_INET, SOCK_STREAM, 0))==-1) {
        perror("[CLIENTE][ERROR] No se pudo crear socket de cliente\n");
        return -1;
    }

    // Conectamos con servidor
    if (connect(sock_client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[CLIENTE][ERROR] No se pudo conectar con el socket del servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Crear elementos de la petición y respuesta
    char operacion = 'e';
    
    int32_t clave = htonl(key);

    int32_t respuesta;

    // Mandamos la operacion
    if (sendMessage(sock_client_fd, (char*) &operacion, sizeof(char)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos la key
    if(sendMessage(sock_client_fd, (char*) &clave, sizeof(int32_t)) == -1) {
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
    
    // Cerramos el socket
    if(close(sock_client_fd) == -1) {
        perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
        return -1;
    }

    // Comprobamos respuesta
    switch(respuesta){
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
    char *ip_tuplas;
    ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable IP_TUPLAS no definida\n");
        return -1;
    }

    char *port_tuplas;
    port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL){
        perror("[CLIENTE][ERROR] Variable PORT_TUPLAS no definida\n");
        return -1;
    }

    int sock_client_fd;
    struct sockaddr_in serv_addr;
    short puerto = (short) atoi(port_tuplas);

    // Inicializamos a 0
    bzero((char *)&serv_addr, sizeof(serv_addr));


    // Dirección y puerto del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = strtoul(ip_tuplas, NULL, 10);
    serv_addr.sin_port = htons(puerto);

    // Creación socket cliente
    if ((sock_client_fd = socket(AF_INET, SOCK_STREAM, 0))==-1) {
        perror("[CLIENTE][ERROR] No se pudo crear socket de cliente\n");
        return -1;
    }

    // Conectamos con servidor
    if (connect(sock_client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[CLIENTE][ERROR] No se pudo conectar con el socket del servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Crear elementos de la petición y respuesta
    char operacion = 'c';
    
    int32_t clave1 = htonl(key1);
    int32_t clave2 = htonl(key2);

    int32_t respuesta;

    // Mandamos la operacion
    if (sendMessage(sock_client_fd, (char*) &operacion, sizeof(char)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos la key1
    if(sendMessage(sock_client_fd, (char*) &clave1, sizeof(int32_t)) == -1) {
        perror("[CLIENTE][ERROR] No se pudo mandar la operacion al servidor\n");
        if(close(sock_client_fd) == -1) {
            perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
            return -1;
        }
        return -1;
    }

    // Mandamos la key2
    if(sendMessage(sock_client_fd, (char*) &clave2, sizeof(int32_t)) == -1) {
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
    
    // Cerramos el socket
    if(close(sock_client_fd) == -1) {
        perror("[CLIENTE][ERROR] No se pudo cerrar el socket del cliente\n");
        return -1;
    }

    // Comprobamos respuesta
    if (respuesta)
        return -1;
    return 0;
}