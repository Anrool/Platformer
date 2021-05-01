#ifndef ANIMATION_H
#define ANIMATION_H

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include <memory>
#include <vector>

class Animation : private sf::NonCopyable {
private:
  using FrameArray = std::vector<sf::IntRect>;

public:
  explicit Animation(const std::shared_ptr<const FrameArray> &frames,
                     sf::Time frameDuration, bool loop);

  size_t getFrameCount() const;
  const sf::IntRect &getCurrentFrame() const;
  sf::Time getDuration() const;

  void setLoop(bool loop);
  bool isLooped() const;

  void start();
  void pause();
  void stop();
  bool isPlaying() const;

  void update(sf::Time dt);

private:
  std::shared_ptr<const FrameArray> mFrames;
  FrameArray::const_iterator mCurrentFrame;

  sf::Time mFrameDuration;
  sf::Time mTimeSincePrevFrame;

  bool mLoop;
  bool mPlay;

  // indicated whether start() was already called
  bool sailed() const;
};

#endif // ANIMATION_H
