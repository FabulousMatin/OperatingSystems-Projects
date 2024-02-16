
#include "Customer.h"

void log_event(Customer cstmr, char* message, char* log_type) {
    char file_name[100] = {0};
    strcat(file_name, "./logs/");
    strcat(file_name, cstmr.name);
    strcat(file_name, "_logs.txt");

    int fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
    
    if (fd != -1) {
        write(fd, log_type, strlen(log_type));
        write(fd, " (", 2);
        write(fd, "customer", 8);
        write(fd, " = ", 3);
        write(fd, cstmr.name, strlen(cstmr.name));
        write(fd, ") : ", 4);
        write(fd, message, strlen(message));
        write(fd, "\n", 1);
        close(fd);  
    } 
    else {
        write(STDERR_FILENO ,"Failed to open the file.\n", 25);
    }
}

void show_restaurants(Customer cstmr) {
    log_event(cstmr, "wants all restaurants", INFO);

    broadcast(encode("", GET_RSTRNT_INFO, cstmr.tcp.port), cstmr.udp);
    char buffer[MAX_BUFFER_SIZE];
    char* data, *type;
    int port, id;
    
    printf(BLUE"restaurant/port\n"COL_RESET);
    printf("-------------------------------------------\n");
    while(1) {
        memset(buffer, 0, MAX_BUFFER_SIZE);
        signal(SIGALRM, ignore);
        siginterrupt(SIGALRM, 1);
        alarm(1);
        int new_client = accept_client(cstmr.tcp.fd, NULL);
        alarm(0);

        if(recv(new_client, buffer, MAX_BUFFER_SIZE, 0) < 0){
            break;
        } 
        id = decode(buffer, &port, &type, &data);
        printf(YELLOW"%s"COL_RESET" - "PURPLE"%d"COL_RESET"\n", data, port);
        printf("-------------------------------------------\n");
        close(new_client);
    }
}

void handle_input_broadcast(char buffer[], Customer cstmr) {
    char* data, *type;
    int port;
    int id = decode(buffer, &port, &type, &data);
    if(id == getpid())
        return;
    if(!strcmp(type, REST_OPEN)) {
        printf("%s start working\n", data);
    }
    else if(!strcmp(type, REST_CLOSE)) {
        printf("%s stop working\n", data);
    }
    else if(!strcmp(type, CHECK_NAME)) {
        int server = connect_to_server(port, cstmr.tcp.port);
        char is_equal[2] = "0";
        if(!strcmp(cstmr.name, data)) {
            sprintf(is_equal, "1");
        }
        if(send(server, is_equal, strlen(is_equal), 0) < 0) {
            printf("not sent\n");
        }
        close(server);
    }
}


void request_food(Customer cstmr) {
    show_restaurants(cstmr);

    int BUF_SIZE = 20;
    char buffer[BUF_SIZE];
    char food[BUF_SIZE];
    char port[BUF_SIZE];
    // port
    memset(port, 0, BUF_SIZE);
    write(STDIN_FILENO, "port of restaurant: ", 20);
    read(STDIN_FILENO, port, BUF_SIZE);
    int server_port = atoi(port);
    port[strlen(port) - 1] = 0;
    // food
    memset(food, 0, BUF_SIZE);
    write(STDIN_FILENO, "name of food: ", 14);
    read(STDIN_FILENO, food, BUF_SIZE);
    food[strlen(food) - 1] = 0;
    if(is_food_exist(cstmr.recipes, cstmr.n_recipes, food) == 0) {
        log_event(cstmr, "Bad request", ERROR);
        printf(RED"food does not exist"COL_RESET"\n");
        return;
    } 
    // send request
    char* request;
    encode_food_request(&request, cstmr.name, food);
    int server = connect_to_server(server_port, cstmr.tcp.port);
    if(server == -1){
        log_event(cstmr, "Error in connection", ERROR);
        return;
    }
    send(server, request, strlen(request), 0);
    lock_terminal();
    log_event(cstmr, concat_strings(4, "request ", food, " from a restaurant with port ", port), INFO);

    // recieve response
    memset(buffer, 0, BUF_SIZE);
    signal(SIGALRM, restaurant_not_respond);
    siginterrupt(SIGALRM, 1);
    alarm(120);
    if (recv(server, buffer, BUF_SIZE, 0) < 0) {
        unlock_terminal();
        log_event(cstmr, "request timed-out!", WARNING);
        close(server);
        return;
    }
    alarm(0);
    unlock_terminal();
    if(!strcmp(buffer, "yes")) {
        log_event(cstmr, "request accepted", INFO);
        printf(GREEN"request accepted!"COL_RESET"\n");
    }
    else if(!strcmp(buffer, "no")) {
        log_event(cstmr, "request denied", INFO);
        printf(RED"request denied"COL_RESET"\n");
    }
    else {
        log_event(cstmr, "got a bad respond", INFO);
        printf("bad respond\n");
    }
    close(server);
}

