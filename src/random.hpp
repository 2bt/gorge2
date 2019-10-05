#pragma once
#include <chrono>
#include <random>


class Random {
public:
    Random() {}
    explicit Random(uint32_t s) { seed(s); }
    void seed(uint32_t s = std::chrono::system_clock::now().time_since_epoch().count()) {
        m_gen.seed(s);
    }
    int get_int(int min, int max) {
        return m_int_dist(m_gen, decltype(m_int_dist)::param_type{ min, max });
    }
    float get_float(float min, float max) {
        return m_float_dist(m_gen, decltype(m_float_dist)::param_type{ min, max });
    }

    template<class Dist>
    auto get(Dist& dist) { return dist(m_gen); }

private:
    std::default_random_engine            m_gen;
    std::uniform_int_distribution<int>    m_int_dist;
    std::uniform_real_distribution<float> m_float_dist;
};

