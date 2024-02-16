#ifndef REST_H
#define REST_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <asm-generic/socket.h>
#include <sys/types.h>
#include <sys/select.h>

#define ACCEPTED 1
#define DENIED 0
#define TIMEDOUT -1

#define OPEN 1
#define CLOSED 0

#include "network.c"
#include "Food.c"


typedef struct
{
    char* customer;
    char* food;
    int status;

} History;

typedef struct
{
    char* customer_name;
    char* food_name;
    int status;
    Client client;

} Order;


typedef struct
{
    char* name;
    int status;

    Socket udp;
    Socket tcp;

    Ingredient* ingredients;
    int n_ingredient;
    
    Order* orders;
    int n_orders;
    
    History* histories;
    int n_history;
    
    Recipe* recipes;
    int n_recipes;

} Restaurant;

#endif