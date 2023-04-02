#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>


const char* filename = "./tuples/tupla_";     // Ruta relativa del fichero de tupla (sin num. clave)
const char* ext = ".dat";                     // Extension del fichero de tupla
const char* dirname =  "tuples";              // Nombre del directorio    

// Estructura para tuplas
typedef struct tuple{
    int key;
    char value1[256];  
    int value2;      
    double value3;
} Tuple;

// Inicializa el directorio de tuplas
int init_serv(){

    DIR* tpl_dir;

    struct dirent *tpl_entry;
    char entry_path[512];

    if ((tpl_dir = opendir(dirname)) == NULL){
        perror("[SERVIDOR][ERROR] El directorio no pudo ser abierto\n");
        return -1;
    }

    // Elimina los ficheros con extesion .dat
    while ((tpl_entry = readdir(tpl_dir)) != NULL) {
            if (strstr(tpl_entry->d_name, ".dat") != NULL){
                snprintf(entry_path, 512, "%s/%s", dirname, tpl_entry->d_name);
                remove(entry_path);
            }
    }

    closedir(tpl_dir);

    if (rmdir(dirname) == -1) {
        perror("[SERVIDOR][ERROR] El fichero no pudo ser eliminado\n");
        return -1;
    }

    if (mkdir(dirname, 0755) == -1) {
        perror("[SERVIDOR][ERROR] El directorio no pudo ser creado\n");
        return -1;
    }

    return 0;
}


// Insercion de la tupla
int set_value_serv(int key, char *value1, int value2, double value3){

    // Obtiene nombre completo del fichero
    char tuple_file [1024];
    sprintf(tuple_file, "%s%d%s", filename, key, ext);

    if (!access(tuple_file, F_OK)){
        perror("[SERVIDOR][ERROR] Fichero ya existente\n");
        return -1;
    }

    FILE* tpl_fp;

    if ((tpl_fp = fopen(tuple_file, "w")) == NULL){
        perror("[SERVIDOR][ERROR] El fichero para la tupla no pudo ser creado\n");
        return -1;
    }

    Tuple tupla;
    tupla.key = key;
    strcpy(tupla.value1, value1);
    tupla.value2 = value2;
    tupla.value3 = value3; 

    if (fwrite(&tupla, sizeof(Tuple), 1, tpl_fp) == 0){
        perror("[SERVIDOR][ERROR] No se pudo escribir en el fichero\n");
        fclose(tpl_fp);
        return -1;
    }

    fclose(tpl_fp);
    return 0;
}


// Obtención de los valores asociados a la clave proporcionada
int get_value_serv(int key, char* value1, int* value2, double* value3){

    // Obtiene nombre completo del fichero
    char tuple_file [1024];
    sprintf(tuple_file, "%s%d%s", filename, key, ext);

    if (access(tuple_file, F_OK)){
        perror("[SERVIDOR][ERROR] Fichero no existe\n");
        return -1;
    }

    FILE* tpl_fp;

    if ((tpl_fp = fopen(tuple_file, "r")) == NULL){
        perror("[SERVIDOR][ERROR] El fichero para la tupla no pudo ser abierto\n");
        return -1;
    }

    Tuple tupla;
    if (fread(&tupla, sizeof(Tuple), 1, tpl_fp) == 0){
        perror("[SERVIDOR][ERROR] Fichero no pudo leerse\n");
        fclose(tpl_fp);
        return -1;
    }

    fclose(tpl_fp);

    strcpy(value1, tupla.value1);
    *value2 = tupla.value2;
    *value3 = tupla.value3;

    return 0;
}


// Modificación de los valores asociados a la clave proporcionada
int modify_value_serv(int key, char* value1, int value2, double value3){

    // Obtiene nombre completo del fichero
    char tuple_file [1024];
    sprintf(tuple_file, "%s%d%s", filename, key, ext);
    

    if (access(tuple_file, F_OK)){
        perror("[SERVIDOR][ERROR] Fichero no existe\n");
        return -1;
    }

    if (remove(tuple_file) == -1){
        perror("[SERVIDOR][ERROR] El directorio no pudo ser eliminado\n");
        return -1;
    }

    FILE* tpl_fp;

    if ((tpl_fp = fopen(tuple_file, "w")) == NULL){
        perror("[SERVIDOR][ERROR] El fichero para la tupla no pudo ser creado\n");
        return -1;
    }

    Tuple tupla;
    tupla.key = key;
    strcpy(tupla.value1, value1);
    tupla.value2 = value2;
    tupla.value3 = value3;

    if (fwrite(&tupla, sizeof(Tuple), 1, tpl_fp) == 0){
        perror("[SERVIDOR][ERROR] No se pudo escribir en el fichero\n");
        fclose(tpl_fp);
        return -1;
    }
    
    fclose(tpl_fp);

    return 0;
}


// Eliminación de la tupla asociada a la clave proporcionada
int delete_key_serv(int key){

    // Obtiene nombre completo del fichero
    char tuple_file [1024];
    sprintf(tuple_file, "%s%d%s", filename, key, ext);
    

    if (access(tuple_file, F_OK)){
        perror("[SERVIDOR][ERROR] Fichero no existe\n");
        return -1;
    }
    
    if (remove(tuple_file) == -1){
        perror("[SERVIDOR][ERROR] El directorio no pudo ser eliminado\n");
        return -1;
    }

    return 0;
}


// Comprobacion de la existencia de alguna tupla asociado a la clave proporcionada
int exist_serv(int key){

    // Obtiene nombre completo del fichero
    char tuple_file [1024];
    sprintf(tuple_file, "%s%d%s", filename, key, ext);
    
    if (!access(tuple_file, F_OK))
        return 1;

    return 0;
}


// Creacion e insercion de un nuevo elemento con la segunda clave proporcionada, copiando los valores de la primera
int copy_key_serv(int key1, int key2){

    // Obtiene nombre completo del fichero original (key 1)
    char original_tpl_file [1024], copy_tpl_file [1024];
    sprintf(original_tpl_file, "%s%d%s", filename, key1, ext);

    if (access(original_tpl_file, F_OK)){
        perror("[SERVIDOR][ERROR] Fichero no existe\n");
        return -1;
    }

    FILE* original_tpl_fp;
    FILE* copy_tpl_fp;

    Tuple tupla;

    if ((original_tpl_fp = fopen(original_tpl_file, "r")) == NULL){
        perror("[SERVIDOR][ERROR] El fichero para la tupla no pudo ser creado\n");
        return -1;
    }

    if (fread(&tupla, sizeof(Tuple), 1, original_tpl_fp) == 0){
        perror("[SERVIDOR][ERROR] Fichero no pudo leerse\n");
        fclose(original_tpl_fp);
        return -1;
    }

    fclose(original_tpl_fp);

    tupla.key = key2;

    // Obtiene nombre completo del fichero copia (key 2)
    sprintf(copy_tpl_file, "%s%d%s", filename, key2, ext);

    if ((copy_tpl_fp = fopen(copy_tpl_file, "w")) == NULL){
        perror("[SERVIDOR][ERROR] El fichero para la tupla no pudo ser creado/abierto\n");
        return -1;
    }

    if (fwrite(&tupla, sizeof(Tuple), 1, copy_tpl_fp) == 0){
        perror("[SERVIDOR][ERROR] No se pudo escribir en el fichero\n");
        fclose(copy_tpl_fp);
        return -1;
    }
    
    fclose(copy_tpl_fp);
    return 0;
}
