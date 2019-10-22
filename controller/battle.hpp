#pragma once

#include <controller/asteroid.hpp>
#include <controller/explosion.hpp>
#include <controller/ship.hpp>
#include <controller/bullet.hpp>
#include <model/battle.hpp>
#include <model/types.hpp>
#include <deque>
#include <vector>
#include <map>

namespace controller{
  
size_t collision_check(/*ship::ptr real_ship,*/ ship::ptr fship, std::vector<asteroid::ptr>& asteroids)
{
  size_t collision_count = 0;
  for (const auto& a: asteroids)
  {
    if ( fship->is_collide(*a) )
    {
      ++collision_count;
      /*size_t dist = distance(real_ship->get_model()->get_position(), a->get_model()->get_position());
      if ( dist < mindist )
        mindist = dist;*/
    }
  }
    
  return collision_count;
  // Нет столкновений 
  //return mindist;
}

// Проверка пути, что нет коллизий
// Чем больше число, тем хуже
inline std::pair<size_t, size_t> course_forecast(/*ship::ptr real_ship,*/ ship::ptr fship, std::vector<asteroid::ptr>& asteroids)
{
  size_t collision_count = 0;
  ship::ptr nship = fship->clone();
  std::vector<asteroid::ptr> nasteroids;
  nasteroids.reserve(asteroids.size() );
  for (auto a : asteroids)
    nasteroids.push_back(a->clone());
  
  for (size_t i = 0 ; i < model::FORECAST_DEPTH; ++i)
  {
    nship->get_model()->apply(true);
    nship->update();
    for (auto& a : nasteroids)
      a->update();
    size_t collisions = collision_check(nship, nasteroids);
    if ( collisions != 0 )
      return std::make_pair(i, collisions);
    collision_count += collision_check(nship, nasteroids);
  }
  return std::make_pair(0, 0);
}
// Проверка пути, что нет коллизий
// Чем больше число, тем хуже
inline size_t course_forecast(/*ship::ptr real_ship,*/ ship::ptr fship, std::vector<asteroid::ptr>& asteroids, const model::ship::move_list_t& in)
{
  size_t collision_count = 0;
  ship::ptr nship = fship->clone();
  nship->get_model()->set_course(in);
  std::vector<asteroid::ptr> nasteroids;
  nasteroids.reserve(asteroids.size() );
  for (auto a : asteroids)
    nasteroids.push_back(a->clone());
  
  while ( nship->get_model()->apply(true) )
  {
    nship->update();
    for (auto& a : nasteroids)
      a->update();
    collision_count += collision_check(/*real_ship,*/ nship, nasteroids);
  }
  
  return collision_count;
}

inline size_t bruteforce_courses(/*ship::ptr real_ship,*/ ship::ptr fship, std::vector<asteroid::ptr>& asteroids, model::ship::move_list_t& in, size_t pos)
{
  size_t collision_count = 999999;
  model::ship::move_list_t res = in;
  
  if ( pos == in.size() )
    return course_forecast(/*real_ship,*/ fship, asteroids, in);
  
  model::ship::move_list_t nin = in;
  static const auto rl1=std::make_pair(model::ship::THRUST|model::ship::ROTATE_LEFT,1);
  static const auto rl2=std::make_pair(model::ship::THRUST|model::ship::ROTATE_LEFT,2);
  static const auto rl3=std::make_pair(model::ship::THRUST|model::ship::ROTATE_LEFT,3);
  static const auto rr1=std::make_pair(model::ship::THRUST|model::ship::ROTATE_RIGHT,1);
  static const auto rr2=std::make_pair(model::ship::THRUST|model::ship::ROTATE_RIGHT,2);
  static const auto rr3=std::make_pair(model::ship::THRUST|model::ship::ROTATE_RIGHT,3);
  static const auto t3=std::make_pair(model::ship::THRUST,3);
  static const auto t4=std::make_pair(model::ship::THRUST,4);
  static const auto t5=std::make_pair(model::ship::THRUST,5);
  static const auto b3=std::make_pair(model::ship::BREAKING,3);
  static const auto b4=std::make_pair(model::ship::BREAKING,4);
  static const auto b5=std::make_pair(model::ship::BREAKING,5);
  //typedef std::vector<size_t> state_list_t
  static const std::vector< std::vector<std::pair<size_t,size_t> > > states_set({
    {{0, 10}},
    {b5, {0, 5}},
    {rl1, t3, {0, 6}},
    {rr1, t3, {0, 6}},
    {t3, {0, 6}},
    
  });
  //for (size_t state = 0; state < model::ship::SHOOTING; ++state)
  for (const auto& states : states_set )
  {
    size_t curpos = pos;
    nin.clear();
    for (const auto& state : states )
    {
      for (size_t i =0; i < state.second; ++i)
      {
        if ( curpos < in.size() )
        {
          //nin[curpos++]=state.first;
          nin.push_back(state.first);
        }
      }
    }
    
    size_t cur = course_forecast(/*real_ship,*/ fship, asteroids, nin);
    std::cout << "Вариант: " << cur << std::endl;
    if ( cur < collision_count )
    {
      collision_count = cur;
      res = nin;
    }    
    /*if ( in[pos] == state)
      continue;
    nin[pos] = static_cast<model::ship::states>(state);
    if ( bruteforce_courses(fship, asteroids, nin, pos+1) )
      */
    /*size_t cur = bruteforce_courses(real_ship, fship, asteroids, nin, curpos);
    if ( cur > curlevel )
    {
      std::cout << "Вариант: " << cur << std::endl;
      curlevel = cur;
      res = nin;
    }*/
    /*if ( bruteforce_courses(real_ship, fship, asteroids, nin, curpos) )
    {
      in = nin;
      return true;
    }*/
  }
  in = res;
  return collision_count;
}

inline bool gen_course(model::ship::move_list_t* course, size_t pos)
{
  static constexpr size_t l=model::ship::ROTATE_LEFT;
  static constexpr size_t r=model::ship::ROTATE_RIGHT;
 
  static constexpr size_t tl=model::ship::THRUST|model::ship::ROTATE_LEFT;
  static constexpr size_t tr=model::ship::THRUST|model::ship::ROTATE_RIGHT;
  static constexpr size_t bl=model::ship::BREAKING|model::ship::ROTATE_LEFT;
  static constexpr size_t br=model::ship::BREAKING|model::ship::ROTATE_RIGHT;
  static constexpr size_t t=model::ship::THRUST;
  static constexpr size_t b=model::ship::BREAKING;
  static constexpr size_t n=0;

      
  
  
  // STAT 1:7 3:6 8:6 13:2 18:1 20:8 
  static const std::vector< model::ship::move_list_t > courses = {
    {b,b,b},  // 65 122
    // право
    {tr},     // 35 81
    {tr, t},  // 14 5
    //{tr, t, t},  
    {br, t},  // 2 2
   // {br, t, t},  // 2 2
   // {tr, b, b}, // 1 41
    {r, b},   // 1 2
    // лево
    {tl},     // 36 46
    {tl, t},  // 1 3
   // {tl, t, t},
    {bl, t},  // 1 1
   // {bl, t, t},  // 1 1
   // {tl, b, b}, // 2 3
    {l, b}, 
    // прямо
    {t,t,t},  // 7 3
    
    // пробники
    {b,b,b,b}, // 65 122
    {b,b,b,b,b,b}, // 65 122
    {t,t,t,t}, // 65 122
    {t,t,t,t,t,t}, // 65 122
    
    {br,br},
    {br,br,br,br}, // 
    {bl,bl},
    {bl,bl,bl,bl}, // 
    
    // не рабочие
    
    {l, b}, 
    {l, b, b},
    {l, b, b, b},  
    {r, b, b},
    {r, b, b, b},
    {tr, b, b, b},  
    {tl, b, b, b},
    
  
    
  };
  
  if ( pos >= courses.size() )
    return false;
  
  *course = courses[pos];
  return true;
}


std::map<size_t, size_t> stat;

inline void calculate_moves_forward(model::ship& s, model::battle::ptr battle)
{
  /*if ( s.get_course().size() > model::PREDICTION_DEPTH - 2 )
    return;*/
  std::cout << "calculate_moves_forward" << std::endl;
  model::ship::move_list_t moves = s.get_course();
  moves.clear();
  moves.resize(model::PREDICTION_DEPTH);
  model::ship::radar_list_t radar = s.get_radar();
  
  // Корабль для прогнозирования 
  ::controller::ship::ptr fship = std::make_shared< ::controller::ship>(std::make_shared<model::ship>(), battle);
  *(fship ->get_model()) = s;
  ship::ptr sship = fship->clone();
  
  // Астероиды для прогнозирования 
  std::vector<asteroid::ptr> asteroids;
  for (auto a: radar)
  {
    asteroids.push_back(std::make_shared<asteroid>(std::make_shared<model::asteroid>(), battle));
    static_cast<model::entity&>(*(asteroids.back()->get_model())) = a;
  }
  
  
  auto collision_count = course_forecast(fship, asteroids);
  
  
  if ( collision_count.second > 0 )
  {
    auto best_collision = collision_count;
    model::ship::move_list_t best_course = fship->get_model()->get_course();
    std::cout << "Столкновений ожидается: " << collision_count.second << " через " << collision_count.first << std::endl;
    size_t m = 0; size_t best_m = 0;
    model::ship::move_list_t course;
    while (gen_course(&course, m++))
    {
      fship = sship->clone();
      fship->get_model()->set_course(course);
      auto cur_collision = course_forecast(fship, asteroids);
      std::cout << "Прогноз для N" << m -1 << " Столкновений ожидается: " << cur_collision.second << " через " << cur_collision.first << std::endl;
      
      
      
      bool the_best = 
        cur_collision.second == 0 
        || cur_collision.first > best_collision.first 
        || (cur_collision.first == best_collision.first && cur_collision.second < best_collision.second );

      if ( the_best && best_collision.second != 0 /*Пока собираем стату*/ )
      {
        best_collision = cur_collision;
        best_course = course;
        best_m = m - 1;
      }

      /*if ( cur_collision.first >= best_collision.first || cur_collision.second==0 )
      {
        if ( cur_collision.second <= best_collision.second )
        {
          best_collision = cur_collision;
          best_course = course;
          best_m = m - 1;
        }
      }*/
      
      /*if ( best_collision.second == 0)
        break;*/
    }
    std::cout << "Лучший прогноз для N" << best_m << " Столкновений ожидается: " << best_collision.second << " через " << best_collision.first << std::endl;
    if ( collision_count.first >= best_collision.first && best_collision.second!=0)
      std::cout << "Скорее всего ты труп" << std::endl;
    else
      ++stat[best_m];
    s.set_course(course);
    
    std::cout << "STAT ";
    for ( auto ss: stat )
    { 
      std::cout << ss.first << ":" << ss.second << " ";
    }
    std::cout << std::endl;
  }
  /*else
    s.set_course({model::ship::BREAKING});*/
    
    
    
  
  
  /*
  size_t level = bruteforce_courses(sship, fship, asteroids, moves, 0);
  std::cout << "МАРШРУТ " << level << std::endl;
  for ( auto m : moves)
    std::cout << m << " ";
  std::cout << "МАРШРУТ " << std::endl;
  
  s.set_course(moves);*/
  /*
  if ( bruteforce_courses(fship, asteroids, moves, 0) )
  {
    std::cout << "ЕСТЬ МАРШРУТ" << std::endl;
    s.set_course(moves);
  }
  else
  {
    std::cout << "Кранты" << std::endl;
  }*/
}

  
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
    std::cout << " --------- Battle --------- " << std::endl;
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
      
