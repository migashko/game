#pragma once
#include <model/ientity.hpp>
#include <memory>

namespace model{
  
struct iasteroid: ientity
{
  typedef std::shared_ptr<iasteroid> ptr;

  virtual ~iasteroid() {}
  
  // 0 - мелкий
  // 1 - стандарт
  // 2 - большой
  // 3 - гиганский
  // 4 - монструозный
  virtual size_t size() const = 0;
  
  // Определяет текстуру 
  virtual size_t type() const = 0;
};
  
}

