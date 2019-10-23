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

class smart_pilot
{
public:
  typedef model::ship::move_list_t course_t;
  struct comb
  {
    int weight = 0;
    course_t course;
    bool operator < (const comb& other ) const
    {
      return weight > other.weight;
    }
  };

  struct course_status
  {
    // коичество столкновений на пути
    size_t collision_count = 0;
    // минимальный путь без столкновений
    size_t free_course = 0;

    operator bool () const
    {
      return collision_count==0 && free_course!=0;
    }
  };

  typedef std::vector<comb> comb_list_t;
public:
  typedef std::shared_ptr<smart_pilot> ptr;

  ~smart_pilot()
  {
    // выгрузть состояние
  }

  smart_pilot()
  {
    // загруить состояние

    // для 9 вариантов и 6 ходов = 531441 комбинаций
    _actions = {
      0,
      model::ship::THRUST,
      model::ship::BREAKING,
      model::ship::THRUST|model::ship::ROTATE_LEFT,
      model::ship::THRUST|model::ship::ROTATE_RIGHT,
      model::ship::BREAKING|model::ship::ROTATE_LEFT,
      model::ship::BREAKING|model::ship::ROTATE_RIGHT,
      model::ship::ROTATE_LEFT,
      model::ship::ROTATE_RIGHT
    };

  }

  void new_ship(const ship::ptr& ship)
  {
    _ship = ship;
    if ( ship != nullptr )
      _ghost_ship = ship->clone();
    else
      _ghost_ship = nullptr;
  }

  // Определяем маршрут движения
  bool chart_course()
  {
    if ( _ship == nullptr )
      return false;

    // Сначала проверим текущий курс
    const auto& orig_course = _ship->get_model()->get_course();
    course_status status = this->course_forecast(orig_course);
    if ( status )
    {
      std::cout << "Текущий маршрут (" << orig_course.size() << " действий) свободен!" << std::endl;
      return true;
    }

    std::cout << "На маршруте препятсвия! (" << orig_course.size() << " действий) " << status.collision_count
              << " свободный путь " << status.free_course << std::endl;

    course_t new_course;
    status = this->chart_exist_course(&new_course);
    if ( status )
    {
      std::cout << "Найден существующий маршрут (" << new_course.size() << " действий)!" << std::endl;
      _ship->get_model()->set_course(new_course);
      return true;
    }

    /*if (status.free_course > _actions.size() * 3)
      return true;*/

    std::cout << "Нужен новый маршрут! Генерируем... уже создано " << _comb_list.size() << std::endl;
    status = this->chart_brute_course(&new_course);
    if ( status )
    {
      std::cout << "Создан новый маршрут (" << new_course.size() << " действий)!" << std::endl;
      _ship->get_model()->set_course(new_course);
      return true;
    }
    else if (status.free_course < _actions.size() )
    {
      std::cout << "Фенита ля комедия " << std::endl;
      std::sort(_comb_list.begin(), _comb_list.end() );
      for (const auto& c: _comb_list)
      {
        std::cout << c.weight << ": ";
        for (const auto& i: c.course)
          std::cout << i << " ";
        std::cout << std::endl;
      }
      exit(0);
      return true;
    }
    return false;
  }

private:

  size_t collision_check()
  {
    size_t collision_count = 0;
    for (const auto& a: _asteroids)
    {
      if ( _ghost_ship->is_collide(*a) )
      {
        ++collision_count;
      }
    }
    return collision_count;
  }

  course_status course_forecast(const course_t& course)
  {
    this->create_ghost_ship(course);
    this->create_ghost_asteroids();
    course_status status;

    for (size_t i = 0 ; i < model::FORECAST_DEPTH; ++i)
    {
      _ghost_ship->get_model()->apply(true);
      _ghost_ship->update();
      for (auto& a : _asteroids)
        a->update();
      status.collision_count += this->collision_check();
      if ( status.collision_count == 0 )
        ++status.free_course;
    }

    return status;
  }

  course_status chart_exist_course(course_t* course)
  {
    course_status best_status;
    course->clear();
    const course_t* best_course = nullptr;
    for ( auto& c : _comb_list )
    {
      course_status cur_statis = this->course_forecast(c.course);
      if ( cur_statis.collision_count == 0 )
      {
        c.weight += 1; // Увеличиваем вес маршрута
        *course = c.course;
        return cur_statis;
      }

      bool the_best =
        cur_statis.collision_count == 0
        || cur_statis.collision_count < best_status.collision_count
        || (cur_statis.free_course > best_status.free_course && cur_statis.collision_count == best_status.collision_count );

      if ( the_best )
      {
        best_status = cur_statis;
        best_course = &(c.course);
      }
    }
    if ( best_course != nullptr)
      *course = *best_course;
    return best_status;
  }

