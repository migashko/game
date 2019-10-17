#pragma once

namespace model{
  
// Максимально допустимые размеры поля 
constexpr int MAXW = 1200;
constexpr int MAXH = 800;
constexpr float DEGTORAD = 0.017453f;

struct position
{
  float x = 0.0;
  float y = 0.0;
  float a = 0.0; // угол
};

}
