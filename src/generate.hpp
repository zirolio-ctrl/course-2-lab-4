#pragma once

#include <random>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "simulation.hpp"

const char* SCEN_NAME[3] = {"a (за умовою)", "b (рівні частоти)", "c (вручну, не за умовою)"};
const char  SCEN_ID[3] = {'a', 'b', 'c'};
const long OPS_PER_FILE = 1000000;

const double WEIGHTS[3][7] = {
    {20, 5, 20, 5, 10, 20, 20},
    {1, 1, 1, 1, 1, 1, 1},
    {30, 50, 2, 2, 3, 3, 10},
};

std::string file_name(int s, int i) {
    return std::string("tests/") + SCEN_ID[s] + "_" + std::to_string(i) + ".txt";
}

void generate(int s, int i) {
    std::mt19937 gen(1000u * (s + 1) + i);
    std::discrete_distribution<int> dist(WEIGHTS[s], WEIGHTS[s] + 7);
    
    std::ofstream out(file_name(s, i));

    for (long k = 0; k < OPS_PER_FILE; ++k) {
        int t = dist(gen);
        if (t == 6) out << "string\n";
        else if (t % 2 == 0) out << "read " << t / 2 << "\n";
        else out << "write " << t / 2 << " 1\n";
    }
}

std::vector<Op> load(const std::string& path) {
    std::vector<Op> ops;
    ops.reserve(OPS_PER_FILE);

    std::ifstream in(path);
    std::string cmd;
    
    while (in >> cmd) {
        if (cmd == "read")  { int f; in >> f; ops.push_back({READ, (uint8_t)f}); }
        else if (cmd == "write") { int f, v; in >> f >> v; ops.push_back({WRITE, (uint8_t)f}); }
        else ops.push_back({STR, 0});
    }

    return ops;
}