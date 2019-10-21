
#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include <model/bullet.hpp>
#include <model/types.hpp>
#include <view/animator.hpp>
#include <view/ientity.hpp>
#include <list>

namespace view{


class bullet: public ientity
{
public:
  bullet(const model::bullet::ptr& am)
    : _model(am)
  {
  }

  void initialize(const sf::Texture& texture)
  {
    _animator.initialize(texture, 0, 0, 32, 64, 16, 0.8);
  }

  virtual void update()
  {
    _animator.update();
  }

  virtual void draw(sf::RenderWindow& app)
  {
     _animator.draw(app, _model->get_position());
     //_animator.draw_circle(app, _model->get_position(), _model->get_radius());
  }

  virtual bool is_life() const
  {
    return _model->is_life();
  }
private:
  model::bullet::ptr _model;
  animator _animator;
};

}

