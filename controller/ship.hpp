
#pragma once
#include <memory>
#include <model/battle.hpp>
#include <model/ship.hpp>
#include <model/types.hpp>
#include <model/asteroid.hpp>
#include <controller/icontroller.hpp>
#include <list>
#include <algorithm>
#include <functional>
#include <iostream>
#include <cmath>

namespace controller{


struct iship: icontroller
{
  typedef std::shared_ptr<iship> ptr;
};

// TODO: привязать контроллер к конкретному набору астероидов
class ship: public iship
{
public:

  typedef std::shared_ptr<ship> ptr;

  ship(const model::ship::ptr& m, const model::battle::ptr& b)
    : _model_ship(m)
    , _model_battle(b)
  {
    _square = _model_battle->space_size();
  }

  ptr clone()
  {
    return std::make_shared<ship>( std::make_shared<model::ship>(*_model_ship), _model_battle);
  }

  // для прогноза
  model::position next(const model::position& cur) const
  {
    model::position p = cur;
    model::position d = _model_ship->get_delta();
    p.x+=d.x;
    p.y+=d.y;
    if ( p.x > _square.x ) p.x = 0;
    if ( p.x < 0 ) p.x = _square.x;
    if ( p.y > _square.y ) p.y=0;
    if ( p.y < 0 ) p.y = _square.y;
    return p;
  }


  virtual void update() override
  {
    model::position p = _model_ship->get_position();
    model::position d = _model_ship->get_delta();
    bool is_trust = _model_ship->has_state<model::ship::THRUST>();
    bool is_breaking = _model_ship->has_state<model::ship::BREAKING>();



    if ( _model_ship->has_state<model::ship::ROTATE_LEFT>() )
    {
      p = model::rotate(p, -3);
    }

    if ( _model_ship->has_state<model::ship::ROTATE_RIGHT>() )
    {
      p = model::rotate(p, 3);
    }

    if ( is_trust )
    {
      d.x+=cos(p.a*model::DEGTORAD)*0.2;
      d.y+=sin(p.a*model::DEGTORAD)*0.2;
    }
    else if ( is_breaking )
    {
      /*d.x*=0.98;
      d.y*=0.98  ;*/
      d.x*=0.95;
      d.y*=0.95;
    }

    int maxSpeed=15;
    float speed = sqrt(d.x*d.x+d.y*d.y);
    if ( speed > maxSpeed )
    {
      d.x *= maxSpeed/speed;
      d.y *= maxSpeed/speed;
    }

    /*
    p.x += d.x;
    p.y += d.y;

    if ( p.x > _square.x ) p.x = 0;
    if ( p.x < 0 ) p.x = _square.x;
    if ( p.y > _square.y ) p.y=0;
    if ( p.y < 0 ) p.y = _square.y;
    */
    p = this->next(p);

    _model_ship->set_position(p);
    _model_ship->set_delta(d);
  }

  bool is_life() const
  {
    return _model_ship->is_life();
  }

  bool is_collide(const asteroid& other)
  {
    if ( !_model_ship->is_life() )
      return false;

    return _model_ship->is_collide(*other.get_model());
  }

  void collision(ship& /*other*/)
  {
    //_model_ship->kill(true);
  }

  static ship::ptr create(const model::position& p, const model::battle::ptr& m)
  {
    model::ship::ptr a = m->create_ship(p);
    a->set_radius(20);
    return std::make_shared<ship>(a, m);
  }

  static ship::ptr create(const model::battle::ptr& m)
  {
    auto square = m->space_size();
    return create(model::position{ float(square.x/2), float(square.y/2), 0}, m);
  }

  model::ship::ptr get_model() const
  {
    return _model_ship;
  }

  void rotate_left(bool value)
  {
    _model_ship->set_state<model::ship::ROTATE_LEFT>(value);
  }
  void rotate_right(bool value)
  {
    _model_ship->set_state<model::ship::ROTATE_RIGHT>(value);
  }

  void thrust(bool value)
  {
    _model_ship->set_state<model::ship::THRUST>(value);
  }
  void breaking(bool value)
  {
    _model_ship->set_state<model::ship::BREAKING>(value);
  }

  void shuting(bool value)
  {
    _model_ship->set_state<model::ship::SHOOTING>(value);
  }

    /*
   * На сколько шагов просчитывать
   * Через сколько шагов возможно
   * return количество шагов,
   * -1 невозможно
   * Пока простым перебором
   */
  long collision_forecast(const asteroid& other, long steps)
  {
    auto ps = *_model_ship;
    auto pa = *other.get_model();
    for (long i = 0; i < steps; ++i)
    {
      if ( ps.is_collide(pa))
        return i;
      ps.set_position( this->next( ps.get_position() ) );
      pa.set_position( other.next( pa.get_position() ) );
    }
    return -1;
  }


private:

private:
  model::ship::ptr _model_ship;
  model::battle::ptr _model_battle;
  model::position _square;
};

}

