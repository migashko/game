#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include <cmath>
#include <chrono>

#include <memory>
#include <functional>
#include <iostream>

using namespace std::chrono;

const int W = 1200;
const int H = 800;

float DEGTORAD = 0.017453f;

struct position
{
  float x = 0.0;
  float y = 0.0;
  float a = 0.0; // угол
};


class animator
{
public:
  typedef std::shared_ptr<animator> ptr;
  animator(){}
  animator(sf::Texture &t, int x, int y, int w, int h, int count, float Speed)
  {
    _current_frame = 0;
    _speed = Speed;

    for (int i=0;i<count;i++)
      _frames.push_back( sf::IntRect(x+i*w, y, w, h)  );

    _sprite.setTexture(t);
    _sprite.setOrigin(w/2,h/2);
    _sprite.setTextureRect(_frames[0]);
  }

  void update()
  {
    _current_frame += _speed;
    int n = _frames.size();
    if ( _current_frame >= n )
      _current_frame -= n;

    if ( n>0 )
      _sprite.setTextureRect( _frames[ static_cast<int>(_current_frame)] );
  }

  bool isEnd()
  {
    return _current_frame + _speed >= _frames.size();
  }

  void draw(sf::RenderWindow &app, const position& pos)
  {
    _sprite.setPosition(pos.x, pos.y);
    _sprite.setRotation(pos.a+90);
    app.draw(_sprite);
  }

private:
   float _current_frame = 0.0;
   float _speed = 0.0;
   sf::Sprite _sprite;
   std::vector<sf::IntRect> _frames;

};


namespace model{

class imodel
{
public:
  virtual ~imodel() {}
  virtual void update() = 0;
  virtual position get_position() const = 0;
  virtual void set_position( const position& p) = 0;
  virtual position get_delta() const = 0;
  virtual float get_radius() const = 0;
  virtual void rotate(float dr) = 0;
  virtual void thrust(bool value) = 0;
  virtual bool is_thrust() = 0;
  virtual void set_delta( const position& p) = 0;
  virtual bool is_life() const= 0;

  virtual void kill(bool value)= 0;
};

class entity: public imodel
{
public:
  typedef std::shared_ptr<entity> ptr;
  virtual ~entity(){}

  entity(const position& pos,int radius=1)
    : _pos(pos)
    , _radius(radius)
  { }

  virtual position get_position() const
  {
    return _pos;
  }

  virtual position get_delta() const
  {
    return _delta;
  }

  virtual float get_radius() const
  {
    return _radius;
  }

  virtual void rotate(float da)
  {
    _pos.a += da;
  }

  virtual void thrust(bool value)
  {
    _thrust = value;
  }

  virtual bool is_thrust()
  {
    return _thrust;
  }

  virtual bool is_life() const
  {
    return _life;
  }

  virtual void kill(bool value)
  {
    _life = !value;
  }

  virtual void set_delta( const position& p)
  {
    _delta = p;
  }

  virtual void set_position( const position& p)
  {
    _pos = p;
  }

  virtual void update() override
  {
  }

  bool is_collide(const entity& other)
  {
    position a = this->get_position();
    float ar = this->get_radius();
    position b = other.get_position();
    float br = other.get_radius();
    return (b.x - a.x)*(b.x - a.x)+
           (b.y - a.y)*(b.y - a.y)<
           (ar + br)*(ar + br);
  }

protected:
  position _pos;
  position _delta;
  int _radius = 0;
  bool _life = true;
  bool _thrust = false;
};

class asteroid: public entity
{
public:
  typedef std::shared_ptr<asteroid> ptr;

  asteroid(const position& pos,int radius=1)
    : entity(pos, radius)
  {
    _delta.x=rand()%8-4;
    _delta.y=rand()%8-4;
  }

  virtual void update() override
  {
    _pos.x+=_delta.x;
    _pos.y+=_delta.y;

    if (_pos.x>W) _pos.x=0;  if ( _pos.x<0 ) _pos.x = W;
    if (_pos.y>H) _pos.y=0;  if ( _pos.y<0 ) _pos.y = H;
  }
};

class bullet: public entity
{
public:
  typedef std::shared_ptr<bullet> ptr;
  bullet(const position& pos,int radius=1)
    : entity(pos, radius)
  {}

