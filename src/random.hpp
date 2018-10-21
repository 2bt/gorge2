#pragma once
#include <chrono>
#include <random>


class Random {
public:
    void seed(uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count()) {
        m_gen.seed(seed);
    }
    int get_int(int min, int max) {
        return m_int_dist(m_gen, decltype(m_int_dist)::param_type{ min, max });
    }
    float get_float(float min, float max) {
        return m_float_dist(m_gen, decltype(m_float_dist)::param_type{ min, max });
    }
private:
    std::default_random_engine            m_gen;
    std::uniform_int_distribution<int>    m_int_dist;
    std::uniform_real_distribution<float> m_float_dist;
};

