#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include <cmath>
#include <chrono>

#include <memory>

using namespace std::chrono;

const int W = 1200;
const int H = 800;

float DEGTORAD = 0.017453f;

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
   
  void draw(sf::RenderWindow &app, float x, float y, float angle)
  {
    _sprite.setPosition(x,y);
    _sprite.setRotation(angle+90);
    app.draw(_sprite);
  }
   
private:
   float _current_frame = 0.0;
   float _speed = 0.0;
   sf::Sprite _sprite;
   std::vector<sf::IntRect> _frames;

};


class Entity
{
   public:
   float x,y,dx,dy,R,angle;
   bool life = 1;
   std::string name;
   animator anim;

   Entity()
   {
     life=1;
   }

   void settings(const animator &a,int X,int Y,float Angle=0,int radius=1)
   {
     anim = a;
     x=X; y=Y;
     angle = Angle;
     R = radius;
   }

   virtual void update(){};

   void draw(sf::RenderWindow &app)
   {
     anim.draw(app, x, y, angle);
     
     sf::CircleShape circle(R);
     circle.setFillColor(sf::Color(255,0,0,170));
     circle.setPosition(x,y);
     circle.setOrigin(R,R);
     //app.draw(circle);
   }

   virtual ~Entity(){};
};


class asteroid: public Entity
{
   public:
   asteroid()
   {
     dx=rand()%8-4;
     dy=rand()%8-4;
     name="asteroid";
   }

   void update()
   {
     x+=dx;
     y+=dy;

     if (x>W) x=0;  if (x<0) x=W;
     if (y>H) y=0;  if (y<0) y=H;
   }

};


class bullet: public Entity
{
   public:
   bullet()
   {
     name="bullet";
   }

   void  update()
   {
     dx=cos(angle*DEGTORAD)*6 + start_dx;
     dy=sin(angle*DEGTORAD)*6 + start_dy;
     // "Покачивание" пуль
     //angle+=rand()%7-3;  /*try this*/
     //angle+=rand()%20-10;  /*try this*/
     x+=dx;
     y+=dy;

     if (x>W || x<0 || y>H || y<0) life=0;
   }
   
   float start_dx = 0;
   float start_dy = 0;
};


class player: public Entity
{
   public:
   bool thrust;

   player()
   {
     name="player";
   }

   void update()
   {
     if (thrust)
     {
        dx+=cos(angle*DEGTORAD)*0.2;
        dy+=sin(angle*DEGTORAD)*0.2; 
    }
    else
    {
      dx*=0.99;
      dy*=0.99; 
    }

    int maxSpeed=15;
    float speed = sqrt(dx*dx+dy*dy);
    if (speed>maxSpeed)
     { dx *= maxSpeed/speed;
       dy *= maxSpeed/speed; }

    x+=dx;
    y+=dy;

    if (x>W) x=0; if (x<0) x=W;
    if (y>H) y=0; if (y<0) y=H;
   }

};


bool isCollide(Entity *a,Entity *b)
{
  return (b->x - a->x)*(b->x - a->x)+
         (b->y - a->y)*(b->y - a->y)<
         (a->R + b->R)*(a->R + b->R);
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


    std::list<Entity*> entities;

    for(int i=0;i<15;i++)
    {
      asteroid *a = new asteroid();
      a->settings(*sRock, rand()%W, rand()%H, rand()%360, 25);
      entities.push_back(a);
    }

    player *p = new player();
    p->settings(*sPlayer,200,200,0,20);
    entities.push_back(p);

    auto gun_time = high_resolution_clock::now();
    
    /////main loop/////
    while (app.isOpen())
    {
      sf::Event event;
      if (app.pollEvent(event))
      {
        if (event.type == sf::Event::Closed)
          app.close();
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
      {
        auto now = high_resolution_clock::now();
        if ( /*75000*/ 35000 < duration_cast<std::chrono::microseconds>(now - gun_time).count() )
        {
          gun_time = now;
          bullet *b = new bullet();
          b->settings(*sBullet,p->x,p->y,p->angle,10);
          b->start_dx = p->dx;
          b->start_dy = p->dy;
          entities.push_back(b);
        }
      }

      if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) p->angle+=3;
      if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  p->angle-=3;
      if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) p->thrust=true;
      else p->thrust=false;

      for(auto a:entities)
      {
        for(auto b:entities)
        {
          if (a->name=="asteroid" && b->name=="bullet")
          {
            if ( isCollide(a,b) )
            {
              a->life=false;
              b->life=false;

              Entity *e = new Entity();
              e->settings(*sExplosion,a->x,a->y);
              e->name="explosion";
              entities.push_back(e);

              for(int i=0;i<2;i++)
              {
                if (a->R==15) continue;
                Entity *e = new asteroid();
                e->settings(*sRock_small,a->x,a->y,rand()%360,15);
                entities.push_back(e);
              }
            }
          }

          if (a->name=="player" && b->name=="asteroid")
          {
            if ( isCollide(a,b) )
            {
              b->life=false;

              Entity *e = new Entity();
              e->settings(*sExplosion_ship,a->x,a->y);
              e->name="explosion";
              entities.push_back(e);

              p->settings(*sPlayer,W/2,H/2,0,20);
              p->dx=0; p->dy=0;
            }
          }
        }
      }

      if (p->thrust)  p->anim = *sPlayer_go;
      else   p->anim = *sPlayer;

      for(auto e:entities)
      {
        if (e->name=="explosion")
          if (e->anim.isEnd()) e->life=0;
      }

      if (rand()%150==0)
      {
        asteroid *a = new asteroid();
        a->settings(*sRock, 0,rand()%H, rand()%360, 25);
        entities.push_back(a);
      }

      for(auto i=entities.begin();i!=entities.end();)
      {
        Entity *e = *i;
        e->update();
        e->anim.update();

        if (e->life==false) {i=entities.erase(i); delete e;}
        else i++;
      }

      //////draw//////
      app.draw(background);
      for(auto i:entities) i->draw(app);
      app.display();
    }
  
    return 0;
}
