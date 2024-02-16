#include "utils.hpp"
using namespace std;


std::vector<std::string> MAJORS = {
    "total_consumptions",
    "mean_consumption",
    "pick_hour",
    "pick_consumption",
    "normal_consumption",
    "pick_difference"
};

void to_log(string type, string name, string log) {
    ofstream file("./logs.txt", ios_base::app);
    file << "[" << type << "]";
    file << "[" << name << "]";
    file << " : " << log << "\n";
    file.close();
}

vector<string> decode_resources(int code) {
    if(code == 1)
        return(vector<string>{"Gas"});
    else if (code == 2)
        return(vector<string>{"Water"});
    else if (code == 4)
        return(vector<string>{"Electricity"});
    else if (code == 1 + 2)
        return(vector<string>{"Gas", "Water"});
    else if (code == 1 + 4)
        return(vector<string>{"Electricity", "Gas"});
    else if (code == 2 + 4)
        return(vector<string>{"Electricity", "Water"});
    else if (code == 1 + 2 + 4)
        return(vector<string>{"Electricity", "Gas", "Water"});
    return(vector<string>{});
}

vector<string> read_directories(string path) {
    string folderPath = path; 
    DIR* directory = opendir(folderPath.c_str());

    if (directory == nullptr) {
        cerr << "Failed to open directory." << endl;
        throw("Failed to open directory.");
    }   

    vector<string> directories;

    dirent* entry;
    while ((entry = readdir(directory)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            string name = entry->d_name;
            if (name != "." && name != "..") {
                directories.push_back(name);
            }
        }
    }

    closedir(directory);

    return directories;
}

int read_csv(string file_path, vector<vector<string>>& data) {
    ifstream file(file_path);

    if (!file) {
        cerr << "Failed to open the file." << endl;
        return -1;
    }

    string line;

    while (getline(file, line)) {
        vector<string> row;
        stringstream ss(line);
        string cell;

        while (getline(ss, cell, ',')) { 
            row.push_back(cell);
        }

        data.push_back(row);
    }

    file.close();
    return 0;
}


