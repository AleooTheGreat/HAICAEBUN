//
// Created by pasca on 5/27/2024.
//

#include "../header/Chapter1.h"

Chapter1::Chapter1() {
      map1 = {
              "####################",
              "#        #         #",
              "# ## ### # ### ##  #",
              "#                  #",
              "# ## # ##### # ### #",
              "#    #   #   #     #",
              "#### ### # ### #####",
              "#   # #       # #  #",
              "##### # ## ## # ####",
              "#       #   #      #",
              "##### # ## ## # ####",
              "#   # #       # #  #",
              "#### # ##### # #####",
              "#        #         #",
              "# ## ### # ### ### #",
              "# #            #   #",
              "## # # ##### # # ###",
              "#    #   #   #     #",
              "# ###### # ######  #",
              "####################"
      };

    ///Initializari
     Bat bat4(bat1);  ///Aici demonstram ca functioneaza copy si cu egal si &Bat
     bat3 = bat4;
     bat3.setPosition(768,960);
     bat2 = bat1;

     stage = Playing;
}

void Chapter1::update() {
    if(main_player.getHp() > 0 && (bat1.getHp() > 0 || bat2.getHp() > 0 || bat3.getHp() > 0)) {
        main_player.handleInput();
        main_player.update(Map::convert_map(map1));
        if(bat1.getHp() > 0) {
            bat1.update(main_player, Map::convert_map(map1));
        }
        if(bat2.getHp() > 0) {
            bat2.update(main_player, Map::convert_map(map1));
        }
        if(bat3.getHp() > 0) {
            bat3.update(main_player, Map::convert_map(map1));
        }
        stage = Playing;
    }else if(main_player.getHp() < 0){
         stage = Defeat;
    }else{
        stage = Victory;
    }
}

unsigned short int Chapter1::keepCondition() {
    switch(stage){
        case Defeat: {
            return 0;
        }
        case Victory: {
            return 2;
        }
        case Playing: {
            return 1;
        }
        default: {
            return 3;
        }
    }

}

void Chapter1::render(sf::RenderWindow &window) {
    if(stage == Playing) {

        Map::draw(Map::convert_map(map1), window);
        main_player.drawPlayer(window);

        if(bat1.getHp() > 0) {
            bat1.draw(window);
        }
        if(bat2.getHp() > 0) {
            bat2.draw(window);
        }
        if(bat3.getHp() > 0) {
            bat3.draw(window);
        }
    }
}