  bullet(const position& pos, const position& delta, int radius=1)
    : entity(pos, radius)
    , start_dx(delta.x)
    , start_dy(delta.y)
  {}

  virtual void update() override
  {
    _delta.x=cos(_pos.a*DEGTORAD)*6 + start_dx;
    _delta.y=sin(_pos.a*DEGTORAD)*6 + start_dy;
     // "Покачивание" пуль
     //angle+=rand()%7-3;  /*try this*/
     //angle+=rand()%20-10;  /*try this*/
    _pos.x+=_delta.x;
    _pos.y+=_delta.y;

     if ( _pos.x>W || _pos.x<0 || _pos.y>H || _pos.y<0 )
       _life=false;
   }

   float start_dx = 0;
   float start_dy = 0;
};

class player: public entity
{
public:
  typedef std::shared_ptr<player> ptr;

  player(const position& pos,int radius=1)
    : entity(pos, radius)
  {}

  virtual void update() override
  {
    if ( _thrust )
    {
      _delta.x+=cos(_pos.a*DEGTORAD)*0.2;
      _delta.y+=sin(_pos.a*DEGTORAD)*0.2;
    }
    else
    {
      /*_delta.x*=0.99;
      _delta.y*=0.99;*/
    }

    int maxSpeed=15;
    float speed = sqrt(_delta.x*_delta.x+_delta.y*_delta.y);
    if ( speed > maxSpeed )
    {
      _delta.x *= maxSpeed/speed;
      _delta.y *= maxSpeed/speed;
    }

    _pos.x += _delta.x;
    _pos.y += _delta.y;

    if ( _pos.x > W ) _pos.x=0;
    if ( _pos.x< 0 ) _pos.x=W;
    if ( _pos.y>H ) _pos.y=0;
    if ( _pos.y<0 ) _pos.y=H;
   }

  void fire(bool value)
  {
    _fire = value;
  }

  bool is_fire() const
  {
    return _fire;
  }

  bool _fire = false;
};

class engine
{
  typedef std::list<asteroid::ptr> asteroid_list;
  typedef std::list<bullet::ptr> bullet_list;
  typedef std::list<entity::ptr> explosion_list;

public:

  void initialize()
  {
     _last_fire = high_resolution_clock::now();
    _player = this->create_player();
    for(int i=0;i<15;i++)
    {
      auto a = create_asteroid();
      _new_asteroids.push_back(a);
      _asteroids.push_back(a);
    }
  }

  void update()
  {
    std::cout << "asteroids: " << _asteroids.size() << " " << _new_asteroids.size() << std::endl;
    std::cout << "bullets: " << _bullets.size() << " " << _new_bullets.size() << std::endl;
    _player->update();
    this->update_bullets();
    this->update_asteroids();
    this->update_explosions();
    this->fire_bullets();
    this->collide_bullets();
    this->create_random_asteroid_or_not();
  }

  void update_bullets()
  {
    auto itr = _bullets.begin();
    for ( ; itr != _bullets.end() ; )
    {
      (*itr)->update();
      if ( !(*itr)->is_life() )
        _bullets.erase(itr++);
      else
        ++itr;
    }
  }

  void update_asteroids()
  {
    auto itr = _asteroids.begin();
    for ( ; itr != _asteroids.end() ; )
    {
      (*itr)->update();
      if ( !(*itr)->is_life() )
        _asteroids.erase(itr++);
      else
        ++itr;
    }
  }

  void update_explosions()
  {
    auto itr = _explosions.begin();
    for ( ; itr != _explosions.end() ; )
    {
      (*itr)->update();
      if ( !(*itr)->is_life() )
        _explosions.erase(itr++);
      else
        ++itr;
    }
  }

  void fire_bullets()
  {
    if ( _player->is_fire() )
    {
      auto now = high_resolution_clock::now();
      if ( /*75000*/ /*35000*/ 10000 < duration_cast<std::chrono::microseconds>(now - _last_fire).count() )
      {
        _last_fire = now;
        auto b = create_bullet();
        _new_bullets.push_back(b);
        _bullets.push_back(b);
      }
    }
  }

