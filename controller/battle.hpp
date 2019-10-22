#pragma once

#include <controller/asteroid.hpp>
#include <controller/explosion.hpp>
#include <controller/ship.hpp>
#include <controller/bullet.hpp>
#include <model/battle.hpp>
#include <deque>
#include <vector>

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
    for (int i = 0; i < 15; ++i)
      this->create_asteroid();
    _player = this->create_ship();
  }

  void update()
  {
    // Созание случайного астероида
    if ( /*rand() % 50 == 0*/ false)
    {
      this->create_asteroid();
    }

    // Созание выстрела
    if (_player!=nullptr){
      if ( _player->get_model()->has_state<model::ship::SHOOTING>() )
      {
        this->create_bullet();
      }
    }

    // Столкновения
    {
      long ahtung = 1000000;
      asteroid::ptr ahtung_asteroid;

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

        // астероид с игроками (одним)
        for (auto itr_ship = _controlled_ship.begin(); itr_ship != _controlled_ship.end(); ++itr_ship)
        {
          if ( (*itr_ship)->is_collide(**beg) )
          {
            auto p1 = (*itr_ship)->get_model()->get_position();
            auto p2 = (*beg)->get_model()->get_position();
            this->create_explosion(p2);
            this->create_explosion(p1)->get_model()->set_type(1);
            (*itr_ship)->get_model()->kill(true);
            (*beg)->get_model()->kill(true);
          }
          else if (_auto )
          {
            long forecast =  (*itr_ship)->collision_forecast(**beg, 400);
            if ( forecast > 0 )
            {
              std::cout << "Ахтунг: " << forecast << std::endl;
              if ( forecast < ahtung )
              {
                ahtung = forecast;
                ahtung_asteroid = *beg;
              }
            }
          }
        }
        // Столкновения астероидов
        /*auto itr = beg;
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
        }*/

        // проверка на пульи
        if ( (*beg)->is_life() )
        for (auto& b: _controlled_bullet)
        {

          if ( b->is_collide( *(*beg) ) )
          {
            b->get_model()->kill(true);
            (*beg)->get_model()->kill(true);
            this->create_explosion((*beg)->get_model()->get_position());
          }
        }
        ++beg;
      }

      // TODO сделать рекурсивную функцию глубиины N преебрать все варианты действий
      if ( ahtung > 0 && _player!=nullptr && ahtung_asteroid!=nullptr)
      {
        std::cout << "TOTAL: " << ahtung << std::endl << std::endl;
        // Определяем последовательность событий, которое приведет к уходу от астероида
        // Прогназируем на 100 шагов, прекращаем на любом успехе
        size_t curaction = 32;
        long curahtung = ahtung;
        // перебираем все действия
        std::vector<size_t> actions {0,1,2,4,6,8};


        for (auto action : actions)
        {
            auto sprobe = _player->clone();
            auto aprobe = ahtung_asteroid->clone();
            sprobe->get_model()->set_state(action);
            bool fatal = false;

            for ( long step = 0; step < 400; ++step)
            {
              sprobe->update();
              aprobe->update();
              if ( step > 0 )
              {
                sprobe->get_model()->set_state<model::ship::ROTATE_LEFT>(0);
                sprobe->get_model()->set_state<model::ship::ROTATE_RIGHT>(0);
              }
              fatal =  sprobe->is_collide(*aprobe);
              if ( fatal )
                break;
            }

            if ( !fatal )
            {
              curaction = action;
              break;
            }
        }
        std::cout << "curaction: " <<  curaction << " curahtung " << curahtung << std::endl;
        if ( curaction < 32 )
        {
          _last_action = curaction;
          _player->get_model()->set_state(_last_action);
        }
      }
      else if ( _auto && _player!=nullptr)
        _player->get_model()->set_state(_last_action);
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
          if ( _player!= nullptr && _player == *beg)
            _player = nullptr;
          _controlled_ship.erase(beg++);
          continue;
        }
        ++beg;
      }
    }

    {
      auto beg = _controlled_bullet.begin();
      auto end = _controlled_bullet.end();
      for (;beg!=end;)
      {
        (*beg)->update();
        if ( !(*beg)->is_life() )
        {
          _controlled_bullet.erase(beg++);
          continue;
        }
        ++beg;
      }
    }
    _auto = true;
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
      a->get_model()->set_radius(10);
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

  bullet::ptr create_bullet()
  {
    if ( _player == nullptr )
      return nullptr;

    auto p = _player->get_model()->get_position();
    auto d = _player->get_model()->get_delta();
    //auto a = bullet::create( p, d, _model);
    //_controlled_bullet.push_back(a);

    for ( float i =0 ; i < 360; i+=45)
    {
      p.a+=i;
      auto a = bullet::create( p, d, _model);
      _controlled_bullet.push_back(a);
    }

    return nullptr;
  }


  void rotate_left(bool value) { if (_player!=nullptr) _player->rotate_left(value); _auto |= value;}
  void rotate_right(bool value) { if (_player!=nullptr) _player->rotate_right(value); _auto |= value;}
  void thrust(bool value) { if (_player!=nullptr) _player->thrust(value); _auto |= value;}
  void breaking(bool value) { if (_player!=nullptr) _player->breaking(value); _auto |= value;}
  void shuting(bool value) { if (_player!=nullptr) _player->shuting(value); _auto |= value;}


private:
  bool _auto = true;
  size_t _last_action = 0;
  model::battle::ptr _model;
  std::list<asteroid::ptr> _controlled_asteroid;
  std::list<explosion::ptr> _controlled_explosion;
  std::list<ship::ptr> _controlled_ship;
  std::list<bullet::ptr> _controlled_bullet;
  ship::ptr _player;

};

}

