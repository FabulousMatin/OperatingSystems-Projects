#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "defs.h"

char* concat_strings(int num_strings, ...) {
    va_list arguments;
    va_start(arguments, num_strings);

    size_t total_length = 0;
    for (int i = 0; i < num_strings; i++) {
        char* str = va_arg(arguments, char*);
        total_length += strlen(str);
    }

    char* result = (char*)malloc((total_length + 1) * sizeof(char));

    size_t current_pos = 0;
    va_start(arguments, num_strings);
    for (int i = 0; i < num_strings; i++) {
        char* str = va_arg(arguments, char*);
        size_t str_length = strlen(str);
        memcpy(result + current_pos, str, str_length);
        current_pos += str_length;
    }
    result[current_pos] = '\0';

    va_end(arguments);

    return result;
}

char* encode(char data[], char type[], int port) {
    char* buffer = malloc(sizeof (char) * MAX_BUFFER_SIZE);
    memset(buffer, 0, MAX_BUFFER_SIZE);

    // add id
    strcat(buffer, "id:");
    sprintf(buffer + strlen(buffer), "%d", getpid());
    strcat(buffer, "|");

    // add port
    strcat(buffer, "port:");
    sprintf(buffer + strlen(buffer), "%d", port);
    strcat(buffer, "|");
    
    // add type
    strcat(buffer, "type:");
    strcat(buffer, type);
    strcat(buffer, "|");

    // add data
    strcat(buffer, "data:");
    strcat(buffer, data);

    return buffer;
}

int decode(char buffer[], int* port, char* type[], char* data[]) {

    char* id_token = strtok(buffer, "|");
    char* port_token = strtok(NULL, "|");
    char* type_token = strtok(NULL, "|");
    char* data_token = strtok(NULL, "|");
    
    char* id = strchr(id_token, ':') + 1; 
    *port = atoi(strchr(port_token, ':') + 1);
    *type = strchr(type_token, ':') + 1; 
    *data = strchr(data_token, ':') + 1; 

    return atoi(id);
}

void decode_food_request(char* req, char* customer_name[], char* food_name[]) {
    char* customer_token = strtok(req, "|");
    char* food_token = strtok(NULL, "|");

    *customer_name = strchr(customer_token, ':') + 1; 
    *food_name = strchr(food_token, ':') + 1; 
}

void encode_food_request(char* request[], char customer_name[], char food_name[]) {
    int BUF_SIZE = 100;
    char buffer[BUF_SIZE];
    memset(buffer, 0, BUF_SIZE);

    strcat(buffer,"customer:");
    strcat(buffer, customer_name);
    strcat(buffer,"|");
    strcat(buffer,"food:");
    strcat(buffer, food_name);

    *request = buffer;
}



