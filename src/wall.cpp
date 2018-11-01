#include "wall.hpp"
#include "debug_renderer.hpp"
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>


void Wall::reset(uint32_t seed) {
    m_random.seed(seed);
    m_row_counter = 0;
    m_offset = 0;
    m_speed  = 0.25;
    m_cursor = { W * 0.5 - 0.5, 10 };
    m_radius = W * 0.4;
    m_island_delay = 80;

    for (auto& row : m_data) row.fill(1);
    for (auto& row : m_gen_data) row.fill(1);

    for (int i = 0; i < 36; ++i) generate();
}


void Wall::update() {

    m_offset += m_speed;
    while (m_offset >= 8) {
        m_offset -= 8;
        generate();
        // TODO: trigger enemy spawning
    }

}

void Wall::generate() {
    ++m_row_counter;

    m_gen_data.front().fill(1);
    std::rotate(m_gen_data.begin(), m_gen_data.begin() + 1, m_gen_data.end());

    constexpr int P = 4;

    m_cursor.y -= 1;
    while (m_cursor.y < 25) {

        // make hole
        constexpr float padding = 13;

        float r = m_radius * m_radius;
        for (int y = 0; y < (int) m_gen_data.size(); ++y)
        for (int x = P; x < W - P; ++x) {
            if (length2(vec2(x, y) - m_cursor) < r) {
                m_gen_data[y][x] = 0;
            }
        }

        // move cursor
        float ang = m_random.get_float(0.1, 0.6) * M_PI;
        if (m_random.get_int(0, 1) == 1) ang = -ang;
        m_cursor.y += std::cos(ang) * m_radius;
        m_cursor.x += std::sin(ang) * m_radius;
        m_cursor.x = clamp(m_cursor.x, padding, W - 1 - padding);
        m_radius = m_random.get_float(2.6, 10.8);

    }


    // create islands
    if (m_island_delay > 0) --m_island_delay;
    else {
        vec2 c = {
            m_random.get_float(P + 1, W - 2 - P),
            m_random.get_float(8.5, 10)
        };
        float r = m_random.get_float(1.9, 5.3);
        float r2 = r + 3.7;
        r2 *= r2;
        r *= r;

        // check for available space
        bool occupied = false;
        for (int y = 0; y < (int) m_gen_data.size(); ++y)
        for (int x = P - 1; x < W - P + 1; ++x) {
            vec2 p = { x, y };
            if (m_gen_data[y][x] == 1 && length2(p - c) < r2) {
                occupied = true;
                break;
            }
        }

        if (!occupied) {
            m_island_delay = m_random.get_int(2, 70);

            // write island
            for (int y = 0; y < (int) m_gen_data.size(); ++y)
            for (int x = P; x < W - P; ++x) {
                vec2 p = { x, y };
                if (length2(p - c) < r) {
                    m_gen_data[y][x] = 1;
                }
            }
        }
    }



    // translate row to data
    auto& row = m_data.front();
    for (int x = 0; x < W; ++x) {

        int n[] = {
            m_gen_data[0][x],
            x > 0 ? m_gen_data[1][x - 1] : 1,
            m_gen_data[2][x],
            x < W - 1 ? m_gen_data[1][x + 1] : 1,
        };

        int s = n[0] + n[1] + n[2] + n[3];
        row[x] = m_gen_data[1][x];
        if (row[x] == 0) {

            // fill small holes
            if (s >= 3) row[x] = 1;
            else if (s == 2) {
                // insert diagonals
                for (int i = 0; i < 4; ++i) {
                    if (n[i] + n[(i + 1) % 4] == 0) {
                        row[x] = i + 2;
                        break;
                    }
                }
            }
        }
        else if (s == 0) {
            int t = (x > 0     ? m_gen_data[1][x - 1] : 1)
                  + (x < W - 1 ? m_gen_data[1][x + 1] : 1)
                  + (x > 0     ? m_gen_data[3][x - 1] : 1)
                  + (x < W - 1 ? m_gen_data[3][x + 1] : 1);
            if (t == 0) row[x] = 0;
        }


        // speckles
        if (row[x] == 1 && s == 4) row[x] = m_random.get_int(57, 80);
    }
    std::rotate(m_data.begin(), m_data.begin() + 1, m_data.end());
}


vec2 Wall::get_tile_position(int x, int y) const {
    return {
        (x - W / 2.0 + 0.5) * 8,
        (m_data.size() / 2.0 - y) * 8 + m_offset,
    };
}

glm::ivec2 Wall::get_tile_address(vec2 const& pos) const {
    return {
        floor(pos.x / 8 + W / 2.0),
        floor((m_offset - pos.y) / 8 + m_data.size() / 2.0 + 0.5)
    };
}