  void create_ghost_ship(const model::ship::move_list_t& course)
  {
    _ghost_ship->set_model(*_ship->get_model());
    _ghost_ship->get_model()->set_course(course);
  }

  void create_ghost_asteroids()
  {
    const model::ship::radar_list_t& radar = _ship->get_model()->get_radar();
    if ( radar.size() < _asteroids.size() )
      _asteroids.resize(radar.size());
    else while ( _asteroids.size() < radar.size() )
    {
      _asteroids.resize( _asteroids.size() + 1);
      if ( _asteroids.back() == nullptr )
        _asteroids.back() = std::make_shared<asteroid>(std::make_shared<model::asteroid>(), _ship->get_model_battle());
    }

    for (size_t i = 0 ; i < radar.size(); ++i)
    {
      model::asteroid am;
      static_cast<model::entity&>(am) = radar[i];
      _asteroids[i]->set_model( am );
    }
  }

  void make_course(course_t* course, size_t pos)
  {
    // для 9 вариантов и 6 ходов = 531441 комбинаций
    course->clear();
    while ( pos >= _actions.size() )
    {
      course->push_back(_actions[pos%_actions.size()]);
      pos/=_actions.size();
    }
    course->push_back(_actions[pos]);
    std::reverse(course->begin(), course->end());
  }

  course_status chart_brute_course(course_t* course)
  {
    for (size_t i = 0; i < 531441; ++i)
    {
      if ( i % 10000 == 0 )
        std::cout << i << std::endl;
      make_course(course, i);
      if ( course_status status = this->course_forecast(*course) )
      {
        _comb_list.push_back(comb{1, *course});
        std::cout << "Создан новый курс " <<  i << " size " << course->size() << ": ";
        for ( size_t c : *course )
          std::cout << c << " ";
        std::cout << std::endl;
        return status;
      }
    }
    return course_status();
  }


private:
  ship::ptr _ship;
  // Текущие комбинации
  comb_list_t _comb_list;
  // Текущие астероиды (временная переменная)
  std::vector<asteroid::ptr> _asteroids;
  ship::ptr _ghost_ship;
  std::vector< size_t > _actions;
};

size_t collision_check(ship::ptr fship, std::vector<asteroid::ptr>& asteroids)
{
  size_t collision_count = 0;
  for (const auto& a: asteroids)
  {
    if ( fship->is_collide(*a) )
    {
      ++collision_count;
    }
  }
  return collision_count;
}

// Проверка пути, что нет коллизий
// Чем больше число, тем хуже
inline std::pair<size_t, size_t> course_forecast(ship::ptr fship, std::vector<asteroid::ptr>& asteroids)
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

 // STAT 0:146 1:45 2:114 3:8 5:1 14:1 16:1 17:1 18:3 19:3 20:4 21:6 22:3
  static const std::vector< model::ship::move_list_t > courses = {
    // Бутальные варианты
    {2,1},
    {5,1,2,1,2},
    {10,0},
    {9,9,9,9,6},
    {2,10,10,10,9,6},
    {2,6,5,6,5,10},
    {2,6,6,0},
    {2,2,2,10,9,10},
    {6,10},
    {10,6},
    {5,4,4,4,6},

    //
    {b},        //
    {t},        //
    {tl},       //
    {tr},       //
    {tl,t,t},   //
    {tr,t,t},   //
    // 6 : ...
    {b,b},      //
    {b,b,b},    //
    {tl,tl},    //
    {tl,tl,tl}, //
    {tr,tr},    //
    {tr,tr,tr}, //
    {t,t},      //
    {t,t,t},    //
    {l, t},     // 1 1 1 1
    {l, t, t, t},     // 1
    {r, t, t, t},     // 4 2 1
    {tl, l, l, t, t},  // 1 2 1 1 1   1
    {tr, r, r, t, t},   // 13 3
    {tl, l, l, tl, tl},  // 9 3 1
    {tr, r, r, tr, tr},   // 1 4
    {tl, l, l, l, tl, tl},   // 6 1 1
    {tr, r, r, r, tr, tr},   // 3
    {tl, tl, tl, tr, tr, tr, tl}, // 1 1
    {tr, tr, tr, tl, tl, tl, tr},// 2 1
    {tl, tl, tr, tr, tl, tl, tr}, // 1 1
    {tr, tr, tl, tl, tr, tr, tl},// 2
    {tr, tl, tr, tl, tr, tl, tr},
    {tr, tr, tr, tr, tr, tr, tr},
    {tl, tl, tl, tl, tl, tl, tl}
  };

  if ( pos >= courses.size() )
    return false;

  *course = courses[pos];
  return true;
}

