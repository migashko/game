#pragma once
#include <model/entity.hpp>
#include <memory>

namespace model{

class bullet: public entity
{
public:

  enum types:size_t
  {
    RED = 1,
    BLUE = 2,
    ROCKET = 4
  };

  typedef std::shared_ptr<bullet> ptr;

  void set_type(types value )
  {
    _type = value ;
  }

  // Определяет текстуру ()
  types get_type() const
  {
    return _type;
  }

private:
  // Состояние enum (разгон|торможение|стрельба|поворот)
  types _type = RED;
  float _speed = 1.0;
};

}
