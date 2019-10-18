#pragma once
#include <memory>
#include <model/battle.hpp>
#include <model/explosion.hpp>
#include <model/types.hpp>
#include <list>
#include <algorithm>
#include <functional>
#include <iostream>
#include <controller/icontroller.hpp>
namespace controller{

// TODO: привязать контроллер к конкретному набору астероидов
class explosion: public icontroller
{
public:

  typedef std::shared_ptr<explosion> ptr;

  explosion(const model::explosion::ptr& m, const model::battle::ptr& b)
    : _model_explosion(m)
    , _model_battle(b)
  {
    
  }
  
  bool is_life() const
  {
    return _model_explosion->is_life();
  }


  virtual void update() override
  {
    if ( _life > 0 )
    {
      --_life;
      if ( _life == 0 ) 
      {
        _model_explosion->kill(true);
      }
    }
  }
  
  static explosion::ptr create(const model::position& p, const model::battle::ptr& m)
  {
    model::explosion::ptr a = m->create_explosion(p);
    a->set_position(p);
    return std::make_shared<explosion>(a, m);
  }

  model::explosion::ptr get_model() const
  {
    return _model_explosion;
  }
private:
  size_t _life = 48;
  model::explosion::ptr _model_explosion;
  model::battle::ptr _model_battle;
  
};

}

