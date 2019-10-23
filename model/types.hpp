#pragma once
#include <cstddef>
#include <cmath>

namespace model{

// Максимально допустимые размеры поля
constexpr int MAXW = 1200;
constexpr int MAXH = 800;
constexpr float DEGTORAD = 0.017453f;
// Объектов на радаре
constexpr int MAXRADAR = 30;
// Глубина прогноза для каждого объекта
constexpr int FORECAST_DEPTH = 50;

// Проработка ходов для каждого корабля
constexpr int PREDICTION_DEPTH = /*100*/10;
constexpr int PREDICTION_REP = /*100*/5;

struct position
{
  float x = 0.0;
  float y = 0.0;
  float a = 0.0; // угол
  float r = 0.0; // Радиус
};

inline size_t distance( const position& left, const position& right)
{
  return std::hypot(right.x - left.x, right.y - left.y);
}

inline float angle( const position& left, const position& right)
{
  return std::atan2(left.y - right.y, left.x - right.x)/DEGTORAD;
}

inline position middle( const position& left, const position& right)
{
  position res;
  res.x = (left.x + right.x)/2;
  res.y = (left.y + right.y)/2;
  res.a = (left.a + right.a)/2;
  return res;
}

inline position rotate(const position& p,  float da)
{
  position res = p;
  res.a += da;
  if ( res.a > 360 ) res.a -= 360;
  if ( res.a < 0 ) res.a += 360;
  return res;
}


}
