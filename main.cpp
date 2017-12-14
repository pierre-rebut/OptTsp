#include "OptTsp.h"
#include "Filetage.h"

#include <iostream>
#include <fstream>

void saveResultInFile(const std::string& filename, const std::vector<int> &result) {
    std::ofstream output(filename);
    if (!output.is_open())
        throw std::runtime_error("can't open output file : " + filename);

    for (const int city : result) {
        output << (city + 1) << std::endl;
    }
}

#define TOTAL_T 5

int main(int ac, char **av) {

    try {
        if (ac != 4)
            throw std::runtime_error("./OptTsp <input> <output> <duration>");

        std::time_t timeStart = std::time(nullptr);
        std::time_t timeLimit = std::stoi(av[3]) + timeStart;

        OptTsp::loadDataFromFile(av[1]);
		
		OptTsp tsp[TOTAL_T];
		Filetage result[TOTAL_T];

        for (size_t i = 0; i < TOTAL_T; i++) {
            result[i] = Filetage([](OptTsp *tsp, std::time_t lim, long seed) {
				tsp->exec(lim, seed);
			}, &tsp[i], timeLimit, std::time(nullptr) * i + i);
        }

		size_t id = 0;
		tourLength_t bestLength = -1;
        for (size_t i = 0; i < TOTAL_T; i++) {
            result[i].join();
            if (bestLength > tsp[i].getBestTourLength() || bestLength == -1) {
                bestLength = tsp[i].getBestTourLength();
                id = i;
            }
        }

        std::cout << "Best: " << bestLength << std::endl;
        saveResultInFile(av[2], tsp[id].getBestTour());
        std::cout << "Duration : " << (std::time(nullptr) - timeStart) << std::endl;

    } catch(std::exception &err) {
        std::clog << "[ERROR] " << err.what() << std::endl;
    }

    return 0;
}