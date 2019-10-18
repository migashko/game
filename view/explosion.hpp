#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include <model/explosion.hpp>
#include <model/types.hpp>
#include <view/animator.hpp>
#include <view/ientity.hpp>
#include <list>

namespace view{

class explosion: public ientity
{
public:
  explosion(const model::explosion::ptr& am)
    : _model(am)
  {
  }

  void initialize(const sf::Texture& texture)
  {
    _animator.initialize(texture, 0,0,256,256, 48, 0.5);
  }

  virtual void update()
  {
    _animator.update();
  }

  virtual void draw(sf::RenderWindow& app)
  {
    _animator.draw(app, _model->get_position());
  }
  
  virtual bool is_life() const
  {
    return _model->is_life();
  }
private:
  model::explosion::ptr _model;
  animator _animator;
};

}

