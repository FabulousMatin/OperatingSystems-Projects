#include "Resource.hpp"

using namespace std;

void log_event(string s) {
    int fd = open("logs.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    write(fd, s.c_str(), strlen(s.c_str()));
    close(fd);
}

int main(int argc, char* argv[]) {
    string building, month;
    cin >> building >> month;
    Gas* gas_counter = new Gas(building);
    gas_counter->report(stoi(month) - 1);

}