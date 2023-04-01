#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "claves.h"

// El cliente inicializa el directorio de tuplas
int Test1_C(){

    int resultado = init();

    if (resultado == 0) {
        printf("\n[ TEST ] Test1_C ejecutado correctamente\n");
        return 1;
    } else {
        printf("[ TEST ERROR ] Error en test 1_C (cliente inicializa directorio de tuplas). Valor retornado %i, se esperaba 0\n", resultado);
        return 0;
    }
}

// El cliente crea una tupla
int Test2_C(){

    init();

    int key = 4; char *value1 = "Prueba1"; int value2 = 2; double value3 = 8.3;

    int resultado = set_value(key, value1, value2, value3);

    if (resultado == 0) {
        printf("[ TEST ] Test2_C ejecutado correctamente\n");
        return 1;
    } else {
        printf("[ TEST ERROR ] Error en test 2_C (cliente crea una tupla). Valor retornado %i, se esperaba 0\n", resultado);
        return 0;
    }
}

// El cliente lee una tupla
/*int Test3_C(){

    init();
    
    // Se crea tupla primero
    int key = 4; char *value1 = "Prueba1"; int value2 = 2; double value3 = 8.3;
    
    set_value(key, value1, value2, value3);
    
    // Se lee tupla
    char value1_recv[256] = ""; int value2_recv; double value3_recv;

    int resultado = get_value(key, value1_recv, &value2_recv, &value3_recv);

    if ((strcmp(value1_recv, value1) == 0) && value2_recv == value2 && value3_recv == value3) {
        if (resultado == 0) {
            printf("[ TEST ] Test3_C ejecutado correctamente\n");
            return 1;
        } else {
            printf("[ TEST ERROR ] Error en test 3_C (cliente lee una tupla). Valores recuperados pero valor retornado %i, se esperaba 0\n", 
                    resultado);
            return 0;
        }
    } else {
        printf("[ TEST ERROR ] Error en test 3_C (cliente lee una tupla). Valor retornado %i, se esperaba 0\n",resultado);
        return 0;
    }
}

// El cliente modifica una tupla
int Test4_C(){

    init();

    // Se crea tupla primero
    int key = 4; char *value1_1 = "Prueba1"; int value2_1 = 2; double value3_1 = 8.3;

    set_value(key, value1_1, value2_1, value3_1);

    // Se modifica tupla
    char *value1_2 = "Prueba2"; int value2_2 = 3; double value3_2 = 10.5;

    int resultado1 = modify_value(key, value1_2, value2_2, value3_2);
    if (resultado1 == -1) {
        printf("[ TEST ERROR ] Error en test 4_C (cliente modifica una tupla). Valor retornado %i, se esperaba 0\n", resultado1);
        return 0;
    }

    // Se recupera la tupla para comprobar que se modifico correctamente
    char value1_recv[256] = ""; int value2_recv; double value3_recv;
    
    int resultado2 = get_value(key, value1_recv, &value2_recv, &value3_recv);
    if ((strcmp(value1_recv, value1_2) == 0) && value2_recv == value2_2 && value3_recv == value3_2) {
        if (resultado2 == 0) {
            printf("[ TEST ] Test4_C ejecutado correctamente\n");
            return 1;
        } else {
            printf("[ TEST ERROR ] Error en test 4_C (cliente modifica una tupla). Valores recuperados pero valor retornado %i, se esperaba 0\n",
                    resultado2);
            return 0;
        }
    } else {
        printf("[ TEST ERROR ] Error en test 4_C (cliente modifica una tupla). Los valores modificados no coinciden con lo esperado\n");
        return 0;
    }
}

// El cliente elimina una tupla
int Test5_C(){

    init();

    // Se crean 2 tuplas primero
    int key_1 = 4; char *value1_1 = "Prueba1"; int value2_1 = 2; double value3_1 = 8.3;

    int key_2 = 21; char *value1_2 = "Prueba2"; int value2_2 = 8; double value3_2 = 14.7;
    
    set_value(key_1, value1_1, value2_1, value3_1);
    set_value(key_2, value1_2, value2_2, value3_2);

    // Se elimina 1 tupla
    int resultado = delete_key(key_1);

    // Se revisa si todavia la segunda tupla permanece en el directorio
    DIR* dir = opendir("./tuples");
    struct dirent* entry;
    int count = 0;
    
    while ((entry = readdir(dir)) != NULL) {
        // Se omiten las entradas "." y ".."
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            count++;
        }
    }

    closedir(dir);

    // Si directorio se vacio
    if (count == 0){
        printf("[ TEST ERROR ] Error en test 5_C (cliente elimina una tupla cuando hay más de una). El directorio se vació por completo\n");
        return 0;
    }

    if (resultado == 0) {
        printf("[ TEST ] Test5_C ejecutado correctamente\n");
        return 1;
    } else {
        printf("[ TEST ERROR ] Error en test 5_C (cliente elimina una tupla cuando hay más de una). Valor retornado %i, se esperaba 0\n",
                resultado);
        return 0;
    }
}

// El cliente comprueba si existe una tupla existente
int Test6_C(){

    init();

    // Se crea tupla primero
    int key = 4; char *value1 = "Prueba1"; int value2 = 2; double value3 = 8;

    set_value(key, value1, value2, value3);

    // Se comprueba si existe
    int resultado = exist(key);
    if (resultado == 1) {
        printf("[ TEST ] Test6_C ejecutado correctamente\n");
        return 1;
    } else {
        printf("[ TEST ERROR ] Error en test 6_C (cliente comprueba que existe una tupla existente). Valor retornado %i, se esperaba 1\n", 
                resultado);
        return 0;
    }
}

// El cliente comprueba si existe una tupla no existente
int Test7_C(){

    init();

    // Se comprueba si existe
    int key = 4;
    int resultado = exist(key);

    if (resultado == 0) {
        printf("[ TEST ] Test7_C ejecutado correctamente\n");
        return 1;
    } else {
        printf("[ TEST ERROR ] Error en test 7_C (cliente comprueba que existe una tupla no existente). Valor retornado %i, se esperaba 0\n",
                resultado);
        return 0;
    }
}

// El cliente copia tupla a key nueva
int Test8_C(){

    init();

    // Se crea tupla primero
    int key1 = 4; char *value1 = "Prueba1"; int value2 = 2; double value3 = 8;

    set_value(key1, value1, value2, value3);

    // Se copia a key2
    int key2 = 2;

    int resultado1 = copy_key(key1,key2);
    if (resultado1 == -1) {
        printf("[ TEST ERROR ] Error en test 8_C (cliente copia una tupla en nueva key). Valor retornado %i, se esperaba 0\n", resultado1);
        return 0;
    }

    // Se recupera la tupla (usando key2) para comprobar que se copio correctamente
    char value1_recv[256] = ""; int value2_recv; double value3_recv;

    int resultado2 = get_value(key2, value1_recv, &value2_recv, &value3_recv);

    if (resultado2 == 0) {
        if ((strcmp(value1_recv, value1) == 0) && value2_recv == value2 && value3_recv == value3) {
            printf("[ TEST ] Test8_C ejecutado correctamente\n");
            return 1;
        } else {
            printf("[ TEST ERROR ] Error en test 8_C (cliente copia una tupla en nueva key). Los valores copiados no coinciden con lo esperado\n");
            return 0;
        }
    } else {
        printf("[ TEST ERROR ] Error en test 8_C (cliente copia una tupla en nueva key). Valor retornado %i, se esperaba 0 en get\n", resultado2);
        return 0;
    }
}

// El cliente copia tupla a key existente
int Test9_C(){

    init();

    // Se crea tupla primero
    int key_1 = 4; char *value1_1 = "Prueba1"; int value2_1 = 2; double value3_1 = 8;

    set_value(key_1, value1_1, value2_1, value3_1);

    // Se crea segunda tupla
    int key_2 = 2; char *value1_2 = "Prueba2"; int value2_2 = 4; double value3_2 = 10;

    set_value(key_2, value1_2, value2_2, value3_2);

    // Se copia a key2
    int resultado1 = copy_key(key_1,key_2);

    if (resultado1 == -1) {
        printf("[ TEST ERROR ] Error en test 9_C (cliente copia una tupla en key ya en uso). Valor retornado %i, se esperaba 0\n", resultado1);
        return 0;
    }

    // Se recupera la tupla (usando key2) para comprobar que se copio correctamente
    char value1_recv[256] = ""; int value2_recv; double value3_recv;

    int resultado2 = get_value(key_2, value1_recv, &value2_recv, &value3_recv);
    if (resultado2 == 0) {
        if ((strcmp(value1_recv, value1_1) == 0) && value2_recv == value2_1 && value3_recv == value3_1) {
            printf("[ TEST ] Test9_C ejecutado correctamente\n");
            return 1;
        } else {
            printf("[ TEST ERROR ] Error en test 9_C (cliente copia una tupla en key ya en uso). Los valores copiados no coinciden con lo esperado\n");
            return 0;
        }
    } else {
        printf("[ TEST ERROR ] Error en test 9_C (cliente copia una tupla en key ya en uso). Valor retornado %i, se esperaba 0 en get\n", resultado2);
        return 0;
    }
}

// El cliente crea una tupla con valor1 de longitud 255 caracteres + '\0'
int Test10_C(){

    init();

    // Se crea la tupla
    int key = 4;
    char *value1 = "Prueba111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\0";
    int value2 = 2;
    double value3 = 8.3;

    int resultado = set_value(key, value1, value2, value3);

    if (resultado == 0) {
        printf("[ TEST ] Test10_C ejecutado correctamente\n");
        return 1;
    } else {
        printf("[ TEST ERROR ] Error en test 10_C (cliente crea una tupla de valor1 255 y \'\\0\'). Valor retornado %i, se esperaba 0\n",
                resultado);
        return 0;
    }
}

// El cliente crea una tupla (ya existe una con la misma key)
int Test1_NC(){

    init();

    // Se crea tupla primero
    int key = 4; char *value1_1 = "Prueba1"; int value2_1 = 2; double value3_1 = 8;

    set_value(key, value1_1, value2_1, value3_1);

    // Se crea segunda tupla
    char *value1_2 = "Prueba2"; int value2_2 = 4; double value3_2 = 10;

    int resultado1 = set_value(key, value1_2, value2_2, value3_2);

    if (resultado1 == 0) {
        printf("[ TEST ERROR ] Error en test 1_NC (cliente crea una tupla con una key que ya existe). Valor retornado %i, se esperaba -1\n",
                resultado1);
        return 0;
    }

    // Se recupera la tupla para comprobar que no se sobrescribe
    char value1_recv[256] = ""; int value2_recv; double value3_recv;

    int resultado2 = get_value(key, value1_recv, &value2_recv, &value3_recv);
    
    if (resultado2 == 0) {
        if ((strcmp(value1_recv, value1_1) == 0) && value2_recv == value2_1 && value3_recv == value3_1) {
            printf("[ TEST ] Test1_NC ejecutado correctamente\n");
            return 1;
        } else {
            printf("[ TEST ERROR ] Error en test 1_NC (cliente crea una tupla con una key que ya existe). Los valores de la tupla han cambiado\n");
            return 0;
        }
    } else {
        printf("[ TEST ERROR ] Error en test 1_NC (cliente crea una tupla con una key que ya existe). Valor retornado %i, se esperaba -1\n",
                resultado2);
        return 0;
    }
}

// El cliente lee una tupla que no existe
int Test2_NC(){

    init();

    // Se lee tupla
    int key = 4; char* value1 = ""; int value2; double value3;
    
    int resultado = get_value(key, value1, &value2, &value3);
    if (resultado == -1) {
        printf("[ TEST ] Test2_NC ejecutado correctamente\n");
        return 1;
    } else {
        printf("[ TEST ERROR ] Error en test 2_NC (cliente lee una tupla que no existe). Valor retornado %i, se esperaba -1\n", resultado);
        return 0;
    }
}

// El cliente modifica una tupla que no existe
int Test3_NC(){

    init();

    // Se modifica tupla
    int key = 4; char *value1 = "Prueba2"; int value2 = 3; double value3 = 10;

    int resultado = modify_value(key, value1, value2, value3);

    if (resultado == -1) {
        printf("[ TEST ] Test3_NC ejecutado correctamente\n");
        return 1;
    } else {
        printf("[ TEST ERROR ] Error en test 3_NC (cliente modifica una tupla  que no existe). Valor retornado %i, se esperaba -1\n", resultado);
        return 0;
    }
}

// El cliente elimina una tupla que no existe
int Test4_NC(){

    init();

    // Se elimina tupla
    int key = 4;
    int resultado = delete_key(key);

    if (resultado == -1) {
        printf("[ TEST ] Test4_NC ejecutado correctamente\n");
        return 1;
    } else {
        printf("[ TEST ERROR ] Error en test 4_NC (cliente elimina una tupla que no existe). Valor retornado %i, se esperaba -1\n",resultado);
        return 0;
    }
}

// El cliente copia una tupla que no existe
int Test5_NC(){

    init();

    // Se copia tupla
    int key1 = 4; int key2 = 2;

    int resultado = copy_key(key1,key2);
    if (resultado == -1) {
        printf("[ TEST ] Test5_NC ejecutado correctamente\n");
        return 1;
    } else {
        printf("[ TEST ERROR ] Error en test 5_NC (cliente copia una tupla que no existe). Valor retornado %i, se esperaba -1\n", resultado);
        return 0;
    }
}

// El cliente crea una tupla con un valor1 demasiado grande
int Test6_NC(){
    init();

    int key = 4;
    char *value1 = "Prueba11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111\0";
    int value2 = 2;
    double value3 = 8.3;

    int resultado = set_value(key, value1, value2, value3);
    if (resultado == -1) {
        printf("[ TEST ] Test6_NC ejecutado correctamente\n");
        return 1;
    } else {
        printf("[ TEST ERROR ] Error en test 6_NC (cliente crea una tupla con valor1 muy grande). Valor retornado %i, se esperaba -1\n", resultado);
        return 0;
    }
}

// El cliente crea una tupla con un valor1 de 256 caracteres que no termina en '\0'
int Test7_NC(){
    init();

    int key = 4;
    char *value1 = "Prueba1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
    int value2 = 2;
    double value3 = 8.3;

    int resultado = set_value(key, value1, value2, value3);
    if (resultado == -1) {
        printf("[ TEST ] Test7_NC ejecutado correctamente\n");
        return 1;
    } else {
        printf("[ TEST ERROR ] Error en test 7_NC (cliente crea una tupla con valor1 256 caracteres que no termina en \'\\0\')). Valor retornado %i, se esperaba -1\n", 
                resultado);
        return 0;
    }
}*/


int main(){

    int total_correctos = 0;

    printf("---------- EJECUTANDO TESTS ----------\n");
    total_correctos += Test1_C();
    total_correctos += Test2_C();
    /*total_correctos += Test3_C();
    total_correctos += Test4_C();
    total_correctos += Test5_C();
    total_correctos += Test6_C();
    total_correctos += Test7_C();
    total_correctos += Test8_C();
    total_correctos += Test9_C();
    total_correctos += Test10_C();
    total_correctos += Test1_NC();
    total_correctos += Test2_NC();
    total_correctos += Test3_NC();
    total_correctos += Test4_NC();
    total_correctos += Test5_NC();
    total_correctos += Test6_NC();
    total_correctos += Test7_NC();*/
    printf("\n---------- TESTS FINALIZADOS ----------\n");
    if (total_correctos == 17) {
        printf("[ OK ] Todos los tests funcionaron correctamente (%i correctos, %i fallidos)\n",total_correctos, (17-total_correctos));
    } else {
        printf("[ FAILURE ] Los tests se pasaron con errores (%i correctos, %i fallidos)\n",total_correctos, (17-total_correctos));
    }
    return 0;
}