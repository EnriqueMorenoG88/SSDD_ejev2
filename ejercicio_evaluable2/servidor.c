#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "comunicacion.h"


int mensaje_no_copiado = 1;                                 // Condicion para el mutex de paso de mensajes
pthread_mutex_t tuples_mutex = PTHREAD_MUTEX_INITIALIZER;   // Mutex para ejecucion de operaciones en direcotrio y ficheros
pthread_mutex_t mutex_msg = PTHREAD_MUTEX_INITIALIZER;      // Mutex para control de paso de mensajes
pthread_cond_t condvar_msg = PTHREAD_COND_INITIALIZER;      // Var. condicion asociada al mutex de paso de mensajes



// Prototipos de la API del lado servidor
int init_serv();
int set_value_serv(int key, char* value1, int value2, double value3);
int get_value_serv(int key, char* value1, int* value2, double* value3);
int modify_value_serv(int key, char* value1, int value2, double value3);
int delete_key_serv(int key);
int exist_serv(int key);
int copy_key_serv(int key1, int key2);


// Atender la peticion del cliente
void cumplir_pet (void* pet){

    Request peticion;
    Service respuesta;
    //mqd_t cola_cliente;

    // Hilo obtiene la peticion del cliente
    pthread_mutex_lock(&mutex_msg);
    peticion = (*(Request *) pet);
    mensaje_no_copiado = 0;
    pthread_cond_signal(&condvar_msg);
    pthread_mutex_unlock(&mutex_msg);

    // Apertura cola del cliente
    /*if ((cola_cliente = mq_open(peticion.qclient, O_WRONLY)) == -1){
        perror("[SERVIDOR][ERROR] Cola del cliente no pudo abrirse\n");
        pthread_exit(NULL);
    }*/
    
    //char status[3];

    switch(peticion.op){

        case 'i':   // Operacion init() 

            // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
            pthread_mutex_lock(&tuples_mutex);
            respuesta.status = init_serv();
            pthread_mutex_unlock(&tuples_mutex);
            respuesta.status = ntohl(respuesta.status);
            // Se envia la respuesta al cliente
            /*if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");*/
            //sprintf(status, "%d", respuesta.status);
            if (sendMessage(peticion.sock_client, (char*) &respuesta.status, sizeof(int32_t)) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");
            // Imprime el valor a enviar
            //printf("[SERVIDOR]Valor a enviar: %s", status);
            //if (sendMessage(peticion.sock_client, status, strlen(status)+1) == -1)
                //perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");

            break;

        case 's':  // Operacion set_value()

            // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
            pthread_mutex_lock(&tuples_mutex);
            respuesta.status = set_value_serv(peticion.content.key, peticion.content.value1, 
                                                peticion.content.value2, peticion.content.value3);
            pthread_mutex_unlock(&tuples_mutex);
            //sprintf(status, "%d", respuesta.status);
            // Se envia la respuesta al cliente
            /*if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");*/
            respuesta.status = ntohl(respuesta.status);
            if (sendMessage(peticion.sock_client, (char*) &respuesta.status, sizeof(int32_t)) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");
            // Imprime el valor a enviar
            //printf("[SERVIDOR] Valor a enviar: %s", status);
            //if (sendMessage(peticion.sock_client, status, strlen(status)+1) == -1)
                //perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");

            break;

        case 'g': // Operacion get_value()

                // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
                pthread_mutex_lock(&tuples_mutex);
                respuesta.status = get_value_serv(peticion.content.key, respuesta.content.value1, 
                                                    &respuesta.content.value2, &respuesta.content.value3);
                pthread_mutex_unlock(&tuples_mutex);
                //sprintf(status, "%d", respuesta.status);
                // Se envia la respuesta al cliente
                /*if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                    perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");*/
                int status = ntohl(respuesta.status);
                if (sendMessage(peticion.sock_client, (char*) &status, sizeof(int32_t)) == -1)
                    perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");

                // Imprime el valor a enviar
               //printf("[SERVIDOR] Valor a enviar: %s", status);
                //if (sendMessage(peticion.sock_client, status, strlen(status)+1) == -1)
                //    perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");
                if (respuesta.status == 0){

                    char value1[256];
                    strcpy(value1, respuesta.content.value1);
                    respuesta.content.value2 = htonl(respuesta.content.value2);
                    uint64_t value3_uint = *(uint64_t*) &respuesta.content.value3;
                    respuesta.content.value3 = (double) ((uint64_t) htonl(value3_uint >> 32) << 32 | htonl(value3_uint & 0xFFFFFFFF));


                    if (sendMessage(peticion.sock_client, value1, sizeof(value1)) == -1)
                        perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente (value 1)\n");
                    if (sendMessage(peticion.sock_client, (char*) &respuesta.content.value2, sizeof(int32_t)) == -1)
                        perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente (value 2)\n");
                    if (sendMessage(peticion.sock_client, (char*) &respuesta.content.value3, sizeof(uint64_t)) == -1)
                        perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente (value 3)\n");
                    /*
                    char* value2; char* value3;

                    char err[3] = "0";

                    if ((value2 = malloc(sizeof(char) * 256)) == NULL){
                        perror("[SERVIDOR][ERROR] No se pudo reservar memoria para el valor 2\n");
                        strcpy(err, "-1");
                    }

                    if ((value3 = malloc(sizeof(char) * 256)) == NULL) {
                        perror("[SERVIDOR][ERROR] No se pudo reservar memoria para el valor 3\n");
                        strcpy(err, "-1");
                    }

                    int size_value2 = snprintf(value2, 256, "%d", respuesta.content.value2);
                    int size_value3 = snprintf(value3, 256, "%lf", respuesta.content.value3);

                    if (size_value2 < 0 || size_value2 >= 256)
                        if ((value2 = realloc(value2, (size_value2 + 1) * sizeof(char))) == NULL) {
                            perror("[SERVIDOR][ERROR] No se pudo reservar memoria para el valor 2\n");
                            strcpy(err, "-1");
                        }

                    if (size_value3 < 0 || size_value3 >= 256)
                        if ((value3 = realloc(value3, (size_value3 + 1) * sizeof(char))) == NULL) {
                            perror("[SERVIDOR][ERROR] No se pudo reservar memoria para el valor 3\n");
                            strcpy(err, "-1");
                        }
                        

                    if (sendMessage(peticion.sock_client, err, sizeof(char)) == -1)
                        perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente (error)\n");
                    
                    if (strcmp(err, "-1") == 0){
                        
                        if (value2 != NULL)
                            free(value2);
                        if (value3 != NULL)
                            free(value3);

                        break;
                    } 

                    if (sendMessage(peticion.sock_client, respuesta.content.value1, strlen(respuesta.content.value1)+1) == -1){
                        perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente (value 1)\n");
                        //strcpy(err, "-1");
                    }
                    if (sendMessage(peticion.sock_client, value2, strlen(value2)+1) == -1){
                        perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente (value 2)\n");
                        //strcpy(err, "-1");
                    }
                    if (sendMessage(peticion.sock_client, value3, strlen(value3)+1) == -1){
                        perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente (value 3)\n");
                        //strcpy(err, "-1");
                    }

                    //if (sendMessage(peticion.sock_client, err, sizeof(char)) == -1)
                      //  perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente (error)\n");
            
                    if (value2 != NULL)
                        free(value2);
                    if (value3 != NULL)
                        free(value3);*/
                }

                break;

        case 'm':  // Operacion modify_value()

            // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
            pthread_mutex_lock(&tuples_mutex);
            respuesta.status = modify_value_serv(peticion.content.key, peticion.content.value1,
                                                peticion.content.value2, peticion.content.value3);
            pthread_mutex_unlock(&tuples_mutex);
            respuesta.status = ntohl(respuesta.status);
            if (sendMessage(peticion.sock_client, (char*) &respuesta.status, sizeof(int32_t)) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");
            //sprintf(status, "%d", respuesta.status);
            // Se envia la respuesta al cliente
            /*if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");*/
            /*if (sendMessage(peticion.sock_client, status, strlen(status)+1) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");*/

            break;

        case 'd':  //Operacion delete_key()

            // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
            pthread_mutex_lock(&tuples_mutex);
            respuesta.status = delete_key_serv(peticion.content.key);
            pthread_mutex_unlock(&tuples_mutex);
            //sprintf(status, "%d", respuesta.status);
            // Se envia la respuesta al cliente
            /*if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");*/
            //if (sendMessage(peticion.sock_client, status, strlen(status)+1) == -1)
            //    perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");
            respuesta.status = ntohl(respuesta.status);
            if (sendMessage(peticion.sock_client, (char*) &respuesta.status, sizeof(int32_t)) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");
            break;

        case 'e':   // Operacion exist()

            // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
            pthread_mutex_lock(&tuples_mutex);
            respuesta.status = exist_serv(peticion.content.key);
            pthread_mutex_unlock(&tuples_mutex);
            //sprintf(status, "%d", respuesta.status);
            // Se envia la respuesta al cliente
            /*if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");*/
            //if (sendMessage(peticion.sock_client, status, strlen(status)+1) == -1)
            //    perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");
            respuesta.status = ntohl(respuesta.status);
            if (sendMessage(peticion.sock_client, (char*) &respuesta.status, sizeof(int32_t)) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");

            break;

        case 'c':    // Operacion copy_key()
            // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
            pthread_mutex_lock(&tuples_mutex);
            respuesta.status = copy_key_serv(peticion.content.key, peticion.second_key);
            pthread_mutex_unlock(&tuples_mutex);
            respuesta.status = ntohl(respuesta.status);

            if (sendMessage(peticion.sock_client, (char*) &respuesta.status, sizeof(int32_t)) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");
            
            //sprintf(status, "%d", respuesta.status);
            // Se envia la respuesta al cliente
            /*if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");*/

            /*if (sendMessage(peticion.sock_client, status, strlen(status)+1) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");*/
                
            break;

        default:
            perror("[SERVIDOR][ERROR] Operacion solicitada no valida\n");
    }

    // Cerrar cola del cliente
    /*if (mq_close(cola_cliente) == -1){
        perror("[SERVIDOR][ERROR] Cola del cliente no pudo cerrarse\n");
    }*/

    if (close(peticion.sock_client) == -1){
        perror("[SERVIDOR][ERROR] Socket del cliente no pudo cerrarse\n");
    }
    pthread_exit(NULL);
}



int main(){


    /*mqd_t cola_servidor;
    struct mq_attr qattr_servidor;
    qattr_servidor.mq_maxmsg = 10;
    qattr_servidor.mq_msgsize = sizeof(Request);*/


    // Mensaje recibido del cliente
    Request peticion;

    pthread_t thid;
    pthread_attr_t th_attr; 

    /*
    // Crear cola del servidor
    if((cola_servidor = mq_open("/COLA_SERVIDOR", O_CREAT|O_RDONLY, 0700, &qattr_servidor)) == -1){
        perror("[SERVIDOR] No se puede crear la cola de servidor\n");
        return -1; 
    }*/
    int sock_serv_fd, sock_client_fd;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // Se establece la IP del servidor a localhost
    serv_addr.sin_addr.s_addr = inet_addr("localhost");
    serv_addr.sin_port = htons(8080);

    char op;
    char value1[256];
    int32_t key, value2, second_key;
    uint64_t value3;



    if ((sock_serv_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[SERVIDOR][ERROR] No se pudo crear socket de recepción de peticiones\n");
        return -1;
    }

    if (bind(sock_serv_fd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        perror("[SERVIDOR][ERROR] No se pudo enlazar el socket de recepción de peticiones\n");
        return -1;
    }

    if (listen(sock_serv_fd, SOMAXCONN) == -1){
        perror("[SERVIDOR][ERROR] No se pudo poner el socket en modo escucha\n");
        return -1;
    }
    
    pthread_attr_init(&th_attr);
    pthread_attr_setdetachstate(&th_attr,PTHREAD_CREATE_DETACHED);
    
    // Recepcion y atencion continua de peticiones
    while(1){

        // Recepcion de peticion
        /*if(mq_receive(cola_servidor, (char *) &peticion, sizeof(Request), 0) == -1)
            break;*/
        if ((sock_client_fd = accept(sock_serv_fd, (struct sockaddr*) &client_addr, &client_addr_len)) == -1){
            perror("[SERVIDOR][ERROR] No se pudo aceptar la conexión del cliente\n");
            break;
        }

        //if (recvMessage(sock_client_fd, (char*) &op, sizeof(char)) == -1){
        if (recvMessage(sock_client_fd, (char*) &op, sizeof(char)) == -1){
            perror("[SERVIDOR][ERROR] No se pudo recibir la petición del cliente (operacion)\n");
            break;
        }

        //if (recvMessage(sock_client_fd, (char*) &key, sizeof(key)) == -1){
        if (recvMessage(sock_client_fd, (char*) &key, sizeof(int32_t)) == -1){
            perror("[SERVIDOR][ERROR] No se pudo recibir la petición del cliente (clave)\n");
            break;
        }

        if (op == 's' || op == 'm'){
            //if (recvMessage(sock_client_fd, &value1, sizeof(value1)) == -1){
            if (readLine(sock_client_fd, value1, sizeof(value1)) == -1){
                perror("[SERVIDOR][ERROR] No se pudo recibir la petición del cliente (valor 1)\n");
                break;
            }

            //if (recvMessage(sock_client_fd, (char*) &value2, sizeof(value2)) == -1){
            if (recvMessage(sock_client_fd, (char*) &value2, sizeof(int32_t)) == -1){
                perror("[SERVIDOR][ERROR] No se pudo recibir la petición del cliente (valor 2)\n");
                break;
            }

            //if (recvMessage(sock_client_fd, (char*) &value3, sizeof(value3)) == -1){
            if (recvMessage(sock_client_fd, (char*) &value3, sizeof(uint64_t)) == -1){
                perror("[SERVIDOR][ERROR] No se pudo recibir la petición del cliente (valor 3)\n");
                break;
            }
        }
        if (op == 'c'){
            //if (recvMessage(sock_client_fd, (char*) &second_key, sizeof(second_key)) == -1){
            if (recvMessage(sock_client_fd, (char*) &second_key, sizeof(int32_t)) == -1){
                perror("[SERVIDOR][ERROR] No se pudo recibir la petición del cliente (second_key)\n");
                break;
            }
        }

        peticion.op = op;

        // Almacena la direccion de socket del cliente en la peticion para volver a abrir el socket del cliente desde el hilo
        //peticion.sock_client = client_addr;
        peticion.sock_client = sock_client_fd;

        //peticion.content.key = ntohl(key);
        peticion.content.key = ntohl(key);
        if (op == 's' || op == 'm'){
            strcpy(peticion.content.value1, value1);
            //memcpy(peticion.content.value1, value1, sizeof(value1));
            peticion.content.value2 = ntohl(value2);
            //peticion.content.value2 = value2;
            // Se convierte el valor de 64 bits a su representacion en big endian usando operadores bitwise
            peticion.content.value3 = (double)((uint64_t) ntohl(value3 >> 32) << 32 | ntohl(value3 & 0xFFFFFFFF));
            //peticion.content.value3 = value3;
        }
        if (op == 'c'){
            peticion.second_key = ntohl(second_key);
            //peticion.second_key = second_key;
        }

        printf("[SERVIDOR] Operacion recibida: %c\n", peticion.op);
        printf("[SERVIDOR] Clave recibida: %d\n", peticion.content.key);
        if (op == 's' || op == 'm'){
            printf("[SERVIDOR] Valor 1 recibido: %s\n", peticion.content.value1);
            printf("[SERVIDOR] Valor 2 recibido: %d\n", peticion.content.value2);
            printf("[SERVIDOR] Valor 3 recibido: %lf\n", peticion.content.value3);
        }
        if (op == 'c'){
            printf("[SERVIDOR] Segunda clave recibida: %d\n", peticion.second_key);
        }

        // Crea un hilo por peticion
        if(pthread_create(&thid, &th_attr, (void*) &cumplir_pet, (void *) &peticion) == -1){
            perror("[SERVIDOR][ERROR] Hilo no pudo ser creado\n");
            break;
        }

        // Asegura que la peticion se copia correctamente en el hilo que atiende al cliente
        pthread_mutex_lock(&mutex_msg);
        while (mensaje_no_copiado) pthread_cond_wait(&condvar_msg, &mutex_msg);
            mensaje_no_copiado = 1;
            pthread_mutex_unlock(&mutex_msg);
        }

    // Cerrar cola del servidor
    /*if (mq_close(cola_servidor) == -1){
        perror("[SERVIDOR] Cola del servidor no pudo cerrarse\n");
        return -1;
    }


    // Desvincular cola del servidor
    if (mq_unlink("/COLA_SERVIDOR") == -1){
        perror("[SERVIDOR] Cola cliente no pudo desvincularse\n");
        return -1;
    }*/

    if (close(sock_serv_fd) == -1){
        perror("[SERVIDOR][ERROR] No se pudo cerrar el socket de recepción de peticiones\n");
        return -1;
    }

    return 0;
}