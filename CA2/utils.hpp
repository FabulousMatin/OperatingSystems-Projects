#ifndef _CSV_H_
#define _CSV_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>
#include <dirent.h>

#define YEAR 0
#define MONTH 1
#define DAY 2

#define WATER 2
#define GAS 3
#define ELECTRICITY 4

#define G "Gas"
#define W "Water"
#define E "Electricity"

#define MAIN "main"
#define BUILDING "building"
#define BILL "bill_handler"
#define DEFAULT "default"

const std::string RESET_COLOR = "\033[0m";
const std::string RED_COLOR = "\033[31m";
const std::string GREEN_COLOR = "\033[32m";
const std::string YELLOW_COLOR = "\033[33m";
const std::string BLUE_COLOR = "\033[34m";
const std::string MAGNETA_COLOR = "\033[1;35m";

extern std::vector<std::string> MAJORS;

#define _resource(x) (x == W ? WATER : (x == G ? GAS : ELECTRICITY))

#define _hour(x) (x + 3)
#define _get_index(month, day) (30 * month + day + 1)

int read_csv(std::string file_path, std::vector<std::vector<std::string>>& data);
std::vector<std::string> read_directories(std::string path);
void to_log(std::string type, std::string name, std::string log);
std::vector<std::string> decode_resources(int code);



#endif