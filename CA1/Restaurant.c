#include "Restaurant.h"

void log_event(Restaurant rstrnt, char* message, char* log_type) {
    char file_name[100] = {0};
    strcat(file_name, "./logs/");
    strcat(file_name, rstrnt.name);
    strcat(file_name, "_logs.txt");

    int fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
    
    if (fd != -1) {
        write(fd, log_type, strlen(log_type));
        write(fd, " (", 2);
        write(fd, "restaurant", 10);
        write(fd, " = ", 3);
        write(fd, rstrnt.name, strlen(rstrnt.name));
        write(fd, ") : ", 4);
        write(fd, message, strlen(message));
        write(fd, "\n", 1);
        close(fd);  
    } 
    else {
        printf("Failed to open the file.\n");
    }
}

void show_suppliers(Restaurant rstrnt) {
    log_event(rstrnt ,"wants all suppliers", INFO);

    broadcast(encode("", GET_SUPP_INFO, rstrnt.tcp.port), rstrnt.udp);
    char buffer[MAX_BUFFER_SIZE];
    char* data, *type;
    int port, id;
    
    printf(BLUE"supplier/port"COL_RESET"\n");
    printf("-----------------------------------\n");
    while(1) {

        memset(buffer, 0, MAX_BUFFER_SIZE);
        signal(SIGALRM, ignore);
        siginterrupt(SIGALRM, 1);
        alarm(1);
        int new_client = accept_client(rstrnt.tcp.fd, NULL);
        alarm(0);

        if(recv(new_client, buffer, MAX_BUFFER_SIZE, 0) < 0){
            break;
        } 
        id = decode(buffer, &port, &type, &data);
        printf(YELLOW"%s"COL_RESET" - "PURPLE"%d"COL_RESET"\n", data, port);
        printf("-----------------------------------\n");
        close(new_client);

    }
}

void handle_input_broadcast(char buffer[], Restaurant rstrnt) {
    char* data, *type;
    int port;
    int id = decode(buffer, &port, &type, &data);
    if(id == getpid())
        return;
    
    if(!strcmp(type, CHECK_NAME)) {
        int as_client = connect_to_server(port, rstrnt.tcp.port);
        char is_equal[2] = "0";
        if(!strcmp(rstrnt.name, data)) {
            sprintf(is_equal, "1");
        }
        if(send(as_client, is_equal, strlen(is_equal), 0) < 0) {
            printf("not sent\n");
        }
        close(as_client);
    }

    else if(!strcmp(type, GET_RSTRNT_INFO)) {
        int as_client = connect_to_server(port, rstrnt.tcp.port);
        char* response = encode(rstrnt.name, RESP_RSTRNT_INFO, rstrnt.tcp.port);
        if(send(as_client, response, strlen(response), 0) < 0) {
            printf("not sent\n");
        }
        close(as_client);
    }
}

void remove_ingredient(Restaurant* rstrnt, int index) {
    log_event(*rstrnt, concat_strings(3, "ingredient ", rstrnt->ingredients[index].name, " finished"), WARNING);
    for (int i = index; i < rstrnt->n_ingredient - 1; i++) {
        rstrnt->ingredients[i] = rstrnt->ingredients[i + 1];
    }
    rstrnt->ingredients = (Ingredient*)realloc(rstrnt->ingredients, --rstrnt->n_ingredient * sizeof(Ingredient));
    
}

void reduce_ingredient(Restaurant* rstrnt, char ingredient[], int amount) {
    for(int i = 0; i < rstrnt->n_ingredient; i++) {
        if(!strcmp(rstrnt->ingredients[i].name, ingredient)) {
            rstrnt->ingredients[i].amount -= amount;
            if(rstrnt->ingredients[i].amount == 0) 
                remove_ingredient(rstrnt, i);
        }
    }
}
     

int have_ingredient(Restaurant* rstrnt, char ingredient[], int amount) {
    for(int i = 0; i < rstrnt->n_ingredient; i++) {
        if(!strcmp(rstrnt->ingredients[i].name, ingredient)) {
            if(rstrnt->ingredients[i].amount >= amount) {
                return 1;
            }
        }
    }
    return 0;
}

