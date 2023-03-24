#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
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
    mqd_t cola_cliente;

    // Hilo obtiene la peticion del cliente
    pthread_mutex_lock(&mutex_msg);
    peticion = (*(Request *) pet);
    mensaje_no_copiado = 0;
    pthread_cond_signal(&condvar_msg);
    pthread_mutex_unlock(&mutex_msg);

    // Apertura cola del cliente
    if ((cola_cliente = mq_open(peticion.qclient, O_WRONLY)) == -1){
        perror("[SERVIDOR][ERROR] Cola del cliente no pudo abrirse\n");
        pthread_exit(NULL);
    }

    switch(peticion.op){

        case 'i':   // Operacion init() 

            // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
            pthread_mutex_lock(&tuples_mutex);
            respuesta.status = init_serv();
            pthread_mutex_unlock(&tuples_mutex);

            // Se envia la respuesta al cliente
            if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");

            break;

        case 's':  // Operacion set_value()

            // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
            pthread_mutex_lock(&tuples_mutex);
            respuesta.status = set_value_serv(peticion.content.key, peticion.content.value1, 
                                                peticion.content.value2, peticion.content.value3);
            pthread_mutex_unlock(&tuples_mutex);

            // Se envia la respuesta al cliente
            if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");

            break;

        case 'g': // Operacion get_value()

                // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
                pthread_mutex_lock(&tuples_mutex);
                respuesta.status = get_value_serv(peticion.content.key, respuesta.content.value1, 
                                                    &respuesta.content.value2, &respuesta.content.value3);
                pthread_mutex_unlock(&tuples_mutex);

                // Se envia la respuesta al cliente
                if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                    perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");

                break;

        case 'm':  // Operacion modify_value()

            // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
            pthread_mutex_lock(&tuples_mutex);
            respuesta.status = modify_value_serv(peticion.content.key, peticion.content.value1,
                                                peticion.content.value2, peticion.content.value3);
            pthread_mutex_unlock(&tuples_mutex);

            // Se envia la respuesta al cliente
            if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");

            break;

        case 'd':  //Operacion delete_key()

            // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
            pthread_mutex_lock(&tuples_mutex);
            respuesta.status = delete_key_serv(peticion.content.key);
            pthread_mutex_unlock(&tuples_mutex);

            // Se envia la respuesta al cliente
            if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");

            break;

        case 'e':   // Operacion exist()

            // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
            pthread_mutex_lock(&tuples_mutex);
            respuesta.status = exist_serv(peticion.content.key);
            pthread_mutex_unlock(&tuples_mutex);

            // Se envia la respuesta al cliente
            if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");

            break;

        case 'c':    // Operacion copy_key()

            // Obtiene acceso exclusivo al directorio "tuples" y sus ficheros
            pthread_mutex_lock(&tuples_mutex);
            respuesta.status = copy_key_serv(peticion.content.key, peticion.second_key);
            pthread_mutex_unlock(&tuples_mutex);

            // Se envia la respuesta al cliente
            if (mq_send(cola_cliente, (const char*) &respuesta, sizeof(Service), 0) == -1)
                perror("[SERVIDOR][ERROR] No se pudo enviar la respuesta al cliente\n");

            break;

        default:
            perror("[SERVIDOR][ERROR] Operacion solicitada no valida\n");
    }

    // Cerrar cola del cliente
    if (mq_close(cola_cliente) == -1){
        perror("[SERVIDOR][ERROR] Cola del cliente no pudo cerrarse\n");
    }

    pthread_exit(NULL);
}



int main(){


    /*mqd_t cola_servidor;
    struct mq_attr qattr_servidor;
    qattr_servidor.mq_maxmsg = 10;
    qattr_servidor.mq_msgsize = sizeof(Request);*/


    // Mensaje recibido del cliente
    //Request peticion;

    pthread_t thid;
    pthread_attr_t th_attr; 

    /*
    // Crear cola del servidor
    if((cola_servidor = mq_open("/COLA_SERVIDOR", O_CREAT|O_RDONLY, 0700, &qattr_servidor)) == -1){
        perror("[SERVIDOR] No se puede crear la cola de servidor\n");
        return -1; 
    }*/
    int sockfd;
    struct sockaddr_in serv_addr, client_addr;
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // Se establece la IP del servidor a localhost
    serv_addr.sin_addr.s_addr = inet_addr("localhost");
    serv_addr.sin_port = htons(8080);



    if (sockfd = socket(AF_INET, SOCK_STREAM, 0)==-1) {
        perror("[SERVIDOR][ERROR] No se pudo crear socket de recepción de peticiones\n");
        return -1;
    }

    

    if (bind(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        perror("[SERVIDOR][ERROR] No se pudo enlazar el socket de recepción de peticiones\n");
        return -1;
    }

    if (listen(sockfd, SOMAXCONN) == -1){
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
        accept(sockfd)

        // Crea un hilo por peticion
        if(pthread_create(&thid, &th_attr, (void*) &cumplir_pet, (void *) &peticion) == -1){
            perror("[SERVIDOR] Hilo no pudo ser creado\n");
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

    if (close(sockfd) == -1){
        perror("[SERVIDOR][ERROR] No se pudo cerrar el socket de recepción de peticiones\n");
        return -1;
    }

    return 0;
}