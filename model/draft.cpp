#include <time.h>
#include <list>
#include <cmath>
#include <chrono>

#include <memory>
#include <functional>
#include <iostream>
#include <sstream>

using namespace std::chrono;


namespace model{




struct ibullet: ientity
{
  typedef std::shared_ptr<ibullet> ptr;
  virtual ~ibullet() {}
  // Определяет текстуру
  virtual size_t type() const = 0;
};


// Описание оружия
struct iweapon
{
  typedef std::shared_ptr<iweapon> ptr;
  virtual ~iweapon() {}
};

struct iship: ientity
{
public:
  // Определяет текстуру
  virtual size_t type() const = 0;
  // Неуязвимый
  virtual bool is_unbreakable() const = 0;
  // разгоняеться ( 0 нет, степень ускорения )
  virtual bool is_thrust() const = 0;
  // Тормозит ( 0 нет, степень ускорения )
  virtual bool is_breaking() const = 0;
  // ведет стрельбу ( 0 нет, степень ускорения )
  virtual bool is_shooting() const = 0;

  virtual iweapon::ptr get_weapon() const = 0;
};

// Взрыв
class iexplosion: ientity
{
public:
  // Определяет текстуру
  virtual size_t type() const = 0;
}




class bullet: public entity_t<ibullet>
{
public:
  typedef std::shared_ptr<bullet> ptr;

  bullet(const imodel::ptr& m)
    : entity_t<ibullet>(m)
  {}
  /*
  bullet(const position& pos,int radius=1)
    : entity(pos, radius)
  {}

  bullet(const position& pos, const position& delta, int radius=1)
    : entity(pos, radius)
    , start_dx(delta.x)
    , start_dy(delta.y)
  {}*/

  virtual void update() override
  {
    position p = this->get_position();
    position d = this->get_delta();

    d.x=cos(p.a*DEGTORAD)*6 + start_dx;
    d.y=sin(p.a*DEGTORAD)*6 + start_dy;
     // "Покачивание" пуль
     //angle+=rand()%7-3;  /*try this*/
     //angle+=rand()%20-10;  /*try this*/
    p.x+=d.x;
    p.y+=d.y;

     if ( p.x > W || p.x < 0 || p.y > H || p.y < 0 )
       this->kill(true);
     this->set_position(p);
   }

   // Убрать
   // float start_dx = 0;
   // float start_dy = 0;
};

class ship: public entity_t<iship>
{
public:
  typedef std::shared_ptr<ship> ptr;

  ship(const imodel::ptr& m)
    : entity_t<iship>(m)
  {}

  virtual void update() override
  {
    position p = this->get_position();
    position d = this->get_delta();

    if ( _thrust )
    {
      d.x+=cos(p.a*DEGTORAD)*0.2;
      d.y+=sin(p.a*DEGTORAD)*0.2;
    }
    else if ( _breaking )
    {
      /*d.x*=0.98;
      d.y*=0.98  ;*/
      d.x*=0.95;
      d.y*=0.95;
    }

    int maxSpeed=15;
    float speed = sqrt(d.x*d.x+d.y*d.y);
    if ( speed > maxSpeed )
    {
      d.x *= maxSpeed/speed;
      d.y *= maxSpeed/speed;
    }

    p.x += d.x;
    p.y += d.y;

    if ( p.x > W ) p.x=0;
    if ( p.x< 0 ) p.x=W;
    if ( p.y>H ) p.y=0;
    if ( p.y<0 ) p.y=H;
    this->set_position(p);
  }

  void fire(bool value)
  {
    _shooting = value;
  }

  bool is_fire() const
  {
    return _shooting;
  }

  void breaking(bool value)
  {
    _breaking = value;
  }

  virtual bool is_breaking() const override
  {
    return _breaking;
  }

  virtual bool is_unbreakable() const override
  {
    return _unbreakable;
  }

  /*
  virtual bool is_thrust() const = 0;
  // Тормозит ( 0 нет, степень ускорения )
  virtual bool is_breaking() const = 0;
  // ведет стрельбу ( 0 нет, степень ускорения )
  virtual bool is_shooting() const = 0;
  virtual bool is_unbreakable() const = 0;
  */
private:
  bool _thrust = false;
  bool _shooting = false;
  bool _breaking = false;
  bool _unbreakable = false;
};

class explosion
  : public entity_t<iexplosion>
{
public:
  explosion(const imodel::ptr& m)
    : entity_t<iexplosion>(m)
  {}
};

class engine
{
  typedef std::list<asteroid::ptr> asteroid_list;
  typedef std::list<bullet::ptr> bullet_list;
  typedef std::list<entity::ptr> explosion_list;

public:

  void initialize()
  {
    _death_time = high_resolution_clock::now();
     _last_fire = high_resolution_clock::now();
    //_player = this->create_player();
    for(int i=0;i<10;i++)
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

    this->update_player();
    this->update_bullets();
    this->update_asteroids();
    this->update_explosions();
    this->fire_bullets();
    this->collide_bullets();
    this->create_random_asteroid_or_not();
  }

