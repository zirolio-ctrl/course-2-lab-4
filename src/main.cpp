// Варіант 12. (C-23)

// m=3
// |   0    |    1   |    2    | string
// | 20% 5% | 20% 5% | 10% 20% | 20%

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <format>
#include <print>

#include "generate.hpp"
#include "Table.hpp"

const int FILES_PER_SCENARIO = 6;
const int REPEATS = 5;

int main(int argc, char** argv) {
    bool regen = (argc > 1 && std::string(argv[1]) == "regen");

    // Generate
    std::filesystem::create_directories("tests");
    for (int s = 0; s < 3; ++s) {
        for (int i = 0; i < FILES_PER_SCENARIO; ++i) {
            if (regen || !std::filesystem::exists(file_name(s, i))) generate(s, i);
        }
    }

    // Testing
    std::print("Тести розпочато...");

    Table resultsTable;
    resultsTable.add_column("Сценарій");
    resultsTable.add_column("Потоки");
    resultsTable.add_column("Запуск");
    resultsTable.add_column("Час");
    resultsTable.add_column("Контр. сума");
    
    double table[3][3];

    for (int s = 0; s < 3; ++s) {
        std::vector<std::vector<Op>> files;
        for (int i = 0; i < FILES_PER_SCENARIO; ++i) {
            files.push_back(load(file_name(s, i)));
        }

        for (int t = 1; t <= 3; ++t) {
            double sum = 0;
            uint64_t chk = 0;

            for (int r = 0; r < REPEATS; ++r) {
                double ms = run_scenario(t, FILES_PER_SCENARIO, files, chk);
                
                std::ostringstream time_ss;
                time_ss << std::fixed << std::setprecision(2) << ms;

                resultsTable.add_row(
                    SCEN_NAME[s],
                    t,
                    r + 1,
                    time_ss.str(),
                    (unsigned long long)chk
                );
                
                sum += ms;
            }

            table[s][t - 1] = sum / REPEATS;
        }
    }

    std::print("\n");
    resultsTable.print();

    // Summaries
    std::printf(
        "\nСередній час, мс (%d запусків, %ld операцій на сценарій):\n",
        REPEATS, OPS_PER_FILE * FILES_PER_SCENARIO
    );

    Table summariesTable;
    summariesTable.add_column("Сценарій");
    summariesTable.add_column("1 потік");
    summariesTable.add_column("2 потоки");
    summariesTable.add_column("3 потоки");

    for (int s = 0; s < 3; ++s) {
        summariesTable.add_row(
            SCEN_NAME[s],
            table[s][0], table[s][1], table[s][2]
        );
    }

    summariesTable.print();
    
    return 0;
}