#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <map>
#include <memory>

enum class HEADING { LEFT, RIGHT, NONE };

enum class ANIMATION_TYPE;
class Animation;

namespace sf {
class Time;
class Sprite;
} // namespace sf

class AnimationManager : public sf::Drawable, private sf::NonCopyable {
public:
  explicit AnimationManager(const std::string &textureName);
  ~AnimationManager() final;

  void addAnimation(ANIMATION_TYPE type, std::unique_ptr<Animation> animation);
  void removeAnimation(ANIMATION_TYPE type);
  size_t getAnimationCount() const;

  void chooseAnimation(ANIMATION_TYPE type, HEADING heading = HEADING::NONE);
  bool hasAnimationFor(ANIMATION_TYPE type) const;
  ANIMATION_TYPE getCurrentAnimationType() const;

  void setHeading(HEADING heading);
  HEADING getHeading() const;
  void changeHeading();

  const sf::IntRect &getCurrentFrame() const;
  sf::Time getDuration() const;

  void setLoop(bool loop) const;
  bool isLooped() const;

  void start() const;
  void pause() const;
  void stop() const;
  bool isPlaying() const;

  void update(sf::Time dt) const;

private:
  using AnimationMap =
      std::map<ANIMATION_TYPE, const std::unique_ptr<Animation>>;

  AnimationMap mAnimations;
  AnimationMap::const_iterator mCurrentAnimation;
  HEADING mHeading;

  std::unique_ptr<sf::Sprite> mSprite;

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;

  bool hasChosenAnimation() const;
};

#endif // ANIMATIONMANAGER_H
