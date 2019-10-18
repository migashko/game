#pragma once

#include <controller/asteroid.hpp>
#include <controller/explosion.hpp>
#include <controller/ship.hpp>
#include <model/battle.hpp>

namespace controller{

class battle
{
public:
  battle(model::battle::ptr m)
    : _model(m)
  {}

  void initialize()
  {
    //_asteroid = std::make_shared<asteroid>(_model);
    for (int i = 0; i < 30; ++i)
      this->create_asteroid();
    _player = this->create_ship();
  }

  void update()
  {
    if ( rand() % 50 == 0)
    {
      this->create_asteroid();
    }
   
    {
    auto beg = _controlled_asteroid.begin();
    auto end = _controlled_asteroid.end();
    for (;beg!=end;)
    {
      (*beg)->update();
      if ( !(*beg)->is_life() )
      {
        _controlled_asteroid.erase(beg++);
        continue;
      }
      
      auto itr = beg;
      ++itr;
      if ( itr == end ) break;
      for (;itr!=end;++itr)
      {
        if ( (*itr)->is_collide(**beg) )
        {
          auto p1 = (*itr)->get_model()->get_position();
          auto p2 = (*beg)->get_model()->get_position();
          bool dead1 = (*itr)->collision(**beg);
          bool dead2 = (*beg)->collision(**itr);
          if ( dead1 || dead2 )
          {
            this->create_explosion(model::middle(p1, p2));
          }
          if ( dead1 && (*itr)->get_model()->get_size() == 1 )
            this->create_small_asteroids(p1, 4);
          if ( dead2 && (*beg)->get_model()->get_size() == 1 )
            this->create_small_asteroids(p2, 4);
        }
      }
      ++beg;
    }
    }
    
    {
      auto beg = _controlled_explosion.begin();
      auto end = _controlled_explosion.end();
      for (;beg!=end;)
      {
        (*beg)->update();
        if ( !(*beg)->is_life() )
        {
          _controlled_explosion.erase(beg++);
          continue;
        }
        ++beg;
      }
    }

    {
      auto beg = _controlled_ship.begin();
      auto end = _controlled_ship.end();
      for (;beg!=end;)
      {
        (*beg)->update();
        if ( !(*beg)->is_life() )
        {
          _controlled_ship.erase(beg++);
          continue;
        }
        ++beg;
      }
    }

  }

  asteroid::ptr create_asteroid()
  {
    auto a = asteroid::create(_model);
    _controlled_asteroid.push_back(a);
    return a;
  }

  void create_small_asteroids(const model::position& p, int count)
  {
    for (int i=0; i < count; ++i)
    {
      auto a = asteroid::create(p, _model);
      a->get_model()->set_size(0);
      _controlled_asteroid.push_back(a);
    }
  }

  explosion::ptr create_explosion(const model::position& p)
  {
    auto a = explosion::create(p, _model);
    _controlled_explosion.push_back(a);
    return a;
  }
  
  ship::ptr create_ship()
  {
    auto a = ship::create(_model);
    _controlled_ship.push_back(a);
    return a;
  }
  
  
  void rotate_left(bool value) { _player->rotate_left(value);}
  void rotate_right(bool value) { _player->rotate_right(value); }
  void thrust(bool value) { _player->thrust(value); }
  void breaking(bool value) { _player->breaking(value); }
  void shuting(bool value) { _player->shuting(value); }
  

private:
  model::battle::ptr _model;
  std::list<asteroid::ptr> _controlled_asteroid;
  std::list<explosion::ptr> _controlled_explosion;
  std::list<ship::ptr> _controlled_ship;
  ship::ptr _player;

};

}

