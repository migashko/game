#pragma once
#include <memory>
#include <model/battle.hpp>
#include <model/asteroid.hpp>
#include <model/types.hpp>
#include <list>
#include <algorithm>
#include <functional>
#include <iostream>

namespace controller{

struct icontroller
{
  typedef std::shared_ptr<icontroller> ptr;
  virtual void update() = 0;
};

struct iasteroid: icontroller
{
  typedef std::shared_ptr<iasteroid> ptr;
};

// TODO: привязать контроллер к конкретному набору астероидов
class asteroid: iasteroid
{
public:

  typedef std::shared_ptr<asteroid> ptr;

  asteroid(const model::battle::ptr& m)
    : _model(m)
  {
    _square = _model->space_size();
  }

  virtual void update() override
  {
    using namespace std::placeholders;
    auto beg = _controlled_asteroid.begin();
    auto end = _controlled_asteroid.end();
    std::for_each(beg, end, std::bind(&asteroid::update_asteroid, this, _1) );

    // Столкновения астероидов
    // TODO: Сделать пару астероид-блокировка коллизий
    for (;beg!=end;++beg)
    {
      auto itr = beg;
      ++itr;
      if ( itr == end ) break;
      for (;itr!=end;++itr)
      {
        if ( (*itr)->is_collide(**beg) )
        {
          if ( rand() % (_controlled_asteroid.size()*2) == 0 )
          {
            auto d1 = (*itr)->get_delta();
            auto d2 = (*beg)->get_delta();
            d1.x *= -1;
            d1.y *= -1;
            d2.x *= -1;
            d2.y *= -1;
            (*itr)->set_delta(d1);
            (*beg)->set_delta(d2);
            for (int i = 0; i < 10 && (*itr)->is_collide(**beg) ; ++i)
            {
              this->update_asteroid(*itr);
              this->update_asteroid(*beg);
            }
          }
        }

      }
    }

  }

  model::asteroid::ptr create_asteroid()
  {
    model::asteroid::ptr a = _model->create_asteroid();
    a->set_position( model::position{ 0, float(rand()%int(_square.y)), float(rand()%360)});
    int dx =  rand()%8-4;
    int dy =  rand()%8-4;
    if ( dx == 0 )
      dx = 1;
    a->set_delta( model::position{ float(dx), float(dy)});
    a->set_radius(20);
    _controlled_asteroid.push_back(a);
    return a;
  }

private:
  void update_asteroid(model::asteroid::ptr& a)
  {
    model::position p = a->get_position();
    model::position d = a->get_delta();

    p.x+=d.x;
    p.y+=d.y;

    if ( p.x > _square.x ) p.x = 0;
    if ( p.x < 0 ) p.x = _square.x;
    if ( p.y > _square.y ) p.y=0;
    if ( p.y < 0 ) p.y = _square.y;

    a->set_position(p);
  }

private:
  model::battle::ptr _model;
  model::position _square;
  std::list<model::asteroid::ptr> _controlled_asteroid;
};

}

