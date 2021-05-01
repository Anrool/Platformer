#ifndef LABEL_H
#define LABEL_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <memory>

class ComponentDefinition;

namespace sf {
class Color;
class Text;
} // namespace sf

class Label : public sf::Drawable,
              public sf::Transformable,
              private sf::NonCopyable {
public:
  explicit Label(const ComponentDefinition &definition);
  ~Label() override;

  void setString(const std::string &textString, const std::string &fontName);
  void setString(const std::string &textString);
  void setFillColor(const sf::Color &color);
  void setCharacteSize(unsigned int size);

private:
  std::unique_ptr<sf::Text> mText;

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};

#endif // LABEL_H