      const auto& c = _player->get_model()->get_course();
      if ( !c.empty() ) 
        std::cout << c.front() << std::endl;
      _player->get_model()->apply(_auto);
      
      _player->get_model()->set_radar({});
      model::entity e = static_cast<model::entity>(*_player->get_model());
      model::position p = e.get_position();
      model::position p1 = p; p1.x = 0;
      model::position p2 = p; p2.y = 0;
      model::position p3 = p; p3.x = _model->space_size().x;
      model::position p4 = p; p4.y = _model->space_size().y;
      
/*      e.set_position(p1);
      _player->get_model()->add_radar(e);
      
      e.set_position(p2);
      _player->get_model()->add_radar(e);

      e.set_position(p3);
      _player->get_model()->add_radar(e);

      e.set_position(p4);
      _player->get_model()->add_radar(e);*/

    }

    // Столкновения
    /*for (auto itr_ship = _controlled_ship.begin(); itr_ship != _controlled_ship.end(); ++itr_ship)
    {
      (*itr_ship)->get_model()->set_radar({});
    }*/
    
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
        //for (auto itr_ship = _controlled_ship.begin(); itr_ship != _controlled_ship.end(); ++itr_ship)
        if ( _player!=nullptr )
        {
          _player->get_model()->add_radar( *(*beg)->get_model() );
          
          if ( _player->is_collide(**beg) )
          {
            auto p1 = _player->get_model()->get_position();
            auto p2 = (*beg)->get_model()->get_position();
            this->create_explosion(p2);
            this->create_explosion(p1)->get_model()->set_type(1);
            _player->get_model()->kill(true);
            (*beg)->get_model()->kill(true);
            exit(1);
          }
          else if (_auto )
          {
            long forecast =  _player->collision_forecast(**beg, model::FORECAST_DEPTH);
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
        /*
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
        */

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

      
      if ( _player!=nullptr) 
      {
        calculate_moves_forward(*_player->get_model(), _model);
        
      }
      
      
      // TODO сделать рекурсивную функцию глубиины N преебрать все варианты действий
      /*
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
      */
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
    auto newp = p;
    newp.r = 10;
    for (int i=0; i < count; ++i)
    {
      auto a = asteroid::create(p, _model);
      a->get_model()->set_size(0);
      //a->get_model()->set_radius(10);
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
    p.r = 5;
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


  void rotate_left(bool value) { if (_player!=nullptr) _player->rotate_left(value); _auto &= !value;}
  void rotate_right(bool value) { if (_player!=nullptr) _player->rotate_right(value); _auto &= !value;}
  void thrust(bool value) { if (_player!=nullptr) _player->thrust(value); _auto &= !value;}
  void breaking(bool value) { if (_player!=nullptr) _player->breaking(value); _auto &= !value;}
  void shuting(bool value) { if (_player!=nullptr) _player->shuting(value); _auto &= !value;}


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

