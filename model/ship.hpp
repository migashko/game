#pragma once
#include <model/entity.hpp>
#include <memory>

namespace model{

class ship: public entity
{
public:

  enum states:size_t
  {
    BREAKING = 1,
    THRUST = 2,
    ROTATE_LEFT = 4,
    ROTATE_RIGHT = 8,
    SHOOTING = 16,
    UNBREAKABLE = 32
  };

  typedef std::shared_ptr<ship> ptr;

  // Определяет текстуру
  virtual void set_state(size_t value )
  {
    _state = value ;
  }

  // Определяет текстуру ()
  size_t get_state() const
  {
    return _state;
  }

  template<states s>
  constexpr bool has_state() const
  {
    return (_state & static_cast<size_t>(s)) != 0;
  }

  template<states s>
  void set_state(bool enable)
  {
    if ( enable )
      _state |= static_cast<size_t>(s);
    else
      _state &= ~static_cast<size_t>(s);
  }

private:
  // Состояние enum (разгон|торможение|стрельба|поворот)
  size_t _state = 0;
};

}


