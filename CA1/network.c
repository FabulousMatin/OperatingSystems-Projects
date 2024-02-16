#include "network.h"

typedef struct {

    int fd;
    int port;
    int ip;
    struct sockaddr_in addr;
    
} Socket;

typedef struct {

    int max;
    fd_set master;
    fd_set working;

} Fds;
  
typedef struct {

    int port;
    int fd;

} Client;


struct termios saved_termios;

void lock_terminal() {
    tcgetattr(STDIN_FILENO, &saved_termios);
    struct termios new_termios = saved_termios;
    new_termios.c_lflag &= ~(ICANON);
    new_termios.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void unlock_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_termios);
}

int generate_tcp_port() {
    int fd, port;
    char buffer[10];
    memset(buffer, 0, 10);
    fd = open("port.txt", O_RDONLY, 0644);
    if (fd != -1) {
        read(fd, buffer, 10);
        port = atoi(buffer);
        close(fd);
        fd = open("port.txt", O_WRONLY, 0644);
        memset(buffer, 0, 10);
        sprintf(buffer, "%d", port + 2);
        write(fd, buffer, 10);
        close(fd);
    }

    return port;
}





void add_fd(int new_fd, Fds* fds) {
    FD_SET(new_fd, &fds->master);
    if(new_fd > fds->max)
        fds->max = new_fd;
}

void remove_fd(int fd, Fds* fds) {
    FD_CLR(fd, &fds->master);
    if(fd == fds->max)
        fds->max-- ;
}

int make_broadcast(struct sockaddr_in* addrOut, int port) {
    int socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd < 0) return socketfd;

    struct sockaddr_in addr;
    int broadcast = 1;
    int reuseport = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(IP);

    if (bind(socketfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        printf("error binindg\n");

    *addrOut = addr;
    return socketfd;
}



void broadcast(char message[], Socket udp) {
    sendto(udp.fd, message, strlen(message), 0, (struct sockaddr *)&udp.addr, sizeof(udp.addr));
}   

int connect_to_server(int server_tcp, int client_tcp) {
    struct sockaddr_in server, client;
    // client
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    client.sin_family = AF_INET; 
    client.sin_port = htons(client_tcp + 1); 
    client.sin_addr.s_addr = INADDR_ANY;
    bind(client_fd, (struct sockaddr *)&client, sizeof(client));

    // server
    server.sin_family = AF_INET; 
    server.sin_port = htons(server_tcp); 
    server.sin_addr.s_addr = INADDR_ANY;

    // connect client to server
    if (connect(client_fd, (struct sockaddr *)&server, sizeof(server)) < 0) { 
        printf(RED"Error in connecting to server"COL_RESET"\n");
        return -1;
    }
    return client_fd;
}

int make_server(int tcp_port, struct sockaddr_in* addrOut) {
    struct sockaddr_in address;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_port = htons(tcp_port);
    address.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 4);

    *addrOut = address;
    return server_fd;
}

int accept_client(int server_fd, int* client_port) {
    struct sockaddr_in client;
    socklen_t l = sizeof(client);
    int client_fd = accept(server_fd, (struct sockaddr *)&client, &l);
    if (client_port != NULL)
        *client_port = ntohs(client.sin_port) - 1;
    return client_fd;
}

int end_connection(int client_fd, Fds* fds) {
    close(client_fd);
    remove_fd(client_fd, fds);
}



void supplier_not_respond() {
    printf(PURPLE"supplier don't respond, your request is terminated!"COL_RESET"\n");

}
void restaurant_not_respond() {
    printf(PURPLE"restaurant don't respond, your request is terminated!"COL_RESET"\n");

}

int check_name(char name[], Socket tcp, Socket udp) {
    broadcast(encode(name, CHECK_NAME, tcp.port), udp);
    printf("Waiting to check your authentication...\n");
    char is_exist[2];
    while(1) {
        memset(is_exist, 0, 2);
        signal(SIGALRM, ignore);
        siginterrupt(SIGALRM, 1);
        alarm(1);
        int new_client = accept_client(tcp.fd, NULL);
        if(recv(new_client, is_exist, 2, 0) < 0){
            break;
        } 
        if(!strcmp(is_exist, "1"))
            return 0;
            
        alarm(0);
    }
    return 1;
}


void get_username(char* username[]) {
    char* buffer;
    buffer = (char*)malloc(100);
    memset(buffer, 0, 100);
    write(STDIN_FILENO, BLUE"Please enter your username: "COL_RESET, 28 + COL_LEN);
    read(STDIN_FILENO, buffer, 100);
    buffer[strlen(buffer) - 1] = 0;
    *username = buffer;
}
