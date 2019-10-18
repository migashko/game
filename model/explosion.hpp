#pragma once
#include <model/entity.hpp>
#include <memory>

namespace model{

class explosion: public entity
{
public:
  typedef std::shared_ptr<explosion> ptr;

  size_t get_type() const
  {
    return _type;
  }

  void set_type(size_t s)
  {
    _type = s;
  }

private:
  size_t _type = 0;
};

}