  void update_player()
  {
    if ( _player!=nullptr)
      _player->update();
    else
    {
      auto now = high_resolution_clock::now();
      if ( 500 < duration_cast<std::chrono::milliseconds>(now - _death_time).count() )
      {
        _player = this->create_player();
      }
    }
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
    if ( _player!=nullptr &&  _player->is_fire() )
    {
      auto now = high_resolution_clock::now();
      if ( 500000 /*75000*/ /*35000*/ /*10000*/ < duration_cast<std::chrono::microseconds>(now - _last_fire).count() )
      {
        _last_fire = now;
        auto b = create_bullet();
        ++_fire_count;
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
      if ( _player!=nullptr && !collide_player && a->is_collide(*_player) )
      {
        collide_player = true;
        a->kill(true);
        _player->kill(true);
        auto b = create_explosion(a->get_position());
        _new_explosions.push_back(b);
        _explosions.push_back(b);
        _player.reset();
        _death_time = high_resolution_clock::now();
        //_player->set_position(position{W/2,H/2,0});
        //_player->set_delta(position{0,0,0});
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
            for(int i=0;i<0;i++)
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
    if ( /*rand()%150==0*/ false )
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
    return std::make_shared<player>( position{ W/2, H/2, 0},20 );
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

  size_t get_fire_count() const
  {
    return _fire_count;
  }
private:
  size_t _fire_count = 0;
  player::ptr _player;
  asteroid_list _asteroids;
  bullet_list _bullets;
  explosion_list _explosions;
  asteroid_list _new_asteroids;
  bullet_list _new_bullets;
  explosion_list _new_explosions;
  high_resolution_clock::time_point _last_fire;
  high_resolution_clock::time_point _death_time;
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

class text_proc
{
  typedef std::shared_ptr<sf::Text> text_ptr;

  typedef std::map<std::string, text_ptr> text_map;
  typedef std::map<std::string, std::wstring> pref_map;
public:
  text_proc()
  {
    _font.loadFromFile("images/DroidSans.ttf");//передаем нашему шрифту файл шрифта
  }

  void create(const std::string& name, const std::wstring& pref, float x, float y)
  {
    auto t = std::make_shared<sf::Text>("", _font);
    t->setPosition(x,y);
    _pref[name]=pref;
    _texts[name]=t;
  }

  void set(const std::string& name, size_t value)
  {
    std::wstringstream ss;
    ss << _pref[name] << value;
    _texts[name]->setString(ss.str());
  }

  void draw(sf::RenderWindow &app)
  {
    for (auto& t : _texts)
      app.draw(*t.second);
    /*
    _sprite.setPosition(pos.x, pos.y);
    _sprite.setRotation(pos.a+90);
    app.draw(_sprite);
    */
  }


private:
  sf::Font _font;
  text_map _texts;
  pref_map _pref;
};

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
    //t5.loadFromFile("images/fire_blue.png");
    t5.loadFromFile("images/fire_red.png");
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/explosions/type_B.png");

    text_proc text;
    text.create("gun", L"GUN: ", 20, 20);
    /*
    sf::Font font;//шрифт
    font.loadFromFile("images/DroidSans.ttf");//передаем нашему шрифту файл шрифта


    sf::Text textLevel("", font);
    textLevel.setPosition(20,20);

    sf::Text textLevel("", font);
    textLevel.setPosition(20,20);
      */

    t1.setSmooth(true);
    t2.setSmooth(true);
    //t1.setSrgb();

    sf::Sprite background(t2);

    auto sExplosion=std::make_shared<animator>(t3, 0,0,256,256, 48, 0.5);
    auto sRock=std::make_shared<animator>(t4, 0,0,64,64, 16, 0.2);
    auto sRock_small=std::make_shared<animator>(t6, 0,0,64,64, 16, 0.2);
    auto sBullet=std::make_shared<animator>(t5, 0,0,32,64, 16, 0.8);
    auto sPlayer    = std::make_shared<animator>(t1, 40,0,40,40, 1, 0);
    auto sPlayer_go = std::make_shared<animator>(t1, 40,40,40,40, 1, 0);
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

    player *p = nullptr;/*new player();
    p->settings(*sPlayer, engine.get_player() );
    entities.push_back(p);*/

    /////main loop/////
    while (app.isOpen())
    {
      if ( p == nullptr )
      {
        if (auto mp = engine.get_player() )
        {
          p = new player();
          sPlayer->set_alfa(50);
          p->settings(*sPlayer, engine.get_player() );
          entities.push_back(p);
          std::cout << "entities new player" << std::endl;
        }
      }


      sf::Event event;
      if (app.pollEvent(event))
      {
        if (event.type == sf::Event::Closed)
          app.close();
      }

      if  ( auto pm = engine.get_player() )
      {
        pm->fire( sf::Keyboard::isKeyPressed(sf::Keyboard::Space) );
        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) pm->rotate(3);
        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  pm->rotate(-3);
        pm->thrust(sf::Keyboard::isKeyPressed(sf::Keyboard::Up));
        pm->breaking(sf::Keyboard::isKeyPressed(sf::Keyboard::Down));
      }


      /////////////////////////////
      engine.update();
      /////////////////////////////

      while ( auto nb = engine.detach_new_bullet() )
      {
        bullet *b = new bullet();
        b->settings(*sBullet, nb);
        entities.push_back(b);
        std::cout << "entities new bullet" << std::endl;
      }

      std::cout << "entities " << entities.size() << std::endl;

      if ( p != nullptr )
      {
        if ( p->get_model()->is_thrust() )
          p->anim = *sPlayer_go;
        else
          p->anim = *sPlayer;
      }

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
          if ( e == p )
          {
            entity *ee = new entity();
            ee->settings(*sExplosion_ship, engine.create_explosion(p->get_model()->get_position() ) );
            ee->name="explosion";
            entities.push_back(ee);
            p = nullptr;
          }
          i=entities.erase(i);
          delete e;
        }
        else
          i++;
      }

      /*std::wstringstream ss;
      ss << L"\tLEVEL 1\tGUN: " << engine.get_fire_count() << L"\tCANNON: 0\t LIFE: 3"  << std::endl;
      text.setString(ss.str());*/
      text.set("gun", engine.get_fire_count());

      //////draw//////
      app.draw(background);
      for(auto i:entities) i->draw(app);
      text.draw(app);
      app.display();
    }

    return 0;
}
