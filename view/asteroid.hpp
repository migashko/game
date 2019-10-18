#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include <model/asteroid.hpp>
#include <model/types.hpp>
#include <view/animator.hpp>
#include <view/ientity.hpp>
#include <list>

namespace view{


class asteroid: public ientity
{
public:
  asteroid(const model::asteroid::ptr& am)
    : _model(am)
  {
  }

  void initialize(const sf::Texture& texture)
  {
    //_texture = &texture;
    _animator.initialize(texture, 0, 0, 64, 64, 16, 0.2);
  }

  virtual void update()
  {
    _animator.update();
  }

  virtual void draw(sf::RenderWindow& app)
  {
     _animator
     .draw(app, _model->get_position());
      /*
     sf::CircleShape circle(R);
     circle.setFillColor(sf::Color(255,0,0,170));
     circle.setPosition(x,y);
     circle.setOrigin(R,R);
     //app.draw(circle);
      */
  }
  
  virtual bool is_life() const
  {
    return _model->is_life();
  }
private:
  model::asteroid::ptr _model;
  animator _animator;
  //const sf::Texture* _texture;

};

}

