#pragma once

#include <chrono>
#include <filesystem>
#include <vector>
#include <thread>

#include "Struct.hpp"

enum Kind : uint8_t { READ = 0, WRITE = 1, STR = 2 };
struct Op { uint8_t kind; uint8_t field; };

uint64_t run_ops(Struct& d, const std::vector<Op>& ops) {
    uint64_t sink = 0;

    for (const Op& op : ops) {
        switch (op.kind) {
            case READ:
                sink += (op.field == 0) ? d.get0() : (op.field == 1) ? d.get1() : d.get2();
                break;
            case WRITE:
                op.field == 0 ? d.set0(1) : op.field == 1 ? d.set1(1) : d.set2(1);
                break;
            default: {
                std::string s = d;
                sink += s.size();
            }
        }
    }

    return sink;
}

double run_scenario(int threads, int files_per_scenario, const std::vector<std::vector<Op>>& files, uint64_t& check) {
    Struct d;
    
    std::vector<uint64_t> sinks(threads, 0);
    std::vector<std::thread> th;

    auto t0 = std::chrono::steady_clock::now();
    
    for (int t = 0; t < threads; ++t) {
        th.emplace_back([&, t] {
            uint64_t s = 0;
            for (int i = t; i < files_per_scenario; i += threads) s += run_ops(d, files[i]);
            sinks[t] = s;
        });
    }

    for (auto& x : th) x.join();
    auto t1 = std::chrono::steady_clock::now();
    
    check = 0;
    for (auto s : sinks) check += s;

    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}