void add_ingredient(Restaurant* rstrnt, char ingredient[], int amount){
    for(int i = 0; i < rstrnt->n_ingredient; i++) {
        if(!strcmp(rstrnt->ingredients[i].name, ingredient)) {
            rstrnt->ingredients[i].amount += amount;
            return;
        }
    }
    log_event(*rstrnt, concat_strings(3, "ingredient ", ingredient, " added"), INFO);
    rstrnt->ingredients = (Ingredient*)realloc(rstrnt->ingredients, sizeof(Ingredient) * (rstrnt->n_ingredient + 1));
    rstrnt->ingredients[rstrnt->n_ingredient].amount = amount;
    rstrnt->ingredients[rstrnt->n_ingredient].name = (char*)malloc(strlen(ingredient) + 1);
    strcpy(rstrnt->ingredients[rstrnt->n_ingredient].name, ingredient);
    rstrnt->n_ingredient++;
}

void show_ingredients(Restaurant rstrnt) {
    printf(BLUE"ingredient / amount"COL_RESET"\n");
    printf("--------------------------\n");
    for(int i = 0; i < rstrnt.n_ingredient; i++) {
        printf(YELLOW"%s"PURPLE" / %d"COL_RESET"\n", rstrnt.ingredients[i].name, rstrnt.ingredients[i].amount);
        printf("--------------------------\n");
    }
}

void request_ingredient(Restaurant* rstrnt) {
    show_suppliers(*rstrnt);

    int BUF_SIZE = 20;
    char buffer[BUF_SIZE];
    char ingredient[BUF_SIZE];
    char port[BUF_SIZE];
    // port
    memset(port, 0, BUF_SIZE);
    write(STDIN_FILENO, "port of supplier: ", 18);
    read(STDIN_FILENO, port, BUF_SIZE);
    int server_port = atoi(port);
    // ingredient
    memset(ingredient, 0, BUF_SIZE);
    write(STDIN_FILENO, "name of ingredient: ", 20);
    read(STDIN_FILENO, ingredient, BUF_SIZE);
    ingredient[strlen(ingredient) - 1] = 0;
    // amount
    memset(buffer, 0, BUF_SIZE);
    write(STDIN_FILENO, "amount of ingredient: ", 22);
    read(STDIN_FILENO, buffer, BUF_SIZE);
    buffer[strlen(buffer) - 1] = 0;
    int amount = atoi(buffer);

    //send request
    int server = connect_to_server(server_port, rstrnt->tcp.port);
    if(server == -1){
        log_event(*rstrnt, "Error in connection", ERROR);
        return;
    }
    send(server, rstrnt->name, strlen(rstrnt->name), 0);
    lock_terminal();
    log_event(*rstrnt, concat_strings(6, "request ", buffer ,"amount of ", ingredient, " from a supplier with port " ,port), INFO);
    
    // recieve response
    memset(buffer, 0, BUF_SIZE);
    signal(SIGALRM, supplier_not_respond);
    siginterrupt(SIGALRM, 1);
    alarm(90);
    if(recv(server, buffer, BUF_SIZE, 0) < 0){
        unlock_terminal();
        log_event(*rstrnt, "request timed-out", WARNING);
        close(server);
        return;
    }
    unlock_terminal();
    alarm(0);
    if(!strcmp(buffer, "yes")) {
        printf(GREEN"request accepted!"COL_RESET"\n");
        log_event(*rstrnt, "request accepted", INFO);
        add_ingredient(rstrnt, ingredient, amount);
    }
    else if(!strcmp(buffer, "no")) {
        printf(RED"request denied"COL_RESET"\n");
        log_event(*rstrnt, "request denied", INFO);
    }
    else {
        log_event(*rstrnt, "got a bad response", INFO);
        printf("bad respose\n");
    }
    close(server);

}
void show_requests(Restaurant rstrnt) {
    printf(BLUE"name/food/port"COL_RESET"\n");
    printf("-------------------------------\n");
    for(int i = 0; i < rstrnt.n_orders; i++) {
        printf(YELLOW"%s"COL_RESET"/", rstrnt.orders[i].customer_name);
        printf(GREEN"%s"COL_RESET"/", rstrnt.orders[i].food_name);
        printf(PURPLE"%d"COL_RESET"\n", rstrnt.orders[i].client.port);
        printf("-------------------------------\n");
    }
}

int get_fd(Restaurant rstrnt, int port) {
    for(int i = 0; i < rstrnt.n_orders; i++) {
        if(rstrnt.orders[i].client.port == port) 
            return rstrnt.orders[i].client.fd;
    }
    return -1;
}


