#include "Resource.hpp"

using namespace std;

Resource::Resource(string building) {
    path = "./buildings/" + building;
}



vector<int> Resource::hourly_consumption(int month) {
    vector<int> hourly;
    int consume;
    for(int hour = 0; hour < 6; hour++) {
        consume = 0;
        for(int day = 0; day < 30; day++) {
            consume += stoi(data[_get_index(month, day)][_hour(hour)]);
        }
        hourly.push_back(consume);
    }
    return hourly;
}

float Resource::mean_consumption(int month) {
    int consumptions = this->total_consumptions(month);
    
    return consumptions / 180;
}

int Resource::pick_hour(int month) {

    int pick, max_consume, consume;
    
    max_consume = -1;
    for(int hour = 0; hour < 6; hour++) {
        consume = 0;
        for(int day = 0; day < 30; day++) {
            consume += stoi(data[_get_index(month, day)][_hour(hour)]);
        }
        if(consume > max_consume) {
            pick = hour + 1;
            max_consume = consume;
        }
    }
    return pick;
}

int Resource::total_consumptions(int month) {
    vector<int> consumptions = this->hourly_consumption(month);
    int sum = 0;
    for(auto const& c : consumptions) {
        sum += c;
    }
    return sum;
}

int Resource::pick_consumption(int month) {
    int pick = this->pick_hour(month) - 1;
    int consume = 0;
    for(int day = 0; day < 30; day++) {
        consume += stoi(data[_get_index(month, day)][_hour(pick)]);
    }
    return consume;
}

int Resource::normal_consumption(int month) {
    return (this->total_consumptions(month) - this->pick_consumption(month));
}

vector<int> Resource::pick_diff(int month) {
    vector<int> hourly = this->hourly_consumption(month);
    int pick = this->pick_hour(month) - 1;
    vector<int> diff;

    for(auto const& h : hourly) {
        diff.push_back(h - hourly[pick]);
    }

    return diff;
}

void Resource::report(int month) {
    string response = "";
    string delimeter = "|";

    response += this->type;
    response += delimeter;
    response += to_string(this->total_consumptions(month));
    response += delimeter;
    response += to_string(this->mean_consumption(month));
    response += delimeter;
    response += to_string(this->pick_hour(month));
    response += delimeter;
    response += to_string(this->pick_consumption(month));
    response += delimeter;
    response += to_string(this->normal_consumption(month));
    response += delimeter;

    for(auto const& hourly : this->pick_diff(month)) {
        response += to_string(hourly);
        response += delimeter;
    }
    
    response.pop_back();
    cout << response;

}

