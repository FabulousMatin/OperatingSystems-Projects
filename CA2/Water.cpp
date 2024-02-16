#include "Resource.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    string building, month;
    cin >> building >> month;
    Water* water_counter = new Water(building);

    water_counter->report(stoi(month) - 1);

}