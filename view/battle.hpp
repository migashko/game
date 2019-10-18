#pragma once
#include <model/battle.hpp>
#include <SFML/Graphics.hpp>
#include <view/asteroid.hpp>
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
  }

  void update()
  {
    while ( auto ma = _model->detach_new_asteroid() )
    {
      auto va = std::make_shared<asteroid>(ma);
      va->initialize();
      _entities.push_back(va);
    }
    for ( auto& e : _entities)
      e->update();
  }

  void draw(sf::RenderWindow& app)
  {
    app.draw(_background_sprite);
    for ( auto& e : _entities)
      e->draw(app);
  }

private:
  model::battle::ptr _model;
  sf::Texture _background_texture;
  sf::Sprite _background_sprite;
  animator _asteroid_animator;

  std::list<ientity::ptr> _entities;
};

}

