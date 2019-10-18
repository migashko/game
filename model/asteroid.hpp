#pragma once
#include <model/entity.hpp>
#include <memory>

namespace model{

class asteroid: public entity
{
public:
  typedef std::shared_ptr<asteroid> ptr;

  size_t get_size() const
  {
    return _size;
  }

  void set_size(size_t s)
  {
    _size = s;
  }

  // Определяет текстуру
  virtual size_t type() const
  {
    return 0;
  }

private:
  size_t _size = 1;
};

}

