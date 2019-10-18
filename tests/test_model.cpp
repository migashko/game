#include <model/interfaces.hpp>
#include <model/asteroid.hpp>
#include <model/battle.hpp>
#include <controller/asteroid.hpp>
#include <controller/battle.hpp>

int main()
{
  auto m = std::make_shared<model::battle>();
  auto c = std::make_shared<controller::battle>(m);
  auto a = c->create_asteroid();
  model::position p1 = a->get_model()->get_position();
  c->update();
  model::position p2 = a->get_model()->get_position();
  return p1.x==p2.x && p1.y==p2.y;
}
