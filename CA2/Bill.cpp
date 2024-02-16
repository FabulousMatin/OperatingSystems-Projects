#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <vector>
#include <sstream>

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

#define BUF_SIZE 150

struct Bill {
    int normal_usage;
    int pick_usage;
    string type;
    string month;
    string name;

    int total_bill;
};


vector<string> make_fifos(int open_tasks) {
    string new_address, building, resource;
    vector<string> pipe_addresses;
    char buffer[BUF_SIZE];

    while(open_tasks > 0) {
        cin >> building >> resource;
        new_address = "./pipes/" + building + resource;
        
        if(mkfifo(new_address.c_str(), 0777) < 0) {
            cerr << "error making pipe\n";
            to_log(BILL, BILL, "error making pipe");
            exit(EXIT_FAILURE);
        }
        pipe_addresses.push_back(new_address);

        open_tasks--;
    }

    to_log(BILL, BILL, "created fifos");
    return pipe_addresses;
}

void input_majors(vector<string> fifo_adrs, vector<Bill>& bills) {
    
    for(auto const& new_fifo : fifo_adrs) {
        Bill new_bill;
        fstream fifo(new_fifo);

        fifo >> new_bill.name >> new_bill.type >> new_bill.month;
        fifo >> new_bill.normal_usage >> new_bill.pick_usage;
        
        fifo.clear();
        fifo.close();
        bills.push_back(new_bill);
    }

    to_log(BILL, BILL, "bill requests received");
    
}

void evaluate_bills(vector<Bill>& bills, vector<vector<string>> fees){
    for(auto& bill : bills) {
        bill.total_bill = 1.25 * bill.pick_usage + bill.normal_usage;
        bill.total_bill *= stoi(fees[stoi(bill.month)][_resource(bill.type)]); 
    }
}

void response(vector<Bill> bills) {
    string res;
    for(auto const& bill : bills) {
        res = bill.name + "|" + bill.type + "|bill|" + to_string(bill.total_bill);
        cout << res << '\n'; 
    }

    to_log(BILL, BILL, "response sent to main");
}

void delete_fifos(vector<string> names) {
    for(auto const& name : names) {
        unlink(name.c_str());
    }
}

int main() {
    vector<vector<string>> fees;
    vector<Bill> bills;
    read_csv("./buildings/bills.csv", fees);
    
    int open_tasks;
    cin >> open_tasks;
    
    vector<string> pipe_names = make_fifos(open_tasks);
    input_majors(pipe_names, bills);
    evaluate_bills(bills, fees);
    response(bills);
    delete_fifos(pipe_names);

    to_log(BILL, BILL, "work done - exiting");
}