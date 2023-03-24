#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)


# Indicar IP y Puerto del servidor
export IP_TUPLAS=localhost
export PUERTO_TUPLAS=8080

# Iniciar el ejecutable del servidor en background con el puerto 8080
./servidor_ejev2 & 8080

# Esperar a que el servidor arrance antes de iniciar el cliente
sleep 1

# Iniciar el ejecutable del cliente
./cliente_ejev2

# Dar permisos de ejecucion al script de shell y a los ejecutables de servidor y cliente
chmod +x run_server_client.sh
chmod +x servidor_ejev2
chmod +x cliente_ejev2

# Terminar el proceso servidor
killall -9 servidor_ejev2