#include "Resource.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    string building, month;
    cin >> building >> month;
    Electricity* electricity_counter = new Electricity(building);

    electricity_counter->report(stoi(month) - 1);

}