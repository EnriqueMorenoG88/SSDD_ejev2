CC=gcc
CFLAGS=-g -Wall
OBJS_SERVER=servidor.o libimplserv.a
OBJS_CLIENT=cliente.o 

all: libclaves.so $(OBJS_SERVER) $(OBJS_CLIENT) $(OBJS_RST)
	$(CC) $(CFLAGS) -o servidor_ejev2 $(OBJS_SERVER) -lpthread
	$(CC) $(CFLAGS) -o cliente_ejev2 $(OBJS_CLIENT) -L. -lclaves

libimplserv.a: impl_serv.o
	ar rc libimplserv.a impl_serv.o

libclaves.so: claves.c
	$(CC) $(CFLAGS) -shared -fPIC -o libclaves.so claves.c

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS_SERVER) $(OBJS_CLIENT) libclaves.so impl_serv.o servidor_ejev2 cliente_ejev2