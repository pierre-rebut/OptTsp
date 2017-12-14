//
// Created by rebut_p on 30/11/17.
//

#include "OptTsp.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <iostream>
#include <fstream>

size_t OptTsp::graphSize_s = 0;
vector<vector<int>> OptTsp::graph_s;

OptTsp::OptTsp(uint id) : id_m(id) {
    bestTour_m = vector<int>( OptTsp::graphSize_s + 1);
}

void OptTsp::exec(const std::time_t &timeLimit, long seed) {

    currentTour_m = vector<int>( OptTsp::graphSize_s + 1);
    for (size_t i = 0; i < OptTsp::graphSize_s; i++)
        currentTour_m[i] = i;
    currentTour_m[OptTsp::graphSize_s] = 0;


    while (std::time(nullptr) < timeLimit) {
        std::shuffle(currentTour_m.begin() + 1, currentTour_m.end() - 1, std::default_random_engine(seed));

        tourLength_t len = calcOpt();
        if (len < bestTourLength_m || bestTourLength_m == -1) {
            bestTourLength_m = len;
            for (size_t i = 0; i < OptTsp::graphSize_s; i++)
                bestTour_m[i] = currentTour_m[i];
        }
     }
}

tourLength_t OptTsp::calcOpt() {
    tourLength_t len = 0;

    for (size_t loc = 0; loc < OptTsp::graphSize_s; loc++) {

        int before_dist = -1;
        int res = 0;

        for (size_t loc2 = loc + 1; loc2 < OptTsp::graphSize_s; loc2++) {
            int after_dist = OptTsp::graph_s[ currentTour_m[loc] ][ currentTour_m[loc2] ];

            if (before_dist > after_dist || before_dist == -1) {
                res = (int)loc2;
                before_dist = after_dist;
            }
        }

        if (res != 0) {
            int tmp = currentTour_m[loc + 1];
            currentTour_m[loc + 1] = currentTour_m[res];
            currentTour_m[res] = tmp;
        }

        len += OptTsp::graph_s[ currentTour_m[loc] ][ currentTour_m[loc + 1] ];
    }

    return len;
}

/// Static function
void OptTsp::loadDataFromFile(const std::string &filename) {

    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("can't open file: " + filename);

    using std::string;

    string line;
    while( getline(file, line) ) {

        std::vector<int> city;
        size_t stTemp;

        while((stTemp = line.find(',')) != string::npos) {

            city.emplace_back( std::stoi(line.substr(0, stTemp)) );
            line = line.substr(stTemp + 1);
        }

        city.emplace_back(std::stoi(line));
        OptTsp::graph_s.push_back( std::move(city) );
    }

    OptTsp::graphSize_s = OptTsp::graph_s.size();
}