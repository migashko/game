#pragma once
#include <model/types.hpp>
#include <model/imodel.hpp>
#include <memory>

namespace model{
  
template<typename I>
class entity_t: public I
{
public:
  virtual ~entity_t(){}

  entity_t(const imodel::ptr& m)
    : _model(m)
  { }

  virtual position get_position() const override final 
  {
    return _pos;
  }

  virtual float get_radius() const override final 
  {
    return _radius;
  }
  
  virtual bool is_life() const override final 
  {
    return _life;
  }
  
  position get_delta() const  
  {
    return _delta;
  }

  void rotate(float da)  
  {
    _pos.a += da;
  }

  
  void kill(bool value)  
  {
    _life = !value;
  }

  void set_delta( const position& p)  
  {
    _delta = p;
  }

  void set_position( const position& p)  
  {
    _pos = p;
  }

  template<typename T>
  bool is_collide(const entity_t<T>& other)
  {
    position a = this->get_position();
    float ar = this->get_radius();
    position b = other.get_position();
    float br = other.get_radius();
    return (b.x - a.x)*(b.x - a.x)+
           (b.y - a.y)*(b.y - a.y)<
           (ar + br)*(ar + br);
  }
  
  virtual void update() = 0;

  imodel::ptr get_model() const
  {
    return _model;
  }

protected:
  position _pos;
  position _delta;
  int _radius = 0;
  bool _life = true;
private:
  imodel::ptr _model;
};  
}

