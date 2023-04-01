#ifndef COMUNICACION_H
#define COMUNICACION_H

#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
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
    int sock_client;
    Tuple content;
    int second_key;
} Request;

// Estructura para respuesta del servidor a cliente
typedef struct service{
    int status;
    Tuple content;
} Service;

int recvMessage(int socket, char *buffer, int len)
{
	int r, l = len;
		
	do {	
		r = read(socket, buffer, l);
		l -= r ;
		buffer += r;
	} while ((l>0) && (r>=0));
	
	if (r < 0)
		return (-1);   /* fallo */
	return(0);	/* full length has been receive */
}

int sendMessage(int socket, char * buffer, int len)
{
	int r, l = len;
		
	do {	
		r = write(socket, buffer, l);
		l -= r;
		buffer += r;
	} while ((l>0) && (r>=0));
	
	if (r < 0)
		return (-1);   /* fail */
	return(0);	/* full length has been sent */
}

ssize_t readLine(int fd, void *buffer, size_t n){
	ssize_t numRead; /* num of bytes fetched by last read() */
	size_t totRead; /* total bytes read so far */
	char *buf;
	char ch;
	if (n <= 0 || buffer == NULL) {
		errno = EINVAL;
		return -1;
	}
	buf = buffer;
	totRead = 0;
	for (;;) {
		numRead = read(fd, &ch, 1); /* read a byte */
		if (numRead == -1) {
			if (errno == EINTR) /* interrupted -> restart read() */
				continue;
			else
				return -1; /* some other error */
		} else if (numRead == 0) { /* EOF */
			if (totRead == 0) /* no byres read; return 0 */
				return 0;
			else
				break;
		} else { /* numRead must be 1 if we get here*/
			if (ch == '\n')
				break;
			if (ch == '\0')
				break;
			if (totRead < n - 1) { /* discard > (n-1) bytes */
				totRead++;
				*buf++ = ch;
			}
		}
	}
	*buf = '\0';
	return totRead;
}

#endif