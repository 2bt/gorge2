#include "player.hpp"
#include "sprite.hpp"
#include "debug_renderer.hpp"
#include "world.hpp"


namespace {
    constexpr std::array<vec2, 6> PLAYER_POLYGON = {
        vec2{4, 4},
        vec2{4, 0},
        vec2{1, -3},
        vec2{-1, -3},
        vec2{-4, 0},
        vec2{-4, 4},
    };

    constexpr std::array<vec2, 8> FIELD_POLYGON = {
        vec2{4, 6},
        vec2{6, 4},
        vec2{6, 0},
        vec2{1, -5},
        vec2{-1, -5},
        vec2{-6, 0},
        vec2{-6, 4},
        vec2{-4, 6},
    };

    constexpr std::array<vec2, 4> LASER_POLYGON = {
        vec2{-0.5, 2.5},
        vec2{0.5, 2.5},
        vec2{0.5, -2.5},
        vec2{-0.5, -2.5},
    };

};


void Player::reset() {
    m_tick        = 0;
    m_shild       = 3;
    m_pos         = { 0, 40 };
    m_blast_vel   = {};
    m_blast_delay = 0;
    m_shoot_delay = 0;
    m_side_shot   = false;
}


void Player::hit(CollisionInfo const& info) {

    // blast
    if (info.distance > 0) {
        m_pos += info.normal * info.distance;
        transform(m_polygon, PLAYER_POLYGON, m_pos);

        m_blast_vel = info.normal * 1.5f;
        m_blast_delay = 15;

    }

    // damage
}


void Player::update(Input const& input) {

    ++m_tick;
    m_blast_vel *= 0.87f;

    float speed = 0;
    if (m_blast_delay > 0) {
        --m_blast_delay;
    }
    else {
        speed = 0.75;
        if (input.a || m_shoot_delay > 0) speed *= 0.5;
    }

    // move
    m_pos += m_blast_vel + vec2(input.dx, input.dy) * speed;
    m_pos = clamp(m_pos, { -124, -72 }, { 124, 71 });

    // collision with wall
    transform(m_polygon, PLAYER_POLYGON, m_pos);
    CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
    if (info.distance > 0) hit(info);


    // shoot
    if (input.dy > 0) m_side_shot = false;
    if (input.dy < 0) m_side_shot = true;
    if (m_shoot_delay > 0) --m_shoot_delay;
    if (input.a && m_shoot_delay == 0 && m_blast_delay == 0) {
        m_shoot_delay = 10;

        m_world.make_laser(m_pos - vec2(0, 1), {0, -1});
    }
//    if input.a and self.shoot_delay == 0 and self.blast == 0 then
//        sound.play("laser", self.x, self.y)
//        self.shoot_delay = 10
//        Laser(self.x, self.y - 4)
//
//        self.balls[1]:shoot(self.side_shoot)
//        self.balls[2]:shoot(self.side_shoot)
//
//    end
//    if self.shoot_delay > 0 then
//        self.shoot_delay = self.shoot_delay - 1
//    end

}


void Player::draw(SpriteRenderer& ren) const {
    ren.draw(frame(Sprite::PLAYER, m_tick / 8 % 2), m_pos);

    // debug
//    DB_REN.set_color(255, 0, 0);
//    DB_REN.polygon(m_polygon);
}


Laser::Laser(World const& world, vec2 const& pos, vec2 const& vel)
    : m_world(world), m_pos(pos), m_vel(vel)
{
    m_ang = std::atan2(m_vel.x, m_vel.y);
}

bool Laser::update() {
    for (int i = 0; i < 4; ++i) {
        m_pos += m_vel;
        if (std::abs(m_pos.x) > 124 || std::abs(m_pos.y) > 80) {
            return false;
        }

        transform(m_polygon, LASER_POLYGON, m_pos, m_ang);
        CollisionInfo info = m_world.get_wall().check_collision(m_polygon);
        if (info.distance > 0) {

            return false;
        }

    }

    return true;
}

void Laser::draw(SpriteRenderer& ren) const {
    ren.push();
    ren.translate(m_pos);
    ren.rotate(m_ang);
    ren.draw(frame(Sprite::LASER));
    ren.pop();

    // debug
//    DB_REN.set_color(255, 0, 0);
//    DB_REN.polygon(m_polygon);
}


//Laser = Object:New {
//    quad_generator = QuadGenerator(200),
//    list = {},
//    model = { -2, 10, 2, 10, 2, -10, -2, -10, },
//    damage = 1
//}
//Laser:InitQuads("media/laser.png")
//initPolygonRadius(Laser.model)
//function Laser:init(x, y, vx, vy)
//    table.insert(self.list, self)
//    self.x = x
//    self.y = y
//    self.vx = vx or 0
//    self.vy = vy or -4
//    self.ang = math.atan2(self.vx, self.vy)
//    self.trans_model = {}
//end
//function Laser:update()
//    for i = 1, 4 do
//        self.x = self.x + self.vx
//        self.y = self.y + self.vy
//        if self.y < -310 or self.y > 310
//        or self.x < -410 or self.x > 410 then return "kill" end
//        transform(self)
//
//        local d, n, w = game.walls:checkCollision(self.trans_model, true)
//        if d > 0 then
//            for i = 1, 10 do
//                sound.play("miss", w[1], w[2])
//                LaserParticle(w[1], w[2])
//            end
//            return "kill"
//        end
//
//        for _, e in ipairs(Enemy.list) do
//            local d, n, w = polygonCollision(e.trans_model, self.trans_model)
//            if d > 0 then
//                e:hit(self.damage)
//                for i = 1, 10 do
//                    LaserParticle(w[1], w[2])
//                end
//                return "kill"
//            end
//
//        end
//    end
//    if self.ttl then
//        self.ttl = self.ttl - 1
//        if self.ttl <= 0 then return "kill" end
//    end
//end
//function Laser:draw()
//    self.quads.batch:add(self.quads[1], self.x, self.y, -self.ang, 2, 2, 6, 6)
//end
//SmallLaser = Laser:New {
//    model = { -2, 5, 2, 5, 2, -5, -2, -5, },
//    damage = 0.5
//}
//SmallLaser:InitQuads("media/small_laser.png")
//initPolygonRadius(SmallLaser.model)
//function Laser:draw()
//    self.quads.batch:add(self.quads[1], self.x, self.y, -self.ang, 2, 2, 6, 6)
//end

