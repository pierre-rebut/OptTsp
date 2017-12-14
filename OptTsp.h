//
// Created by rebut_p on 30/11/17.
//

#ifndef CPP_OPTTSP_H
#define CPP_OPTTSP_H

#include <vector>
#include <memory>
#include <ctime>

using std::vector;
typedef long long int tourLength_t;
typedef unsigned int uint;

class OptTsp {

public:
    static size_t graphSize_s;
    static vector<vector<int>> graph_s;

    static void loadDataFromFile(const std::string &filename);

private:
    uint id_m;
    vector<int> currentTour_m;

    tourLength_t bestTourLength_m = -1;
    vector<int> bestTour_m;

    tourLength_t calcOpt();

public:
    explicit OptTsp(uint id = 0);

    void exec(const std::time_t &timeLimit, long seed);


    /**
     * GETTER
     */
    inline tourLength_t getBestTourLength() const {
        return bestTourLength_m;
    }

    inline const vector<int>& getBestTour() const {
        return bestTour_m;
    }
};


#endif //CPP_OPTTSP_H
