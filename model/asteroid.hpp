#pragma once
#include <model/entity_t.hpp>
#include <model/iasteroid.hpp>
#include <memory>

namespace model{
  
class asteroid: public entity_t<iasteroid>
{
public:
  typedef std::shared_ptr<asteroid> ptr;

  asteroid(const imodel::ptr& m)
    : entity_t<iasteroid>(m)
  {
    this->set_delta(position{ float(rand()%8-4), float(rand()%8-4)});
  }

  virtual void update() override
  {
    position p = this->get_position();
    position d = this->get_delta();
    position max_size = this->get_model()->space_size();
    
    p.x+=d.x;
    p.y+=d.y;

    if ( p.x > max_size.x ) p.x = 0;
    if ( p.x < 0 ) p.x = max_size.x;
    if ( p.y > max_size.y ) p.y=0;
    if ( p.y < 0 ) p.y = max_size.y;
    
    this->set_position(p);
  }
};
  
}

