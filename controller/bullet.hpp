
#pragma once
#include <memory>
#include <model/battle.hpp>
#include <model/bullet.hpp>
#include <model/asteroid.hpp>
#include <model/types.hpp>
#include <controller/icontroller.hpp>
#include <list>
#include <algorithm>
#include <functional>
#include <iostream>
#include <cmath>

namespace controller{


struct ibullet: icontroller
{
  typedef std::shared_ptr<ibullet> ptr;
};

// TODO: привязать контроллер к конкретному набору астероидов
class bullet: public ibullet
{
public:

  typedef std::shared_ptr<bullet> ptr;

  bullet(const model::bullet::ptr& m, const model::battle::ptr& b)
    : _model_bullet(m)
    , _model_battle(b)
  {
    _square = _model_battle->space_size();
  }

  virtual void update() override
  {
    model::position p = _model_bullet->get_position();
    model::position d = _model_bullet->get_delta();

    p.x+=d.x;
    p.y+=d.y;

    if ( p.x > _square.x || p.x < 0 || p.y > _square.y || p.y < 0 )
      _model_bullet->kill(true);
    _model_bullet->set_position(p);
  }

  bool is_life() const
  {
    return _model_bullet->is_life();
  }

  bool is_collide(const asteroid& other)
  {
    if ( !_model_bullet->is_life() )
      return false;

    return _model_bullet->is_collide(*other.get_model());
  }

  // TODO isateroid???
  bool collision(asteroid& /*other*/)
  {
    _model_bullet->kill(true);
    return true;
  }

  static bullet::ptr create(const model::position& p, const model::position& d, const model::battle::ptr& m)
  {
    model::bullet::ptr a = m->create_bullet(p);
    model::position db = d;
    db.x+=cos(p.a*model::DEGTORAD)*6;
    db.y+=sin(p.a*model::DEGTORAD)*6;
    a->set_delta(db);
    a->set_radius(5);
    return std::make_shared<bullet>(a, m);
  }


  model::bullet::ptr get_model() const
  {
    return _model_bullet;
  }
private:

private:
  model::bullet::ptr _model_bullet;
  model::battle::ptr _model_battle;
  model::position _square;
};

}

