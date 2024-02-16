#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "utils.hpp"
#include <string.h>

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>

class Resource {

protected:
    std::vector<std::vector<std::string>> data;
    std::string path;
    std::string type;

public:
    Resource(std::string building);
    std::vector<int> hourly_consumption(int month);
    std::vector<int> pick_diff(int month);
    float mean_consumption(int month);
    int pick_hour(int month);
    int total_consumptions(int month);
    int pick_consumption(int month);
    int normal_consumption(int month);
    void report(int month);
};

class Water : public Resource {
public:
    Water(std::string building) : Resource(building) {
        type = "Water";
        path = path + "/Water.csv";
        read_csv(path, data);
    }
};

class Gas : public Resource {
public:
    Gas(std::string building) : Resource(building) {
        type = "Gas";
        path = path + "/Gas.csv";
        read_csv(path, data);
    }
};

class Electricity : public Resource {
public:
    Electricity(std::string building) : Resource(building) {
        type = "Electricity";
        path = path + "/Electricity.csv";
        read_csv(path, data);
    }
};


#endif