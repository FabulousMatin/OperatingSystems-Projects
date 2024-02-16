#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <vector>
#include <sstream>
#include <bits/stdc++.h>

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

#include "utils.hpp"

using namespace std;

// report[resource][variable] = value
typedef map<string,map<string,int>> Report;


void request_bill(string name, string type, string month, int normal_usage, int pick_usage) {
    string pipe_address = "./pipes/" + name + type;;

        
    int fd = open(pipe_address.c_str(), O_WRONLY);
    ostringstream fifo;
    fifo << name << "\n" << type << "\n" << month << "\n";
    fifo << normal_usage << "\n" << pick_usage << "\n";    

    write(fd, fifo.str().c_str(), strlen(fifo.str().c_str()));
    close(fd);
    to_log(BUILDING, name, "bill requested for " + type);
}

int to_map(string building, string resource, string month) {
    string process = "./exec/" + resource + ".out";
    int ptoc[2]; // parent to child 
    int ctop[2]; // child to parent
    if (pipe(ptoc) == -1 || pipe(ctop) == -1) {
        cerr << "ERROR creating a pipe\n";
        exit(1);
    }
    if (!fork()) {

        dup2(ptoc[0], 0);
        close(ptoc[0]);
        close(ptoc[1]);
        
        dup2(ctop[1], 1);
        close(ctop[0]);
        close(ctop[1]);

        execl(process.c_str(), "", nullptr);
        exit(0);
    } 
    else {
        close(ptoc[0]);
        close(ctop[1]);
    
        write(ptoc[1], building.c_str(), strlen(building.c_str()));
        write(ptoc[1], "\n", 1);
        write(ptoc[1], month.c_str(), strlen(month.c_str()));
        write(ptoc[1], "\n", 1);
        close(ptoc[1]);

        to_log(BUILDING, building, "report requested for " + resource);
    }

    return ctop[0];
}


string decode_response(string response, Report& reports) {
    string token, resource;
    istringstream iss(response);

    getline(iss, token, '|');
    resource = token;

    getline(iss, token, '|');
    reports[resource]["total_consumptions"] = stoi(token);

    getline(iss, token, '|');
    reports[resource]["mean_consumption"] = stoi(token);
    
    getline(iss, token, '|');
    reports[resource]["pick_hour"] = stoi(token);

    getline(iss, token, '|');
    reports[resource]["pick_consumption"] = stoi(token);

    getline(iss, token, '|');
    reports[resource]["normal_consumption"] = stoi(token);

    for(int hour = 0; hour < 6; hour++) {
        getline(iss, token, '|');
        reports[resource]["hour " + to_string(hour + 1)] = stoi(token);
    }
    return resource;
}

void to_reduce(vector<int> children_fd, Report& reports, string building) {
    string resource;
    for(auto const& fd : children_fd) {
        char response[150];
        read(fd, response, 150);
        resource = decode_response(response, reports);

        to_log(BUILDING, building, "responce received from " + resource);
    }
    
}

void map_and_reduce(Report& reports, string building, vector<string> resources_name, string month) {
    
    int n_processes = resources_name.size();

    vector<int> children_fd;
    for(auto const& resource : resources_name) {
        children_fd.push_back(to_map(building, resource, month));
    }

    for(int i = 0; i < n_processes; i++){
        wait(NULL);
    }

    
    to_reduce(children_fd, reports, building);

    for(auto const& resource : reports) {
        request_bill(
            building,
            resource.first, 
            month, 
            reports[resource.first]["normal_consumption"],
            reports[resource.first]["pick_consumption"]
        );
    }
}

void response(Report reports, string building) {
    string res;
    for(auto const& resource : reports) {
        for(auto const& variable : resource.second) {
            res = building + "|" + resource.first + "|" + variable.first + "|" + to_string(variable.second);
            cout << res << '\n';
        }
    }

    to_log(BUILDING, building, "response sent to main");
}

int main() {
    string building, resources_key, month;
    cin >> building >> resources_key >> month;

    vector<string> resources_name = decode_resources(stoi(resources_key));
    Report reports; 
    map_and_reduce(reports, building, resources_name, month);
    response(reports, building);
    
    to_log(BUILDING, building, "work done - exiting");

    return 0;
}