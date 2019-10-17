#pragma once
#include <model/types.hpp>
#include <memory>

namespace model{
  
struct ientity
{
  virtual ~ientity() {}
  typedef std::shared_ptr<ientity> ptr;
  virtual position get_position() const = 0;
  virtual float get_radius() const = 0;
  virtual bool is_life() const= 0;
};
  
}

