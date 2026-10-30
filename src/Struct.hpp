#pragma once

#include <mutex>
#include <shared_mutex>
#include <string>

class Struct {
public:
    int get0() const { std::shared_lock<std::shared_mutex> l(m0_); return v0_; }
    int get1() const { std::shared_lock<std::shared_mutex> l(m1_); return v1_; }
    int get2() const { std::lock_guard<std::mutex> l(m2_); return v2_; }

    void set0(int x) { std::unique_lock<std::shared_mutex> l(m0_); v0_ = x; }
    void set1(int x) { std::unique_lock<std::shared_mutex> l(m1_); v1_ = x; }
    void set2(int x) { std::lock_guard<std::mutex> l(m2_); v2_ = x; }

    operator std::string() const {
        int a, b, c;

        {
            std::shared_lock<std::shared_mutex> l0(m0_);
            std::shared_lock<std::shared_mutex> l1(m1_);
            std::lock_guard<std::mutex> l2(m2_);
            a = v0_; b = v1_; c = v2_;
        }

        return "(" + std::to_string(a) + ", " + std::to_string(b) + ", " + std::to_string(c) + ")";
    }

private:
    alignas(64) mutable std::shared_mutex m0_; int v0_ = 0;
    alignas(64) mutable std::shared_mutex m1_; int v1_ = 0;
    alignas(64) mutable std::mutex m2_;        int v2_ = 0;
};