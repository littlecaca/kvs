#pragma once

#include "server.h"

#define NTYCO_BUFFER_LENGTH 1024

typedef int (*msg_handler)(char *msg, int length, char *response);

int ntyco_start(unsigned short port,  msg_handler handler);