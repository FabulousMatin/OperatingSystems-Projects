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
// building[name][resource][major] = value
typedef map<string,map<string,map<string,int>>> Building;


vector<string> get_names(Building buildings) {
    vector<string> names;
    for (const auto& building : buildings) {
        names.push_back(building.first);
    }
    return names;
}


int create_bill_handler(vector<string> buildings, int resources_key) {

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

        execl("./exec/Bill.out", "", nullptr);
        exit(0);
    } 
    
    else {
        close(ptoc[0]);
        close(ctop[1]);

        vector<string> resources = decode_resources(resources_key);
        string n_fifo = to_string(buildings.size() * resources.size());

        write(ptoc[1], n_fifo.c_str(), strlen(n_fifo.c_str()));
        write(ptoc[1], "\n", 1);

        for(auto const& building : buildings) {
            for(auto const& resource : resources) {
                write(ptoc[1], building.c_str(), strlen(building.c_str()));
                write(ptoc[1], "\n", 1);

                write(ptoc[1], resource.c_str(), strlen(resource.c_str()));
                write(ptoc[1], "\n", 1);
            }
        }

        close(ptoc[1]);
        to_log(MAIN, MAIN, "create bill handler");
        return ctop[0];
    }


}

int to_map(string building, int resources, int month) {
    
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

        execl("./exec/Building.out", "", nullptr);
        exit(0);
    } 
    else {    
        close(ptoc[0]);
        close(ctop[1]);

        write(ptoc[1], building.c_str(), strlen(building.c_str()));
        write(ptoc[1], "\n", 1);
        write(ptoc[1], to_string(resources).c_str(), strlen(to_string(resources).c_str()));
        write(ptoc[1], "\n", 1);
        write(ptoc[1], to_string(month).c_str(), strlen(to_string(month).c_str()));
        write(ptoc[1], "\n", 1);

        close(ptoc[1]);

        to_log(MAIN, MAIN, "create bulding " + building);
        return ctop[0];
    }

}

void decode_response(string res, 
string& name, string& resource, string& major, int& value) {

    string token;
    istringstream iss(res);

    getline(iss, token, '|');
    name = token;

    getline(iss, token, '|');
    resource = token;

    getline(iss, token, '|');
    major = token;
    
    getline(iss, token, '|');
    value = stoi(token);

}

void to_reduce(vector<int> children_fd, Building& buildings) {
    int BUF_SIZE = 1024;
    char buffer[BUF_SIZE];
    string name, resource, major, token;
    int value;
    for(auto const& fd : children_fd) {
        memset(buffer, 0, BUF_SIZE);
        read(fd, buffer, BUF_SIZE);
        close(fd);
        istringstream res(buffer);
        while (getline(res, token, '\n')) {
            decode_response(token, name, resource, major, value);
            buildings[name][resource][major] = value;
        }
    }
    to_log(MAIN, MAIN, "responses recieved");
}

int map_and_reduce(Building& buildings ,vector<string> names, int resources, int month) {

    int n_processes = names.size() * decode_resources(resources).size() + 1;
    vector<int> children_fd;

    children_fd.push_back(create_bill_handler(names, resources));
    for(auto const& building : names) {
        children_fd.push_back(to_map(building, resources, month));   
    }

    for(int i = 0; i < n_processes; i++) {
        wait(NULL);
    }

    to_reduce(children_fd, buildings);
    
    
    return 0;
}


void input(vector<string>& buildings, int& resources, int& month, vector<string>& features) {
    vector<string> all_buildings = read_directories("./buildings");
    cout << BLUE_COLOR + "Buildings:" + RESET_COLOR << endl;
    for(int i = 0; i < all_buildings.size(); i++) {
        cout << MAGNETA_COLOR << i + 1 << ". " <<  all_buildings[i] << RESET_COLOR << endl;
    }
    cout << BLUE_COLOR + "Enter buildings:" + RESET_COLOR << endl;
    string input, token;
    getline(cin, input);
    istringstream iss(input);
    while (getline(iss, token, ' ')) {
        buildings.push_back(token);
    }
    
    cout << BLUE_COLOR + "Enter resources:" + RESET_COLOR << endl;
    cout << MAGNETA_COLOR + "1.Gas\n2.Electricity\n3.Water" + RESET_COLOR << endl;
    getline(cin, input);
    iss.clear();
    iss.str(input);
    while (getline(iss, token, ' ')) {
        if(token == "Gas")
            resources += 1;
        else if(token == "Water")
            resources += 2;
        else if(token == "Electricity")
            resources += 4;
    }



    cout << BLUE_COLOR + "Enter features:" + RESET_COLOR << endl;
    for(int i = 0; i < MAJORS.size(); i++) {
        cout << MAGNETA_COLOR << i << ". " << MAJORS[i] << RESET_COLOR << endl;
    }
    getline(cin, input);
    iss.clear();
    iss.str(input);
    
    while (getline(iss, token, ' ')) {
        if(token == "5"){
            for(int i = 1; i <= 6; i++)
                features.push_back("hour " + to_string(i));
        }
        else
            features.push_back(MAJORS[stoi(token)]);
    
    }
    cout << BLUE_COLOR + "Enter month:" + RESET_COLOR << endl;
    cin >> month;
}

