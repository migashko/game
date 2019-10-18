#pragma once
#include <memory>
#include <model/battle.hpp>
#include <model/asteroid.hpp>
#include <model/types.hpp>
#include <controller/icontroller.hpp>
#include <list>
#include <algorithm>
#include <functional>
#include <iostream>

namespace controller{


struct iasteroid: icontroller
{
  typedef std::shared_ptr<iasteroid> ptr;
};

// TODO: привязать контроллер к конкретному набору астероидов
class asteroid: public iasteroid
{
public:

  typedef std::shared_ptr<asteroid> ptr;

  asteroid(const model::asteroid::ptr& m, const model::battle::ptr& b)
    : _model_asteroid(m)
    , _model_battle(b)
  {
    _square = _model_battle->space_size();
  }

  virtual void update() override
  {
    model::position p = _model_asteroid->get_position();
    model::position d = _model_asteroid->get_delta();

    p.x+=d.x;
    p.y+=d.y;

    if ( p.x > _square.x ) p.x = 0;
    if ( p.x < 0 ) p.x = _square.x;
    if ( p.y > _square.y ) p.y=0;
    if ( p.y < 0 ) p.y = _square.y;

    _model_asteroid->set_position(p);
    if ( _collision_ban!=0 )
      --_collision_ban;
  }
  
  bool is_life() const
  {
    return _model_asteroid->is_life();
  }
  
  bool is_collide(const asteroid& other)
  {
    if ( !_model_asteroid->is_life() )
      return false;
    
    if ( _collision_ban!=0 )
      return false;
    return _model_asteroid->is_collide(*other.get_model());
  }

  bool collision(asteroid& /*other*/)
  {
    _collision_ban = 150;
    auto d1 = _model_asteroid->get_delta();
    d1.x *= -1;
    d1.y *= -1;
    _model_asteroid->set_delta(d1);
    bool dead = rand()%2 == 0;
    if ( dead )
      _model_asteroid->kill(true);
    return dead;
    
  }
  
  static asteroid::ptr create(const model::position& p, const model::battle::ptr& m)
  {
    model::asteroid::ptr a = m->create_asteroid();
    a->set_size(1);
    a->set_position(p);
    int dx =  rand()%8-4;
    int dy =  rand()%8-4;
    if ( dx == 0 )
      dx = 1;
    a->set_delta( model::position{ float(dx), float(dy)});
    a->set_radius(20);
    return std::make_shared<asteroid>(a, m);
  }

  static asteroid::ptr create(const model::battle::ptr& m)
  {
    auto square = m->space_size();
    return create(model::position{ 0, float(rand()%int(square.y)), float(rand()%360)}, m);
  }

  model::asteroid::ptr get_model() const
  {
    return _model_asteroid;
  }
private:

private:
  size_t _collision_ban = 150;
  model::asteroid::ptr _model_asteroid;
  model::battle::ptr _model_battle;
  model::position _square;
};

}

