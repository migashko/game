#pragma once

#include <controller/asteroid.hpp>
#include <model/battle.hpp>

namespace controller{

class battle
{
public:
  battle(model::battle::ptr m)
    : _model(m)
  {}

  void initialize()
  {
    _asteroid = std::make_shared<asteroid>(_model);
    for (int i = 0; i < 30; ++i)
      _asteroid->create_asteroid();
  }

  void update()
  {
    if ( rand() % 50 == 0)
      _asteroid->create_asteroid();
    _asteroid->update();
  }


private:
  model::battle::ptr _model;
  asteroid::ptr _asteroid;
};

}