// building[name][resource][major] = value
void print_in_style0(Building buildings) {
    for(auto const& building : buildings) {
        for(auto const& resource : buildings[building.first]) {
            cout << building.first << " : "  << resource.first << '\n';
            for(auto const& major : buildings[building.first][resource.first]) {
                cout << major.first << " : " << major.second << "\n";
            }
        }
    }
}
bool is_in_features(string major, vector<string> features) {
    for(auto const& f : features) {
        if(f == major) {
            return true;
        }
    }
    return false;
}

void print_in_style1(Building buildings, vector<string> features) {

    string colorHeader = BLUE_COLOR;    
    string colorBuilding = GREEN_COLOR;  
    string colorResource = YELLOW_COLOR; 
    string colorMajor = MAGNETA_COLOR;     
    string colorReset = RESET_COLOR; 

    int dist = 35;
    
    cout << left << setw(dist) << colorHeader + "Building" + colorReset;
    cout << left << setw(dist) << colorHeader + "Resource" + colorReset;
    cout << left << setw(dist) << colorHeader + "Major" + colorReset;
    cout << left << colorHeader + "Value" + colorReset << endl;

    
    for (const auto& building : buildings) {
        for (const auto& resource : building.second) {
            for (const auto& major : resource.second) {
                if(!is_in_features(major.first, features))
                    continue;
                cout << left << setw(dist) << colorBuilding + building.first + colorReset;
                cout << left << setw(dist) << colorResource + resource.first + colorReset;
                cout << left << setw(dist) << colorMajor + major.first + colorReset;
                cout << left << major.second << endl;
            }
            cout << "_______________________________________________________________________________________\n\n";
        }
    }
}
void print_in_style2(Building buildings) {
    int dist = 35;
    string colorHeader = BLUE_COLOR;    
    string colorBuilding = GREEN_COLOR;  
    string colorResource = YELLOW_COLOR; 
    string colorMajor = MAGNETA_COLOR;     
    string colorReset = RESET_COLOR;        
  
    string horizontalLine(85, '-');

    cout << horizontalLine << endl;
    cout << "| " << left << setw(dist) << colorHeader + "Building" + colorReset;
    cout << "| " << left << setw(dist) << colorHeader + "Resource" + colorReset;
    cout << "| " << left << setw(dist) << colorHeader + "Major" + colorReset;
    cout << "| " << left << colorHeader + "Value" + colorReset << " |" << endl;
    cout << horizontalLine << endl;

    for (const auto& building : buildings) {
        for (const auto& resource : building.second) {
            for (const auto& major : resource.second) {
                cout << "| " << left << setw(dist) << colorBuilding + building.first + colorReset;
                cout << "| " << left << setw(dist) << colorResource + resource.first + colorReset;
                cout << "| " << left << setw(dist) << colorMajor + major.first + colorReset;
                cout << "| " << left << major.second << " |" << endl;
            }
        }
    }

    cout << horizontalLine << endl;
}

void print_log(const string& line) {
    string result;
    string::size_type pos = 0;
    string::size_type startPos = 0;

    while ((pos = line.find("[", startPos)) != string::npos) {
        string::size_type endPos = line.find("]", pos);
        if (endPos == string::npos)
            break;

        result += line.substr(startPos, pos - startPos); 
        string bracketContent = line.substr(pos + 1, endPos - pos - 1);
        string color;
        if (bracketContent == "main")
            color = YELLOW_COLOR;
        else if (bracketContent == "bill_handler")
            color = BLUE_COLOR;
        else if (bracketContent == "building")
            color = GREEN_COLOR;
        else
            color = GREEN_COLOR;

        result += color + "[" + bracketContent + "]" + RESET_COLOR; 

        startPos = endPos + 1;
    }

    result += line.substr(startPos); 

    cout << result << endl;
}

void show_log() {
    dup2(0,0);
    string res;
    cout << BLUE_COLOR + "print logs?" << endl;
    cout << GREEN_COLOR  + "yes" + RESET_COLOR + "/" + RED_COLOR + "no" + RESET_COLOR << endl;
    cin >> res;
    if(res == "yes") {
        ifstream file("logs.txt");
        string line;

        if (file.is_open()) {
            while (getline(file, line)) {
                print_log(line);
            }
            file.close();
        } 
        else 
            cout << "Unable to open file." << endl;
        
    }
}
int main() {
    
    Building buildings;
    vector<string> building_names;
    vector<string> features;
    int resources = 0, month;

    input(building_names, resources, month, features);
    map_and_reduce(buildings, building_names, resources, month);
    print_in_style1(buildings, features);
    to_log(MAIN, MAIN, "work done - exiting");
    show_log();

    return (0);
}