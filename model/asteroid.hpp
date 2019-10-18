#pragma once
#include <model/entity_t.hpp>
#include <memory>

namespace model{

class asteroid: public entity
{
public:
  typedef std::shared_ptr<asteroid> ptr;

  virtual size_t size() const
  {
    return 1;
  }

  // Определяет текстуру
  virtual size_t type() const
  {
    return 0;
  }

};

}

