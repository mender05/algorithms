#ifndef	SOCKET_H_
#define SOCKET_H_

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <stdio.h>
#include <string>

extern int m_socket_id;
uint8 buildSocket(char *argv[]);

#endif