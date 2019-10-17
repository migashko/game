#pragma once
#include <model/types.hpp>
#include <memory>

namespace model{
  
struct imodel
{
  typedef std::shared_ptr<imodel> ptr;
  virtual ~imodel() {}
  virtual position space_size() const = 0;
};
  
}

