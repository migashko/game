#pragma once
#include <model/types.hpp>
#include <model/asteroid.hpp>
#include <memory>
#include <list>

namespace model{

class battle

{
public:
  typedef std::shared_ptr<battle> ptr;
  typedef std::list<asteroid::ptr> asteroid_list_t;
  position space_size() const
  {
    return position{MAXW, MAXH, 0};
  }

  asteroid::ptr create_asteroid()
  {
    _asteroids.push_back( std::make_shared<asteroid>());
    _new_asteroids.push_back(_asteroids.back());
    return _asteroids.back();
  }

  asteroid::ptr detach_new_asteroid()
  {
    if ( _new_asteroids.empty() )
      return nullptr;
    auto a  = _new_asteroids.front();
    _new_asteroids.pop_front();
    return a;
  }

  const asteroid_list_t& get_asteroids() const
  {
    return _asteroids;
  }

private:
  asteroid_list_t _asteroids;
  asteroid_list_t _new_asteroids;
};


}

