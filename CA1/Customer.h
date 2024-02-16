#include "network.c"
#include "Food.c"

#include <unistd.h>
#include <asm-generic/socket.h>
#include <sys/types.h>
#include <sys/select.h>

#define TCP 8003


typedef struct
{
    char* name;
    Socket udp;
    Socket tcp;
    Recipe* recipes;
    int n_recipes;
} Customer;
