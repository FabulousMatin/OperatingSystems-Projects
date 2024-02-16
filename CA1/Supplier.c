
#include "Supplier.h"

void log_event(Supplier supplier, char* message, char* log_type) {
    char file_name[100] = {0};
    strcat(file_name, "./logs/");
    strcat(file_name, supplier.name);
    strcat(file_name, "_logs.txt");

    int fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
    
    if (fd != -1) {
        write(fd, log_type, strlen(log_type));
        write(fd, " (", 2);
        write(fd, "supplier", 8);
        write(fd, " = ", 3);
        write(fd, supplier.name, strlen(supplier.name));
        write(fd, ") : ", 4);
        write(fd, message, strlen(message));
        write(fd, "\n", 1);
        close(fd);  
    } 
    else {
        printf("Failed to open the file.\n");
    }
}



void handle_input_broadcast(char buffer[], Supplier supplier) {
    char* data, *type;
    int port;
    int id = decode(buffer, &port, &type, &data);
    if(id == getpid())
        return;
    // restaurant open
    if(!strcmp(type, REST_OPEN)) {
        printf("%s start working\n", data);
    }
    else if(!strcmp(type, REST_CLOSE)) {
        printf("%s stop working\n", data);
    }
    // name check
    else if(!strcmp(type, CHECK_NAME)) {
        int server = connect_to_server(port, supplier.tcp.port);
        char is_equal[2] = "0\0";
        if(!strcmp(supplier.name, data)) {
            sprintf(is_equal, "1");
        }
        if(send(server, is_equal, strlen(is_equal), 0) < 0) {
            printf("not sent\n");
        }
        close(server);
    }
    // give supplier info to restaurant
    else if(!strcmp(type, GET_SUPP_INFO)) {
        int server = connect_to_server(port, supplier.tcp.port);
        
        char* response = encode(supplier.name, RESP_SUPP_INFO, supplier.tcp.port);
        if(send(server, response, strlen(response), 0) < 0) {
            printf("not sent\n");
        }
        close(server);
    }

}

void answer_request(Supplier* supplier) {
    int BUF_SIZE = 5;
    char buff[BUF_SIZE];
    memset(buff, 0, BUF_SIZE);
    if(supplier->is_serving) {
        write(STDIN_FILENO, "request from: ", 14);
        printf(YELLOW);
        write(STDIN_FILENO, supplier->order.restaurant_name, strlen(supplier->order.restaurant_name) + 1);
        printf(COL_RESET);
        write(STDIN_FILENO, "\nyour answer: ("GREEN"yes"COL_RESET"/"RED"no"COL_RESET")\n", 24 + 2 * COL_LEN);
        
        read(STDIN_FILENO, buff, BUF_SIZE);
        buff[strlen(buff) - 1] = 0;
        if(send(supplier->order.client.fd, buff, strlen(buff), 0) < 0) {
            printf("not sent\n");
        }
        log_event(*supplier, concat_strings(5, "said ", buff, " to ", supplier->order.restaurant_name, " request"), INFO);
        supplier->is_serving = 0;
    }
    else {
        printf(PURPLE"No order!"COL_RESET"\n");
    }
}

void handle_command(char command[], Supplier* supplier) {
    if(!strcmp(command, "answer request")) {
        answer_request(supplier);
    }
    else
        write(STDIN_FILENO, RED"no such command!\n"COL_RESET, 17 + COL_LEN);
}





void handle_request(char* request, Supplier* supplier) {
    supplier->order.restaurant_name = (char*)malloc(strlen(request) + 1);
    strcpy(supplier->order.restaurant_name, request);

    log_event(*supplier, "got a new request", INFO);
    printf("new request just arrived!\n");
}


void authenticate(Supplier* supplier) {
    get_username(&supplier->name);
    while(!check_name(supplier->name, supplier->tcp, supplier->udp)) {
        printf("Username is already taken!\n");
        sleep(1);
        get_username(&supplier->name);
    }
    log_event(*supplier, "joined", INFO);
    write(STDIN_FILENO, "Welcome ", 9);
    write(STDIN_FILENO, supplier->name, strlen(supplier->name));
    write(STDIN_FILENO, " as a ", 6);
    write(STDIN_FILENO, "supplier!\n", 10);
}

void initialize(Supplier* supplier, char* udp_port) {
    supplier->is_serving = 0;
    supplier->udp.port = atoi(udp_port);
    supplier->tcp.port = generate_tcp_port();
    supplier->tcp.fd = make_server(supplier->tcp.port, &supplier->tcp.addr);
    supplier->udp.fd = make_broadcast(&supplier->udp.addr, supplier->udp.port);
}

void add_order(Supplier* supplier, int client_port, int client_fd) {
    supplier->order.client.port = client_port;
    supplier->order.client.fd = client_fd;
    supplier->is_serving = 1;
}

int main(int argc, char* argv[]) {

    char buffer[MAX_BUFFER_SIZE] = {0};
    Supplier supplier;
    
    initialize(&supplier, argv[1]);
    authenticate(&supplier);
    


    Fds fds;
    FD_ZERO(&fds.master);
    fds.max = -1;
    add_fd(STDIN_FILENO, &fds);
    add_fd(supplier.tcp.fd, &fds);
    add_fd(supplier.udp.fd, &fds);

    while (1) {
        memset(buffer, 0, MAX_BUFFER_SIZE);
        fds.working = fds.master;

        select(fds.max + 1, &fds.working, NULL, NULL, NULL);

        for(int i = 0; i <= fds.max; i++) {
            if(!FD_ISSET(i, &fds.working))
                continue;

            // terminal command 
            if (FD_ISSET(STDIN_FILENO, &fds.working)) {
                read(STDIN_FILENO, buffer, MAX_BUFFER_SIZE);
                buffer[strlen(buffer) - 1] = 0;
                handle_command(buffer, &supplier);
            }
            // get a new broadcasted message
            else if(FD_ISSET(supplier.udp.fd, &fds.working)) {
                recv(supplier.udp.fd, buffer, MAX_BUFFER_SIZE, 0); 
                buffer[strlen(buffer)] = 0;
                handle_input_broadcast(buffer, supplier); 
            }
            // client connection
            else if(FD_ISSET(supplier.tcp.fd, &fds.working) && !supplier.is_serving) {
                int client_port;
                int client_fd = accept_client(supplier.tcp.fd, &client_port);
                if(client_fd < 0) {
                    general_errors();
                    continue;
                }
                add_order(&supplier, client_port, client_fd);
                add_fd(client_fd, &fds);
            }
            // client request or client disconnection
            else if(FD_ISSET(i, &fds.working)) {
                if(recv(i , buffer, MAX_BUFFER_SIZE, 0) == 0) {
                    end_connection(i, &fds);
                    supplier.is_serving = 0;
                    continue;
                }
                handle_request(buffer, &supplier);
            }
        }
    }
}