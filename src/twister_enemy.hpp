#pragma once
#include "enemy.hpp"
#include "world.hpp"


class TwisterEnemy;


class TwisterPath {
public:
    using Ptr = std::shared_ptr<TwisterPath>;

    struct Element {
        float ang;
        int   ticks;
    };

    std::vector<Element> elements;

    void add_twister(TwisterEnemy const* t) {
        m_twisters.push_back(t);
    }
    void remove_twister(TwisterEnemy const* t) {
        m_twisters.push_back(t);
        m_twisters.erase(std::remove(m_twisters.begin(), m_twisters.end(), t), m_twisters.end());
    }
    bool is_leader(TwisterEnemy const* t) const {
        assert(!m_twisters.empty());
        return m_twisters.front() == t;
    }


private:
    std::vector<TwisterEnemy const*> m_twisters;
};


class TwisterEnemyChain : public Particle {
public:
    TwisterEnemyChain(World& world, vec2 const& pos);
    bool update() override;
    void draw(SpriteRenderer& ren) const override {}
private:
    World&   m_world;
    int      m_tick = 0;
    TwisterPath::Ptr m_path;
};

class TwisterEnemy : public Enemy {
public:
    TwisterEnemy(World& world, uint32_t seed, vec2 const& pos, TwisterPath::Ptr path);
    void sub_update() override;
    void die() override;
protected:
    float            m_dst_ang;
    int              m_count = 0;
    int              m_index = 0;
    TwisterPath::Ptr m_path;
};