int can_afford_food(Restaurant* rstrnt, int client_port) {
    int order_index = -1;
    for(int i = 0; i < rstrnt->n_orders; i++) {
        if(rstrnt->orders[i].client.port == client_port) {
            order_index = i;
            break;
        }
    }
    if(order_index == -1) {
        printf("user does not exit\n");
        rstrnt->orders[order_index].status = DENIED;
        return -1;
    }

    int food_index = -1;
    for(int i = 0; i < rstrnt->n_recipes; i++){
        if(!strcmp(rstrnt->recipes[i].name, rstrnt->orders[order_index].food_name)) {
            food_index = i;
            break;
        }
    }
    if(food_index == -1){
        printf("food does not exist\n");
        rstrnt->orders[order_index].status = DENIED;
        return -1;
    }

    for(int i = 0; i < rstrnt->recipes[food_index].n_ingredients; i++) {
        if(!have_ingredient(rstrnt, rstrnt->recipes[food_index].ingredients[i].name, rstrnt->recipes[food_index].ingredients[i].amount)) {
            rstrnt->orders[order_index].status = DENIED;
            return 0;
        }
    }

    for(int i = 0; i < rstrnt->recipes[food_index].n_ingredients; i++) {
        reduce_ingredient(rstrnt, rstrnt->recipes[food_index].ingredients[i].name, rstrnt->recipes[food_index].ingredients[i].amount);
    }
    rstrnt->orders[order_index].status = ACCEPTED;
    return 1;
}

void show_history(Restaurant rstrnt) {
    log_event(rstrnt, "checked order history", INFO);

    printf(BLUE"name / food / status"COL_RESET);
    printf("------------------------\n");
    for(int i = 0; i < rstrnt.n_history; i++) {
        char status[10];
        memset(status, 0, 10);
        switch (rstrnt.histories[i].status){
            case 1: strcpy(status, "accepted"); break;
            case 0: strcpy(status, "denied"); break;
            case -1: strcpy(status, "timed-out"); break;
        }
        printf(YELLOW"%s"COL_RESET" /"GREEN" %s "COL_RESET"/ "PURPLE"%s"COL_RESET"\n", rstrnt.histories[i].customer, rstrnt.histories[i].food, status);
        printf("------------------------\n");
    }
}

void add_to_history(Restaurant* rstrnt, int client_fd) {
    rstrnt->histories = (History*)realloc(rstrnt->histories, sizeof(History) * (rstrnt->n_history + 1));

    for(int i = 0; i < rstrnt->n_orders; i++) {
        if(rstrnt->orders[i].client.fd == client_fd) {
            rstrnt->histories[rstrnt->n_history].customer = (char*)malloc(strlen(rstrnt->orders[i].customer_name) + 1);
            strcpy(rstrnt->histories[rstrnt->n_history].customer, rstrnt->orders[i].customer_name);

            rstrnt->histories[rstrnt->n_history].food = (char*)malloc(strlen(rstrnt->orders[i].food_name) + 1);
            strcpy(rstrnt->histories[rstrnt->n_history].food, rstrnt->orders[i].food_name);

            rstrnt->histories[rstrnt->n_history].status = rstrnt->orders[i].status;
            

            rstrnt->n_history++;
            break;
        }
    }
}


void get_info(Restaurant* rstrnt, int port, int* order_index, char* customer[], char* food[]) {
    for(int i = 0; i < rstrnt->n_orders; i++) {
        if(rstrnt->orders[i].client.port == port) {
            *order_index = i;
            *customer = rstrnt->orders[i].customer_name;
            *food = rstrnt->orders[i].food_name;
        }
    }
}

void set_order_status(Restaurant* rstrnt, int client_port, int status) {
    for(int i = 0; i < rstrnt->n_orders; i++) {
        if(rstrnt->orders[i].client.port == client_port) {
            rstrnt->orders[i].status = status;
            break;
        }
    }
}

