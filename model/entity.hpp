#pragma once
#include <model/types.hpp>
//#include <model/imodel.hpp>
#include <memory>
#include <iostream>
#include <cmath>

namespace model{


class entity
{
public:

  position get_position() const
  {
    return _pos;
  }

  float get_radius() const
  {
    return _radius;
  }

  bool is_life() const
  {
    return _life;
  }

  position get_delta() const
  {
    return _delta;
  }

  void rotate(float da)
  {
    _pos.a += da;
  }

  void set_radius(float r)
  {
    _radius = r;
  }

  void kill(bool value)
  {
    _life = !value;
  }

  void set_delta( const position& p)
  {
    _delta = p;
  }

  void set_position( const position& p)
  {
    _pos = p;
  }

  bool is_collide(const entity& other)
  {
    position a = this->get_position();
    float ar = this->get_radius();
    position b = other.get_position();
    float br = other.get_radius();
    //return (b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y) < (ar + br)*(ar + br);
    return std::hypot(b.x - a.x, b.y - a.y) < ar + br;
  }


protected:
  position _pos;
  position _delta;
  int _radius = 0;
  bool _life = true;
/*private:
  imodel::ptr _model;*/
};
}

