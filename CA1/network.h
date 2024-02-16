#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <asm-generic/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <signal.h>
#include <fcntl.h>

#include "defs.h"
#include "custom_string.c"