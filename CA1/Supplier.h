#include "network.c"

#include <unistd.h>
#include <asm-generic/socket.h>
#include <sys/types.h>
#include <sys/select.h>

#define TCP 8002

typedef struct
{
    char* restaurant_name;
    char* ingredient;
    int amount;
    Client client;

} Order;


typedef struct
{
    char* name;
    Socket udp;
    Socket tcp;
    Order order;
    int is_serving;

} Supplier;
