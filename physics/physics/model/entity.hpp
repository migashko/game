#pragma once

namespace physics{ namespace model{

struct point
{
  float x = 0.0;
  float y = 0.0;
};

struct position: point
{
  float a = 0.0; // угол
  float r = 0.0; // Радиус
};

// Импульс
struct momentum
{
  
}

struct physics_object
{
  // положение 
  // размер 
};

}}