inline bool brute_course(model::ship::move_list_t* course, size_t pos)
{
  // для 9 вариантов и 6 ходов = 531441 комбинаций
  static const std::vector< size_t > values = {
    0,
    model::ship::THRUST,
    model::ship::BREAKING,
    model::ship::THRUST|model::ship::ROTATE_LEFT,
    model::ship::THRUST|model::ship::ROTATE_RIGHT,
    model::ship::BREAKING|model::ship::ROTATE_LEFT,
    model::ship::BREAKING|model::ship::ROTATE_RIGHT,
    model::ship::ROTATE_LEFT,
    model::ship::ROTATE_RIGHT
  };

  course->clear();
  while ( pos >= values.size() )
  {
    course->push_back(values[pos%values.size()]);
    pos/=values.size();
  }
  course->push_back(values[pos]);
  std::reverse(course->begin(), course->end());
  return true;
}

std::map<size_t, size_t> stat;

inline void calculate_moves_forward(model::ship& s, model::battle::ptr battle)
{
  /*if ( s.get_course().size() > model::PREDICTION_DEPTH - 2 )
    return;*/
  /*std::cout << "calculate_moves_forward" << std::endl;*/
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
    std::cout << "---------------------" << std::endl;
    auto best_collision = collision_count;
    model::ship::move_list_t best_course = fship->get_model()->get_course();
    std::cout << "Столкновений ожидается: " << collision_count.second << " через " << collision_count.first << std::endl;
    size_t m = 0; size_t best_m = 0;
    model::ship::move_list_t course;
    while ( gen_course(&course, m++)  )
    {
      fship = sship->clone();
      fship->get_model()->set_course(course);
      auto cur_collision = course_forecast(fship, asteroids);
      std::cout << "Прогноз для N" << m -1 << " Столкновений ожидается: " << cur_collision.second << " через " << cur_collision.first << std::endl;
      bool the_best =
        cur_collision.second == 0
        || cur_collision.second < best_collision.second
        || (cur_collision.first > best_collision.first && cur_collision.second == best_collision.second );

      if ( the_best && best_collision.second != 0 /*Пока собираем стату*/ )
      {
        best_collision = cur_collision;
        best_course = course;
        best_m = m - 1;
      }
    }
    std::cout << "Лучший прогноз для N" << best_m << " Столкновений ожидается: " << best_collision.second << " через " << best_collision.first << std::endl;
    if ( collision_count.first >= best_collision.first && best_collision.second!=0)
      std::cout << "Скорее всего ты труп" << std::endl;
    else
      ++stat[best_m];
    s.set_course(best_course);

    std::cout << "STAT ";
    for ( auto ss: stat )
    {
      std::cout << ss.first << ":" << ss.second << " ";
    }
    std::cout << std::endl;

    if ( best_collision.second!=0 )
    {
      std::cout << "Включаем алгоритм полного перебора" << std::endl;
      for (int i = 0; i < 531441; ++i)
      {
        if ( brute_course(&course, i) )
        {
          std::cout << "Вариант " <<  i << " size " << course.size() << ": ";
          for ( size_t c : course )
            std::cout << c << " ";
          std::cout << std::endl;

          fship = sship->clone();
          fship->get_model()->set_course(course);
          auto cur_collision = course_forecast(fship, asteroids);

          if ( cur_collision.second == 0 )
          {
            std::cout << "Найден брутальный вариант!" << std::endl;
            for ( size_t c : course )
              std::cout << c << " ";
            std::cout << std::endl;
            exit(0);
            s.set_course(course);
          }
        }
        else
        {
          std::cout << "Ничего брутального не нашли!" << std::endl;
          break;
        }
      }
    }
  }
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
    _pilot = std::make_shared<smart_pilot>();
    _pilot->new_ship(_player);
  }

  void update()
  {
    //std::cout << " --------- Battle --------- " << std::endl;
    // Созание случайного астероида
    if ( rand() % 50 == 0)
    {
      this->create_asteroid();
    }

    // Созание выстрела
    if (_player!=nullptr)
    {
      /*
      if ( _auto && _player!=nullptr)
      {
        calculate_moves_forward(*_player->get_model(), _model);
      }*/

      /*const auto& c = _player->get_model()->get_course();
      if ( !c.empty() )
        std::cout << "APPLY "<< c.front() << std::endl;
      else
        std::cout << "APPLY EMPTY " << std::endl;*/
      if ( _auto )
        _pilot->chart_course();
      _player->get_model()->apply(_auto);
      _player->get_model()->set_radar({});
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
            _pilot->new_ship(nullptr);
            //exit(1);
          }
          /*else if (_auto )
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
          }*/
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
            bool dead1 = _player!=nullptr ? (*itr)->collision(**beg, *_player->get_model() ) : (*itr)->collision(**beg);
            bool dead2 = _player!=nullptr ? (*beg)->collision(**itr, *_player->get_model()) : (*beg)->collision(**itr);
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
  smart_pilot::ptr _pilot;

};

}

