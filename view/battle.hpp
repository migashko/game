#pragma once
#include <model/battle.hpp>
#include <SFML/Graphics.hpp>
#include <view/asteroid.hpp>
#include <view/explosion.hpp>
#include <view/ship.hpp>
#include <view/animator.hpp>
#include <iostream>

namespace view{

class battle
{
public:
  battle(model::battle::ptr m)
    : _model(m)
  {}

  void initialize()
  {
    _background_texture.loadFromFile("images/background.jpg");
    _background_texture.setSmooth(true);
    _background_sprite.setTexture(_background_texture);
    
    _asteroid_texture.loadFromFile("images/rock.png");
    _asteroid_small_texture.loadFromFile("images/rock_small.png");
    _asteroid_explosion_texture.loadFromFile("images/explosions/type_C.png");
    _ship_texture.loadFromFile("images/spaceship.png");
  }

  void update()
  {
    while ( auto ma = _model->detach_new_asteroid() )
    {
      auto va = std::make_shared<asteroid>(ma);
      if ( ma->get_size() == 0 )
        va->initialize(_asteroid_small_texture);
      else
        va->initialize(_asteroid_texture);
      _entities.push_back(va);
    }

    while ( auto ma = _model->detach_new_explosion() )
    {
      auto va = std::make_shared<explosion>(ma);
      va->initialize(_asteroid_explosion_texture);
      _entities.push_back(va);
    }

    while ( auto ma = _model->detach_new_ship() )
    {
      auto va = std::make_shared<ship>(ma);
      va->initialize(_ship_texture);
      _entities.push_back(va);
    }

    
    for ( auto& e : _entities)
      e->update();
    
    _entities.erase(
      std::remove_if(
        _entities.begin(),
        _entities.end(), 
        [](const ientity::ptr& e) { return !e->is_life(); }
      ),
      _entities.end()
    );
  }

  void draw(sf::RenderWindow& app)
  {
    //std::cout << "draw: " << _entities.size() << std::endl;;
    
    app.draw(_background_sprite);
    for ( auto& e : _entities)
      e->draw(app);
  }

private:
  model::battle::ptr _model;
  sf::Texture _background_texture;
  sf::Sprite _background_sprite;
  
  sf::Texture _asteroid_texture;
  sf::Texture _asteroid_small_texture;
  sf::Texture _asteroid_explosion_texture;
  sf::Texture _ship_texture;
  sf::Texture _ship_thrust_texture;

  std::list<ientity::ptr> _entities;
};

}

