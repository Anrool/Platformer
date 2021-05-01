#ifndef BUTTONVIEW_H
#define BUTTONVIEW_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <list>

namespace sf {
class Event;
}

class Button;

class ButtonView : public sf::Drawable, private sf::NonCopyable {
public:
  using ButtonList = std::list<Button>;

  explicit ButtonView(ButtonList &&buttons);

  void handleEvent(const sf::Event &event);

  Button &getSelectedButton() const;

private:
  ButtonList mButtons;
  ButtonList::iterator mCurrentButton;

  bool mMouseOnButton;

  bool hasChosenButton() const;

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};

#endif // BUTTONVIEW_H
