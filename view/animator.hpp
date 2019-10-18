#pragma once
#include <SFML/Graphics.hpp>
#include <model/types.hpp>
#include <memory>
#include <list>


namespace view{

class animator
{
public:
  typedef std::shared_ptr<animator> ptr;

  animator()
  {

  }

  void initialize(const sf::Texture &t, int x, int y, int w, int h, int count, float Speed)
  {
    _current_frame = 0;
    _speed = Speed;

    for (int i=0;i<count;i++)
      _frames.push_back( sf::IntRect(x+i*w, y, w, h)  );

    _sprite.setTexture(t);
    _sprite.setOrigin(w/2,h/2);
    _sprite.setTextureRect(_frames[0]);
  }

  void set_alfa(uint8_t alpha)
  {
    auto c = _sprite.getColor();
    c.a = alpha;
    _sprite.setColor(c);
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

  void draw(sf::RenderWindow &app, const model::position& pos)
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

}