  void collide_bullets()
  {
    bool collide_player = false;
    for (const auto& a : _asteroids )
    {
      if ( !collide_player && a->is_collide(*_player) )
      {
        collide_player = true;
        a->kill(true);
        auto b = create_explosion(a->get_position());
        _new_explosions.push_back(b);
        _explosions.push_back(b);
        _player->set_position(position{W/2,H/2,0});
        _player->set_delta(position{0,0,0});
      }

      for (const auto& b : _bullets)
      {
        if ( a->is_collide(*b) )
        {
          a->kill(true);
          b->kill(true);
          auto b = create_explosion(a->get_position());
          _new_explosions.push_back(b);
          _explosions.push_back(b);

          if ( a->get_radius()==25 )
          {
            for(int i=0;i<8;i++)
            {
              auto sa = create_small_asteroid(a->get_position());
              _new_asteroids.push_back(sa);
              _asteroids.push_back(sa);
            }
          }

        }
      }
    }
  }


  void create_random_asteroid_or_not()
  {
    if ( rand()%/*1*/50==0 )
    {
      auto a = create_asteroid();
      _new_asteroids.push_back(a);
      _asteroids.push_back(a);
    }
  }


  asteroid::ptr detach_new_asteroid()
  {
    if ( _new_asteroids.empty() )
      return nullptr;
    asteroid::ptr a = _new_asteroids.front();
    _new_asteroids.pop_front();
    return a;
  }

  bullet::ptr detach_new_bullet()
  {
    if ( _new_bullets.empty() )
      return nullptr;
    bullet::ptr a = _new_bullets.front();
    _new_bullets.pop_front();
    return a;
  }

  entity::ptr detach_new_explosions()
  {
    if ( _new_explosions.empty() )
      return nullptr;
    entity::ptr a = _new_explosions.front();
    _new_explosions.pop_front();
    return a;
  }

  player::ptr get_player()
  {
    return _player;
  }

  static player::ptr create_player()
  {
    return std::make_shared<player>( position{200,200,0},20 );
  }

  static asteroid::ptr create_asteroid()
  {
    return std::make_shared<asteroid>( position{ 0, float(rand()%H), float(rand()%360)}, 25);
  }

  static asteroid::ptr create_small_asteroid(const position& pos)
  {
    return std::make_shared<asteroid>( position{ pos.x, pos.y, float(rand()%360)}, 15);
  }

  // TODO: сделать класс explosion
  static entity::ptr create_explosion(const position& pos)
  {
    return std::make_shared<entity>( pos, 0);
  }


  bullet::ptr create_bullet()
  {
    if ( _player == nullptr )
      return nullptr;
    return std::make_shared<bullet>( _player->get_position(), _player->get_delta(), 25);
  }

private:
  player::ptr _player;
  asteroid_list _asteroids;
  bullet_list _bullets;
  explosion_list _explosions;
  asteroid_list _new_asteroids;
  bullet_list _new_bullets;
  explosion_list _new_explosions;
  high_resolution_clock::time_point _last_fire;
};

}

class entity
{
public:
  typedef std::shared_ptr<entity> ptr;
  //float x,y,dx,dy,R,angle;
  std::string name;
  animator anim;

  entity()
  {
  }

  void settings(const animator &a, std::shared_ptr<model::imodel> m)
  {
    anim = a;
    _model = m;
  }

   virtual void update()
   {
      _model->update();
    }

   void draw(sf::RenderWindow &app)
   {
     anim.draw(app, _model->get_position());
      /*
     sf::CircleShape circle(R);
     circle.setFillColor(sf::Color(255,0,0,170));
     circle.setPosition(x,y);
     circle.setOrigin(R,R);
     //app.draw(circle);
      */
   }

   virtual ~entity(){};

  std::shared_ptr<model::imodel> get_model()
  {
    return _model;
  }
private:
  std::shared_ptr<model::imodel> _model;
};


class asteroid: public entity
{
public:
   asteroid()
   {
     name="asteroid";
   }
};


