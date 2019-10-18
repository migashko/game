#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include <model/ship.hpp>
#include <model/types.hpp>
#include <view/animator.hpp>
#include <view/ientity.hpp>
#include <list>

namespace view{


class ship: public ientity
{
public:
  ship(const model::ship::ptr& am)
    : _model(am)
  {
  }

  void initialize(const sf::Texture& texture)
  {
    _animator.initialize(texture, 40, 0, 40, 40, 1, 0);
    _animator_thrust.initialize(texture, 40, 40, 40, 40, 1, 0);
  }

  virtual void update()
  {
    _animator.update();
  }

  virtual void draw(sf::RenderWindow& app)
  {
    if ( _model->has_state<model::ship::THRUST>() )
      _animator_thrust.draw(app, _model->get_position());
    else
      _animator.draw(app, _model->get_position());
  }
  
  virtual bool is_life() const
  {
    return _model->is_life();
  }
private:
  model::ship::ptr _model;
  animator _animator;
  animator _animator_thrust;
};

}

