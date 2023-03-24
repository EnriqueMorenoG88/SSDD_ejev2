#ifndef COMUNICACION_H
#define COMUNICACION_H

#define MAX_MSG 10      // Numero maximo de mensajes en cola

// Estructura para tuplas
typedef struct tuple{
    int key;
    char value1[256];
    int value2;
    double value3;
} Tuple;

// Estructura para peticiones de cliente
typedef struct request{
    char op;
    char qclient[1024];
    Tuple content;
    int second_key;
} Request;

// Estructura para respuesta del servidor a cliente
typedef struct service{
    int status;
    Tuple content;
} Service;

#endif