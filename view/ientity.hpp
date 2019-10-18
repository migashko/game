#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

namespace view{

struct ientity
{
  typedef std::shared_ptr<ientity> ptr;
  virtual ~ientity() {}
  virtual void update() = 0;
  virtual void draw(sf::RenderWindow& app) = 0;
  virtual bool is_life() const = 0;
};

}

