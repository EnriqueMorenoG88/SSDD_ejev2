#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)


# Iniciar el ejecutable del servidor en background
./servidor_ejev1 &

# Esperar a que el servidor arrance antes de iniciar el cliente
sleep 1

# Iniciar el ejecutable del cliente
./cliente_ejev1

# Dar permisos de ejecucion al script de shell y a los ejecutables de servidor y cliente
chmod +x run_server_client.sh
chmod +x servidor_ejev1
chmod +x cliente_ejev1

# Terminar el proceso servidor
killall -9 servidor_ejev1