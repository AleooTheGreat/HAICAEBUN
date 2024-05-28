#include <random>
#include <chrono>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "../header/Chapter2.h"
#include "../header/GameExceptions.h"


Chapter2::Chapter2() : poziti({
                                      {100, 100}, {150, 250}, {350, 250}, {700, 200}, {1000, 300},
                                      {1000, 1100}, {900, 100}, {1050, 250}, {1250, 1250}, {790, 900},
                                      {1100, 100}, {100, 1100}}), wave{Begin}, stage{Playing} {
    try {
        readFromFile("maps/map2.txt");
        generateEnemies();
    } catch (const FileLoadException& e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

void Chapter2::generateEnemies() {
    std::unordered_map<unsigned long long, bool> frq;

    if (wave == Begin) {
        /// Generam 4 vampiri
        for (int i = 0; i <= 3; i++) {
            auto vamp1 = std::make_shared<Vampir>();

            /// Generam pozitii random
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<unsigned long long> distrib(0, poziti.size() - 1);

            unsigned long long number = distrib(gen);
            while (frq[number]) {
                number = distrib(gen);
            }
            frq[number] = true;
            vamp1->positionUpdate(poziti[number].x, poziti[number].y);

            enemies.emplace_back(vamp1);
        }

        frq.clear();
    } else if (wave == Medium) {
        /// generam 5 skeletoni
        for (int i = 0; i <= 4; i++) {
            auto skelet = std::make_shared<Skelet>();

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<unsigned long long> distrib(0, poziti.size() - 1);

            unsigned long long number = distrib(gen);
            while (frq[number]) {
                number = distrib(gen);
            }
            frq[number] = true;
            skelet->positionUpdate(poziti[number].x, poziti[number].y);

            enemies.emplace_back(skelet);
        }

        frq.clear();
    } else if (wave == Hard) {
        /// generam 5 vampiri cu 4 skeletoni
        for (int i = 0; i <= 4; i++) {
            auto skelet = std::make_shared<Skelet>();

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<unsigned long long> distrib(0, poziti.size() - 1);

            unsigned long long number = distrib(gen);
            while (frq[number]) {
                number = distrib(gen);
            }
            frq[number] = true;
            skelet->positionUpdate(poziti[number].x, poziti[number].y);

            enemies.emplace_back(skelet);
        }
        for (int i = 0; i <= 3; i++) {
            auto vamp1 = std::make_shared<Vampir>();

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<unsigned long long> distrib(0, poziti.size() - 1);

            unsigned long long number = distrib(gen);
            while (frq[number]) {
                number = distrib(gen);
            }
            frq[number] = true;
            vamp1->positionUpdate(poziti[number].x, poziti[number].y);

            enemies.emplace_back(vamp1);
        }

        frq.clear();
    }
}

void Chapter2::update() {
    main_player.handleInput();
    main_player.update(Map::convert_map(map2));
    unsigned long long cnt = 0;
    for (auto& it : enemies) {
        if (it->getEnemyHp() > 0) {
            it->update(main_player, help_player);
        } else {
            cnt++;
        }
    }

    if (cnt == enemies.size() && wave == Begin) {
        cnt = 0;
        main_player.buff(120, 6);
        help_player.heal(150);
        wave = Medium;
        enemies.clear();
        generateEnemies();
        std::cout << main_player.getHp() << " " << main_player.get_attack() << '\n';
        std::cout << help_player.getHp() << "\n";
    } else if (cnt == enemies.size() && wave == Medium) {
        cnt = 0;
        main_player.buff(140, 9);
        help_player.heal(170);
        wave = Hard;
        enemies.clear();
        generateEnemies();
        std::cout << main_player.getHp() << " " << main_player.get_attack() << '\n';
        std::cout << help_player.getHp() << "\n";
    }

    if (main_player.getHp() < 0 || help_player.getHp() < 0) {
        stage = Defeat;
    } else if (wave == Hard && cnt == enemies.size()) {
        stage = Victory;
    }

    checkEnemies();
}

void Chapter2::checkEnemies() {
    for (const auto& enemy : enemies) {
        /// dynamic_cast pentru a identifica tipul de inamic
        if (auto vampir = dynamic_cast<Vampir*>(enemy.get())) {
            std::cout << "Found a Vampir with HP: " << vampir->getEnemyHp() << std::endl;
        } else if (auto skelet = dynamic_cast<Skelet*>(enemy.get())) {
            std::cout << "Found a Skelet with HP: " << skelet->getEnemyHp() << std::endl;
        }

        /// Demonstram ca clonare functioneaza
        auto cloned_enemy = enemy->clone();
        std::cout << "Cloned enemy HP: " << cloned_enemy->getEnemyHp() << std::endl;
    }
}

void Chapter2::render(sf::RenderWindow &window) {
    Map::draw(Map::convert_map(map2), window);
    help_player.drawNPC(window);
    for (auto& it : enemies) {
        if (it->getEnemyHp() > 0) {
            it->drawEnemy(window);
        }
    }
    main_player.drawPlayer(window);
}

int Chapter2::keepPlaying() {
    switch (stage) {
        case Victory: {
            return 1;
        }
        case Defeat: {
            return 0;
        }
        default: {
            return 2;
        }
    }
}

void Chapter2::readFromFile(const std::string &filePath) {
    std::ifstream fin(filePath);
    if (!fin.is_open()) {
        throw FileLoadException("Could not open file: " + filePath);
    }

    int index = 0;
    std::string line;
    while (std::getline(fin, line) && index < Map_height) {
        map2[index] = line;
        index++;
    }

    if (index < Map_height) {
        throw FileLoadException("File is incomplete: " + filePath);
    }

    fin.close();
}

void Chapter2::testCopyAndSwap() {
    Skelet original_skelet(100, 10, 1.5, "textures/skelet.png");
    Vampir original_vampir(100, 15, 2.0, "textures/vampir.png");

    /// constructorul de copiere
    const Skelet& copied_skelet(original_skelet);
    const Vampir& copied_vampir(original_vampir);

    /// vedem ca totul e ok
    std::cout << "Original Skelet HP: " << original_skelet.getEnemyHp() << std::endl;
    std::cout << "Copied Skelet HP: " << copied_skelet.getEnemyHp() << std::endl;

    std::cout << "Original Vampir HP: " << original_vampir.getEnemyHp() << std::endl;
    std::cout << "Copied Vampir HP: " << copied_vampir.getEnemyHp() << std::endl;

    /// Copy & Swap
    Skelet assigned_skelet;
    assigned_skelet = original_skelet;

    Vampir assigned_vampir;
    assigned_vampir = original_vampir;

    /// verificam ca totul e ok
    std::cout << "Assigned Skelet HP: " << assigned_skelet.getEnemyHp() << std::endl;
    std::cout << "Assigned Vampir HP: " << assigned_vampir.getEnemyHp() << std::endl;
}
