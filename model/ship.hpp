#pragma once
#include <model/entity.hpp>
#include <model/types.hpp>
#include <memory>
#include <vector>
#include <deque>
#include <algorithm>

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
  
  /*typedef std::pair<states, position> one_move_t;*/
  typedef size_t one_move_t;
  typedef std::deque<one_move_t> move_list_t;
  typedef std::vector<entity> radar_list_t;
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
  
  const radar_list_t& get_radar() const
  {
    return _radar;
  }

  void set_radar(const std::vector<entity>& v) 
  {
    _radar = v;
  }
  void add_radar(const entity& p) 
  {
    auto itr = std::upper_bound(_radar.begin(), _radar.end(), p, [this](const entity& left, const entity& right){
      return distance( this->_pos, left.get_position()) < distance( this->_pos, right.get_position());
    });
    _radar.insert(itr, p);
    if ( _radar.size() > MAXRADAR )
      _radar.pop_back();
  }
  
  bool apply(bool automod)
  {
    if ( _moves.empty() )
      return false;
    
    if ( automod )
    {
      _state = _moves.front();
      //std::cout << "Apply " << _state << std::endl;
    }
    /*else
      std::cout << "Apply empty"  << std::endl;*/
    _moves.pop_front();
    return true;
  }
  
  const move_list_t& get_course() const
  {
    return _moves;
  }
  

  void set_course(const move_list_t& m) 
  {
    _moves = m;
  }

private:
  // Состояние enum (разгон|торможение|стрельба|поворот)
  size_t _state = 0;
  radar_list_t _radar;
  move_list_t _moves;
};

}


