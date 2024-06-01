//
// Created by pasca on 6/1/2024.
//

#include "../header/Zombie.h"
#include <memory>
#include <SFML/Window/Keyboard.hpp>

Zombie::Zombie(int hp, float dmg, float speed, std::string texturePath):
z_hp{hp}, z_dmg{dmg}, z_speed{speed}, z_texture_path{std::move(texturePath)}{
    z_texture.loadFromFile(z_texture_path);
    z_sprite.setTexture(z_texture);
}

void Zombie::update(Player& p, NPC& npc){
    Position target = p.getPosition();

    Position currentPosition = {z_sprite.getPosition().x, z_sprite.getPosition().y};
    Position direction = {target.x - currentPosition.x, target.y - currentPosition.y};
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0) {
        direction.x /= length;
        direction.y /= length;
    }

    z_sprite.move(direction.x * z_speed, direction.y * z_speed);

    if (player_collision(npc)) {
        if (attackTimer.getElapsedTime().asMilliseconds() >= 750) {
            npc.loseHp(z_dmg);
            std::cout << "NPC HP: " << npc.getHp() << '\n';
            attackTimer.restart();
        }
    }

    if (player_collision(p)) {
        if(attackTimer.getElapsedTime().asMilliseconds() >= 750) {
            p.loseHp(z_dmg);
            std::cout << "Player HP: " << p.getHp() << '\n';
            attackTimer.restart();
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            if(getAttacked.getElapsedTime().asMilliseconds() < 20) {
                std::cout << "hit" << '\n';
                z_hp -= static_cast<int>(std::floor(p.get_attack()));
                std::cout << z_hp << '\n';
            }
            if(getAttacked.getElapsedTime().asMilliseconds() > 200){
                getAttacked.restart();
            }
        }
    }
}

bool Zombie::player_collision(Player& p) const {
    sf::FloatRect zombieBounds = z_sprite.getGlobalBounds();
    sf::FloatRect playerBounds = p.getBounds();
    return zombieBounds.intersects(playerBounds);
}

void Zombie::drawEnemy(sf::RenderWindow& window) {
    window.draw(z_sprite);
}

std::shared_ptr<Enemy> Zombie::clone() const {
    return std::make_shared<Zombie>(*this);
}

int Zombie::getEnemyHp() const {
    return z_hp;
}

Zombie::Zombie(const Zombie &other) : z_hp{other.z_hp},z_dmg{other.z_dmg},
z_speed{other.z_speed},z_texture_path(other.z_texture_path){
    z_texture.loadFromFile(z_texture_path);
    z_sprite.setTexture(z_texture);
    z_sprite.setPosition(other.z_sprite.getPosition());
}

void Zombie::positionUpdate(float x,float y) {
    z_sprite.setPosition(x,y);
}

void swap(Zombie &first, Zombie &second) noexcept{
    using std::swap;
    swap(first.z_hp,second.z_hp);
    swap(first.z_dmg,second.z_dmg);
    swap(first.z_speed,second.z_speed);
    swap(first.z_texture_path,second.z_texture_path);
    swap(first.z_texture,second.z_texture);
    swap(first.z_sprite,second.z_sprite);
    swap(first.attackTimer,second.attackTimer);
    swap(first.getAttacked,second.getAttacked);
}

Zombie& Zombie::operator=(Zombie other) {
    swap(*this,other);
    return *this;
}