void handle_command(char command[], Customer cstmr) {
    if(!strcmp(command, "show restaurants"))
        show_restaurants(cstmr);
    else if(!strcmp(command, "show recipes")) {
        log_event(cstmr, "wants all recepies", INFO);
        show_recipes(cstmr.recipes, CUST_GET_RECIPES, cstmr.n_recipes);
    }
    else if(!strcmp(command, "request food")) 
        request_food(cstmr);
    else
        write(STDIN_FILENO, RED"no such command!\n"COL_RESET, 17 + COL_LEN);

}




void authenticate(Customer* cstmr) {
    get_username(&cstmr->name);
    while(!check_name(cstmr->name, cstmr->tcp, cstmr->udp)) {
        printf("Username is already taken!\n");
        sleep(1);
        get_username(&cstmr->name);
    }
    log_event(*cstmr, "joined", INFO);

    write(STDIN_FILENO, "Welcome ", 9);
    write(STDIN_FILENO, cstmr->name, strlen(cstmr->name));
    write(STDIN_FILENO, " as a ", 6);
    write(STDIN_FILENO, "customer!\n", 10);
}

void initialize(Customer* cstmr, char* udp_port) {
    cstmr->udp.port = atoi(udp_port);
    cstmr->tcp.port = generate_tcp_port();
    cstmr->tcp.fd = make_server(cstmr->tcp.port, &cstmr->tcp.addr);
    cstmr->udp.fd = make_broadcast(&cstmr->udp.addr, cstmr->udp.port);

    cstmr->n_recipes = get_recipes(&cstmr->recipes);
}

int main(int argc, char* argv[]) {
    char buffer[MAX_BUFFER_SIZE] = {0};
    int received_bytes;

    Customer cstmr;

    initialize(&cstmr, argv[1]);
    authenticate(&cstmr);


    Fds fds;
    FD_ZERO(&fds.master);
    fds.max = -1;
    add_fd(STDIN_FILENO, &fds);
    add_fd(cstmr.tcp.fd, &fds);
    add_fd(cstmr.udp.fd, &fds);

    while (1) {
        memset(buffer, 0, MAX_BUFFER_SIZE);
        fds.working = fds.master;

        select(fds.max + 1, &fds.working, NULL, NULL, NULL);

        for(int i = 0; i <= fds.max; i++) {
            if(!FD_ISSET(i, &fds.working))
                continue;

            if (FD_ISSET(STDIN_FILENO, &fds.working)) {
                read(STDIN_FILENO, buffer, MAX_BUFFER_SIZE);
                buffer[strlen(buffer) - 1] = 0;
                handle_command(buffer, cstmr);
            }

            else if(FD_ISSET(cstmr.udp.fd, &fds.working)) {
                recv(cstmr.udp.fd, buffer, MAX_BUFFER_SIZE, 0); 
                buffer[strlen(buffer)] = 0;
                handle_input_broadcast(buffer, cstmr); 
            }
        }
    }
}