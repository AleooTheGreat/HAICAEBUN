#include "../header/Skelet.h"
#include "../header/GameExceptions.h"
#include <utility>
#include <valarray>
#include <iostream>
#include <SFML/Window/Keyboard.hpp>


Skelet::Skelet(int hp, unsigned short int dmg, float speed, std::string texturePath)
        : s_hp{hp}, s_dmg{dmg}, s_speed{speed}, skelet_texturePath{std::move(texturePath)},
          target{100, 100}, npcInitialPosition({0, 0}), state{CHASING_NPC} {
    try {
        if (!skelet_texture.loadFromFile(skelet_texturePath)) {
            throw TextureLoadException("Failed to load Skelet texture: " + skelet_texturePath);
        }
    } catch (const TextureLoadException& e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
    skelet_sprite.setTexture(skelet_texture);
    skelet_sprite.setPosition(600, 600);
}

Skelet::Skelet(const Skelet& other)
        : s_hp{other.s_hp}, s_dmg{other.s_dmg}, s_speed{other.s_speed},
          skelet_texturePath{other.skelet_texturePath}, target{other.target},
          npcInitialPosition{other.npcInitialPosition}, state{other.state} {
    try {
        if (!skelet_texture.loadFromFile(skelet_texturePath)) {
            throw TextureLoadException("Failed to load Skelet texture: " + skelet_texturePath);
        }
    } catch (const TextureLoadException& e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
    skelet_sprite.setTexture(skelet_texture);
    skelet_sprite.setPosition(other.skelet_sprite.getPosition());
}

Skelet& Skelet::operator=(Skelet other) {
    swap(*this, other);
    return *this;
}

void swap(Skelet& first, Skelet& second) noexcept {
    using std::swap;
    swap(first.s_hp, second.s_hp);
    swap(first.s_dmg, second.s_dmg);
    swap(first.s_speed, second.s_speed);
    swap(first.skelet_texturePath, second.skelet_texturePath);
    swap(first.skelet_sprite, second.skelet_sprite);
    swap(first.skelet_texture, second.skelet_texture);
    swap(first.target, second.target);
    swap(first.npcInitialPosition, second.npcInitialPosition);
    swap(first.state, second.state);
    swap(first.attackTimer, second.attackTimer);
    swap(first.getAttacked, second.getAttacked);
}

void Skelet::update(Player& p, NPC& npc) {
    switch (state) {
        case CHASING_NPC:
            target = npc.getPosition();
            if (player_collision(npc)) {
                npcInitialPosition = npc.getPosition();
                state = ATTACKING_PLAYER;
            }
            break;

        case ATTACKING_PLAYER:
            target = p.getPosition();
            if (player_collision(p)) {
                state = RETURNING_TO_NPC;
            }
            break;

        case RETURNING_TO_NPC:
            target = npcInitialPosition;
            if (player_collision(npc)) {
                state = CHASING_NPC;
            }
            break;
    }

    Position currentPosition = {skelet_sprite.getPosition().x, skelet_sprite.getPosition().y};
    Position direction = {target.x - currentPosition.x, target.y - currentPosition.y};
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0) {
        direction.x /= length;
        direction.y /= length;
    }

    skelet_sprite.move(direction.x * s_speed, direction.y * s_speed);

    if (player_collision(npc)) {
        if (attackTimer.getElapsedTime().asMilliseconds() >= 150) {
            npc.loseHp(s_dmg);
            std::cout << "NPC HP: " << npc.getHp() << '\n';
            attackTimer.restart();
        }
    }

    if (player_collision(p)) {
        if(attackTimer.getElapsedTime().asMilliseconds() >= 750) {
            p.loseHp(s_dmg);
            std::cout << "Player HP: " << p.getHp() << '\n';
            attackTimer.restart();
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            if(getAttacked.getElapsedTime().asMilliseconds() < 20) {
                std::cout << "hit" << '\n';
                s_hp -= static_cast<int>(std::floor(p.get_attack()));
                std::cout << s_hp << '\n';
            }
            if(getAttacked.getElapsedTime().asMilliseconds() > 200){
                getAttacked.restart();
            }
        }
    }
}

bool Skelet::player_collision(Player& p) const {
    sf::FloatRect skeletBounds = skelet_sprite.getGlobalBounds();
    sf::FloatRect playerBounds = p.getBounds();
    return skeletBounds.intersects(playerBounds);
}

void Skelet::drawEnemy(sf::RenderWindow &window) {
    window.draw(skelet_sprite);
}

void Skelet::positionUpdate(float x,float y) {
    skelet_sprite.setPosition(x,y);
}

std::shared_ptr<Enemy> Skelet::clone() const {
    return std::make_shared<Skelet>(*this);
}

int Skelet::getEnemyHp() const {
    return s_hp;
}
