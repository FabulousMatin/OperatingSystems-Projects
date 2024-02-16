#ifndef DEFS_H
#define DEFS_H

char IP[] = "192.168.11.255";

#define ID_BUF_SIZE 10
#define TYPE_BUF_SIZE 10
#define MAX_BUFFER_SIZE 512 - (ID_BUF_SIZE + TYPE_BUF_SIZE)


// request type
#define NOTHING "NOTHING"
#define CHECK_NAME "name_check"
#define BROADCAST "broadcast"
#define RESPONSE "response"
#define GET_RSTRNT_INFO "get_rest"
#define GET_SUPP_INFO "get_supp"
#define RESP_RSTRNT_INFO "rest_resp"
#define RESP_SUPP_INFO "supp_resp"
#define REST_OPEN "rest_open"
#define REST_CLOSE "rest_close"

#define REST_GET_RECIPES 1
#define CUST_GET_RECIPES 0

#define INFO "[INFO]"
#define ERROR "[ERROR]"
#define WARNING "[WARNING]"

#define YELLOW "\033[33m"
#define PURPLE "\033[35m"
#define RED "\033[91m"
#define GREEN "\033[92m"
#define BLUE "\033[94m"
#define COL_RESET "\033[0m"
#define COL_LEN 9

void general_errors() {
    write(STDERR_FILENO, RED"Some error happened, please try again."COL_RESET"\n", 40 + COL_LEN);
}

void ignore() {
    
}
#endif