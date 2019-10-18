#pragma once
#include <memory>

namespace controller{

struct icontroller
{
  typedef std::shared_ptr<icontroller> ptr;
  virtual void update() = 0;
};


}

