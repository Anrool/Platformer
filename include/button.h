#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <functional>
#include <map>
#include <memory>

namespace sf {
class Sprite;
}

class Label;
class ComponentDefinition;

class Button : public sf::Drawable,
               public sf::Transformable,
               private sf::NonCopyable {
public:
  using OnClick = std::function<void()>;

  explicit Button(const ComponentDefinition &buttonDefinition);
  explicit Button(const ComponentDefinition &buttonDefinition,
                  const OnClick &onClick);
  ~Button() final;

  void setString(const std::string &textString, const std::string &fontName);
  void setString(const std::string &textString);
  void setFillColor(const sf::Color &color);
  void setCharacteSize(unsigned int size);

  void select();
  void deselect();
  bool isSelected() const;

  void activate();
  void deactivate();
  bool isActivated() const;

  sf::FloatRect getBoundingRect() const;

private:
  enum class BUTTON_TYPE {
    REGULAR,
    SELECTED,
    ACTIVATED,
    NONE,
  };

  static const std::map<BUTTON_TYPE, sf::IntRect> mBoundsMap;
  static const std::string mTextureName;
  static const std::string mActivationSound;

  BUTTON_TYPE mType;
  std::unique_ptr<Label> mLabel;
  std::unique_ptr<sf::Sprite> mSprite;
  OnClick mOnClick;

  void centerButtonOrigin();

  void changeButtonType(BUTTON_TYPE type);

  void drawBoundingRect(sf::RenderTarget &target,
                        sf::RenderStates states) const;
  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};

#endif // BUTTON_H