class bullet: public entity
{
public:
   bullet()
   {
     name="bullet";
   }
};


class player: public entity
{
public:
   player()
   {
     name="player";
   }
};


bool isCollide(entity *ma, entity *mb)
{
  position a = ma->get_model()->get_position();
  float ar = ma->get_model()->get_radius();
  position b = mb->get_model()->get_position();
  float br = mb->get_model()->get_radius();

  return (b.x - a.x)*(b.x - a.x)+
         (b.y - a.y)*(b.y - a.y)<
         (ar + br)*(ar + br);
}


int main()
{
    srand(time(0));

    sf::RenderWindow app(sf::VideoMode(W, H), "Asteroids!");
    app.setFramerateLimit(60);

    sf::Texture t1,t2,t3,t4,t5,t6,t7;
    t1.loadFromFile("images/spaceship.png");
    t2.loadFromFile("images/background.jpg");
    t3.loadFromFile("images/explosions/type_C.png");
    t4.loadFromFile("images/rock.png");
    t5.loadFromFile("images/fire_blue.png");
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/explosions/type_B.png");

    t1.setSmooth(true);
    t2.setSmooth(true);

    sf::Sprite background(t2);

    auto sExplosion=std::make_shared<animator>(t3, 0,0,256,256, 48, 0.5);
    auto sRock=std::make_shared<animator>(t4, 0,0,64,64, 16, 0.2);
    auto sRock_small=std::make_shared<animator>(t6, 0,0,64,64, 16, 0.2);
    auto sBullet=std::make_shared<animator>(t5, 0,0,32,64, 16, 0.8);
    auto sPlayer=std::make_shared<animator>(t1, 40,0,40,40, 1, 0);
    auto sPlayer_go=std::make_shared<animator>(t1, 40,40,40,40, 1, 0);
    auto sExplosion_ship=std::make_shared<animator>(t7, 0,0,192,192, 64, 0.5);


    std::list<entity*> entities;

    model::engine engine;
    engine.initialize();

    while ( auto am = engine.detach_new_asteroid() )
    {
      asteroid *a = new asteroid();
      a->settings(*sRock, am );
      entities.push_back(a);
    }

    player *p = new player();
    p->settings(*sPlayer, engine.get_player() );
    entities.push_back(p);

    /////main loop/////
    while (app.isOpen())
    {
      sf::Event event;
      if (app.pollEvent(event))
      {
        if (event.type == sf::Event::Closed)
          app.close();
      }

      engine.get_player()->fire( sf::Keyboard::isKeyPressed(sf::Keyboard::Space) );

      if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) p->get_model()->rotate(3);
      if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  p->get_model()->rotate(-3);
      if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) p->get_model()->thrust(true);
      else p->get_model()->thrust(false);

      /////////////////////////////
      engine.update();
      /////////////////////////////

      while ( auto nb = engine.detach_new_bullet() )
      {
        bullet *b = new bullet();
        b->settings(*sBullet, nb);
        entities.push_back(b);
      }

      std::cout << "entities " << entities.size() << std::endl;

      if (p->get_model()->is_thrust() )  p->anim = *sPlayer_go;
      else   p->anim = *sPlayer;

      for(auto e:entities)
      {
        if (e->name=="explosion")
          if (e->anim.isEnd()) e->get_model()->kill(true);
      }

      while ( auto am = engine.detach_new_asteroid() )
      {
        asteroid *a = new asteroid();
        if ( am->get_radius() > 15 )
          a->settings(*sRock, am );
        else
          a->settings(*sRock_small, am );
        entities.push_back(a);
      }

      while ( auto em = engine.detach_new_explosions() )
      {
        entity *e = new entity();
        e->settings(*sExplosion, em );
        e->name="explosion";
        entities.push_back(e);
      }

      for(auto i=entities.begin();i!=entities.end();)
      {
        entity *e = *i;
        e->anim.update();

        if ( !e->get_model()->is_life() )
        {
          i=entities.erase(i);
          delete e;
        }
        else
          i++;
      }

      //////draw//////
      app.draw(background);
      for(auto i:entities) i->draw(app);
      app.display();
    }

    return 0;
}