int validate_response(Restaurant* rstrnt, int client_port, char* response) {
    char cp[5] = {0};
    sprintf(cp, "%d", client_port);

    if(!strcmp(response, "yes")) {
        int status = can_afford_food(rstrnt, client_port);
        if(status == 1) {
            printf("done!\n");
            log_event(*rstrnt, concat_strings(2, "accepted request of ", cp), INFO);
            set_order_status(rstrnt, client_port, ACCEPTED);
            return 1;
        }
        else if(status == 0) {
            printf("not enough ingredients!\n");
            log_event(*rstrnt, concat_strings(3, "denied request of ", cp, " due to lack of ingredients"), INFO);
            set_order_status(rstrnt, client_port, DENIED);
            return 0;
        }
        else if(status == -1) {
            printf("some error has happen!\n");
            log_event(*rstrnt, concat_strings(2, "denied request of ", cp), INFO);
            set_order_status(rstrnt, client_port, DENIED);
            return 0;
        }
    }
    else {
        set_order_status(rstrnt, client_port, DENIED);
        log_event(*rstrnt, concat_strings(2, "denied request of ", cp), INFO);
        return 0;
    }
}

void answer_request(Restaurant* rstrnt) {
    show_requests(*rstrnt);

    int BUF_SIZE = 10;
    char buff[BUF_SIZE];
    memset(buff, 0, BUF_SIZE);

    write(STDIN_FILENO, "port of request: ", 17);
    read(STDIN_FILENO, buff, BUF_SIZE);
    int port = atoi(buff);

    memset(buff, 0, BUF_SIZE);
    write(STDIN_FILENO, "your answer: ", 13);
    read(STDIN_FILENO, buff, BUF_SIZE);
    buff[strlen(buff) - 1] = 0;

    char response[4];
    memset(response, 0, 4);
    switch (validate_response(rstrnt, port, buff)) {
        case 1: strcpy(response, "yes"); break;
        case 0: strcpy(response, "no"); break;
    }
    
    int client_fd = get_fd(*rstrnt, port);
    if(client_fd == -1) {
        printf("bad errrrrr!\n");
    }
    if(send(client_fd, response, strlen(response), 0) < 0) {
        printf("not sent!\n");
    }

}



void handle_command(char command[], Restaurant* rstrnt) {

    if(!strcmp(command, "show suppliers"))
        show_suppliers(*rstrnt);
    else if(!strcmp(command, "show recipes"))
        show_recipes(rstrnt->recipes, REST_GET_RECIPES, rstrnt->n_recipes);
    else if(!strcmp(command, "request ingredient")) 
        request_ingredient(rstrnt);
    else if(!strcmp(command, "show requests")) 
        show_requests(*rstrnt);
    else if(!strcmp(command, "answer request"))
        answer_request(rstrnt);
    else if(!strcmp(command, "show ingredients"))
        show_ingredients(*rstrnt);
    else if(!strcmp(command, "show history"))
        show_history(*rstrnt);
    else if(!strcmp(command, "break")) {
        if(rstrnt->n_orders > 0) {
            log_event(*rstrnt, "can not close the resturant while having orders!", WARNING);
            printf(RED"can not close the resturant while having orders!"COL_RESET"\n");
            return;
        }
        log_event(*rstrnt, "stop working", INFO);
        broadcast(encode(rstrnt->name, REST_CLOSE, 0), rstrnt->udp);
        exit(EXIT_SUCCESS);
    }
    else
        write(STDIN_FILENO, RED"no such command!\n"COL_RESET, 17 + COL_LEN);
        
}


void handle_request(char* buffer, Restaurant* rstrnt, int client_fd) {
    char* customer_name, *food_name;
    decode_food_request(buffer, &customer_name, &food_name);
    for(int i = 0; i < rstrnt->n_orders; i++) {
        if(rstrnt->orders[i].client.fd == client_fd) {
            rstrnt->orders[i].customer_name = (char*)malloc(strlen(customer_name) + 1);
            strcpy(rstrnt->orders[i].customer_name, customer_name);
            rstrnt->orders[i].food_name = (char*)malloc(strlen(food_name) + 1);
            strcpy(rstrnt->orders[i].food_name, food_name);
            break;
        }
    }
    log_event(*rstrnt, "got a new request", INFO);
    printf("new request just arrived!\n");
}

void remove_order(Restaurant* rstrnt, int index) {
    for (int i = index; i < rstrnt->n_orders - 1; i++) {
        rstrnt->orders[i] = rstrnt->orders[i + 1];
    }
    rstrnt->orders = (Order*)realloc(rstrnt->orders, --rstrnt->n_orders * sizeof(Order));
}

void end_order(Restaurant* rstrnt, int client_fd) {
    add_to_history(rstrnt, client_fd);
    for(int i = 0; i < rstrnt->n_orders; i++) {
        if(rstrnt->orders[i].client.fd == client_fd){
            remove_order(rstrnt, i);
            break;
        }
    }
}