void Wall::draw(SpriteRenderer& ren) {

    // DEBUG
    for (int y = 0; y < (int) m_gen_data.size(); ++y) {
        auto const& row = m_gen_data[y];
        ren.set_color({70, 70, 70});
        if (y == 1) ren.set_color({100, 70, 70});
        for (int x = 0; x < (int) row.size(); ++x) {
            if (row[x] == 0) continue;

            vec2 pos = {
                (x - row.size() / 2.0 + 0.5) * 8,
                (-16 - y) * 8 + m_offset,
            };

            ren.draw(frame(Sprite::WALLS, 0), pos);
        }
    }


    ren.set_color({84, 38, 89});


    for (int y = 0; y < (int) m_data.size(); ++y) {
        auto const& row = m_data[y];

        for (int x = 0; x < W; ++x) {
            int cell = row[x];
            if (cell == 0) continue;

            ren.push();
            ren.translate(get_tile_position(x, y));

            int n = cell - 1;

            if (x == 0 || x == W - 1 ||
                y == 0 || y == (int) m_data.size() - 1) // DEBUG
            {
                ren.draw(frame(Sprite::WALLS, n));
                ren.pop();
                continue;
            }

            auto const& prev = m_data[y - 1];
            auto const& next = m_data[y + 1];

            if (cell == 1) {
                n = 8;
                if (row[x - 1] == 0 && next[x] == 0) n += 1;
                if (row[x + 1] == 0 && next[x] == 0) n += 2;
                if (row[x - 1] == 0 && prev[x] == 0) n += 4;
                if (row[x + 1] == 0 && prev[x] == 0) n += 8;
            }

            ren.draw(frame(Sprite::WALLS, n));


            // smooth
            if (cell >= 6) {
                n = 24;
                if (row[x - 1] == 1 && next[x] == 1 && next[x - 1] < 6) n += 1;
                if (row[x + 1] == 1 && next[x] == 1 && next[x + 1] < 6) n += 2;
                if (row[x - 1] == 1 && prev[x] == 1 && prev[x - 1] < 6) n += 4;
                if (row[x + 1] == 1 && prev[x] == 1 && prev[x + 1] < 6) n += 8;
                ren.draw(frame(Sprite::WALLS, n));
            }
            else if (cell == 1) {
                n = 40;
                if (next[x] > 6 && row[x - 1] > 6) n += 1;
                if (next[x] > 6 && row[x + 1] > 6) n += 2;
                if (prev[x] > 6 && row[x - 1] > 6) n += 4;
                if (prev[x] > 6 && row[x + 1] > 6) n += 8;
                ren.draw(frame(Sprite::WALLS, n));
            }

            ren.pop();
        }

    }

    ren.set_color();
}


namespace {
    std::array<std::vector<vec2>, 5> WALL_POLYGONS = {
        std::vector<vec2>{ {-4, -4}, {-4, 4}, {4, 4}, {4, -4} },
        std::vector<vec2>{ {-4, -4}, {4, 4}, {4, -4} },
        std::vector<vec2>{ {-4, 4}, {4, 4}, {4, -4} },
        std::vector<vec2>{ {-4, -4}, {-4, 4}, {4, 4} },
        std::vector<vec2>{ {-4, -4}, {-4, 4}, {4, -4} },
    };
}

CollisionInfo Wall::check_collision(vec2 const* polygon, int len) const {
    CollisionInfo info = {};

    vec2 min = polygon[0];
    vec2 max = min;
    for (int i = 1; i < len; ++i) {
        min = glm::min(min, polygon[i]);
        max = glm::max(max, polygon[i]);
    }

    auto addr1 = get_tile_address(min);
    auto addr2 = get_tile_address(max);

    min = get_tile_position(addr1.x, addr1.y);
    max = get_tile_position(addr2.x, addr2.y);

//    DB_REN.set_color(200, 200, 0, 100);
//    DB_REN.rect(min - vec2(4, 4), max + vec2(4, 4));

    for (int y = addr2.y; y <= addr1.y; ++y) {
        if (y < 0 || y >= (int) m_data.size()) continue;
        for (int x = addr1.x; x <= addr2.x; ++x) {
            if (x < 0 || x >= W) continue;
            int cell = m_data[y][x];
            if (cell == 0) continue;


            int index = cell - 1;
            if (index < (int) WALL_POLYGONS.size()) {

                vec2 pos = get_tile_position(x, y);
                auto const& poly = WALL_POLYGONS[index];
                std::vector<vec2> ps(poly.size());
                for (int i = 0; i < (int) poly.size(); ++i) ps[i] = poly[i] + pos;

//                DB_REN.filled_polygon(ps);

                // XXX: we pass polys in reverse order because `info.where` is *better*
                //CollisionInfo ci = polygon_collision(polygon, len, ps.data(), ps.size());
                CollisionInfo ci = polygon_collision(ps.data(), ps.size(), polygon, len);
                ci.normal = -ci.normal;


                if (ci.distance > info.distance) info = ci;
            }
        }
    }

    return info;
}


bool Wall::check_sight(vec2 const& a, vec2 const& b) const {
    vec2 normal = glm::normalize(b - a);
    normal = vec2(normal.y, -normal.x);

    auto addr1 = get_tile_address(a);
    auto addr2 = get_tile_address(b);
    auto min = glm::min(addr1, addr2);
    auto max = glm::max(addr1, addr2);

    min = glm::max(min, {0, 0});
    max = glm::min(max, {W - 1, m_data.size() - 1});

	for (int y = min.y; y <= max.y; ++y) {
        for (int x = min.x; x <= max.x; ++x) {
            if (m_data[y][x] == 0) continue;
            float dist = glm::abs(glm::dot(normal, a - get_tile_position(x, y)));
            if (dist < 5) return true;
		}
	}
	return false;
}

