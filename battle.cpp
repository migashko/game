#include <SFML/Graphics.hpp>
#include <model/battle.hpp>
#include <view/battle.hpp>
#include <controller/battle.hpp>
int main()
{
    srand(time(0));

    sf::RenderWindow app(sf::VideoMode(model::MAXW, model::MAXH), "Asteroids!");
    app.setFramerateLimit(60);
    auto battle_model = std::make_shared<model::battle>();
    auto battle_view = std::make_shared<view::battle>(battle_model);

    controller::battle battle_controller(battle_model);
    battle_controller.initialize();
    /////main loop/////
    battle_view->initialize();
    while (app.isOpen())
    {
      sf::Event event;
      if (app.pollEvent(event))
      {
        if (event.type == sf::Event::Closed)
          return 0;
      }

      battle_controller.update();
      battle_view->update();
      //////draw//////
      battle_view->draw(app);
      app.display();
    }


    return 0;
}