void add_order(Restaurant* rstrnt, int client_port, int client_fd) {
    rstrnt->orders = (Order*)realloc(rstrnt->orders, sizeof(Order) * (rstrnt->n_orders + 1));
    rstrnt->orders[rstrnt->n_orders].client.port = client_port;
    rstrnt->orders[rstrnt->n_orders].client.fd = client_fd;
    rstrnt->orders[rstrnt->n_orders].status = TIMEDOUT;// default, unless ...
    rstrnt->n_orders++;
}

void initialize(Restaurant* rstrnt, char* udp_port) {
    rstrnt->udp.port = atoi(udp_port);
    rstrnt->tcp.port = generate_tcp_port();
    rstrnt->tcp.fd = make_server(rstrnt->tcp.port, &rstrnt->tcp.addr);
    rstrnt->udp.fd = make_broadcast(&rstrnt->udp.addr, rstrnt->udp.port);

    rstrnt->n_ingredient = 0;
    rstrnt->n_orders = 0;
    rstrnt->n_history = 0;
    rstrnt->status = CLOSED;

    rstrnt->orders = NULL;
    rstrnt->histories = NULL;
    rstrnt->ingredients = NULL;

    rstrnt->n_recipes = get_recipes(&rstrnt->recipes);
}

void wait_opening(Restaurant* rstrnt) {
    char buffer[MAX_BUFFER_SIZE];
    sleep(1);
    while(rstrnt->status != OPEN) {
        memset(buffer, 0, MAX_BUFFER_SIZE);
        read(STDIN_FILENO, buffer, MAX_BUFFER_SIZE);
        buffer[strlen(buffer) - 1] = 0;
         if(!strcmp(buffer, "start working")) {
            write(STDIN_FILENO, GREEN"Opened!"COL_RESET"\n", 8 + COL_LEN);
            log_event(*rstrnt, "start working", INFO);
            rstrnt->status = OPEN;
        }
        else
            write(STDIN_FILENO, RED"You are closed!"COL_RESET"\n", 16 + COL_LEN);
    }
}


void authenticate(Restaurant* rstrnt) {
    get_username(&rstrnt->name);
    while(!check_name(rstrnt->name, rstrnt->tcp, rstrnt->udp)) {
        printf("Username is already taken!\n");
        sleep(1);
        get_username(&rstrnt->name);
    }
    log_event(*rstrnt, "joined", INFO);
    write(STDIN_FILENO, "Welcome ", 9);
    write(STDIN_FILENO, rstrnt->name, strlen(rstrnt->name));
    write(STDIN_FILENO, " as a ", 6);
    write(STDIN_FILENO, "restaurant!\n", 12);
}

int main(int argc, char* argv[]) {
    char buffer[MAX_BUFFER_SIZE] = {0};

    Restaurant rstrnt;


    authenticate(&rstrnt);
    wait_opening(&rstrnt);
    initialize(&rstrnt, argv[1]);
    broadcast(encode(rstrnt.name, REST_OPEN, 0), rstrnt.udp);
    

    Fds fds;
    FD_ZERO(&fds.master);
    fds.max = -1;
    add_fd(STDIN_FILENO, &fds);
    add_fd(rstrnt.tcp.fd, &fds);
    add_fd(rstrnt.udp.fd, &fds);

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
                handle_command(buffer, &rstrnt);
            }

            // input broadcast
            else if(FD_ISSET(rstrnt.udp.fd, &fds.working)) {
                recv(rstrnt.udp.fd, buffer, MAX_BUFFER_SIZE, 0); 
                buffer[strlen(buffer)] = 0;
                handle_input_broadcast(buffer, rstrnt); 
            }

            // new client => new order!
            else if(FD_ISSET(rstrnt.tcp.fd, &fds.working)) {
                int client_port;
                int client_fd = accept_client(rstrnt.tcp.fd, &client_port);
                if(client_fd < 0) {
                    general_errors();
                    continue;
                }

                add_order(&rstrnt, client_port, client_fd);
                add_fd(client_fd, &fds);
                
            }
            // client order / client end connection
            else if(FD_ISSET(i, &fds.working)) {
                if(recv(i , buffer, MAX_BUFFER_SIZE, 0) == 0) {
                    end_order(&rstrnt, i);
                    end_connection(i, &fds);
                    continue;
                }
                handle_request(buffer, &rstrnt, i);
            }
        }
    }
}