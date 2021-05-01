#define LOG_TAG "Animation"

#include "animation.h"
#include "core.h"

Animation::Animation(const std::shared_ptr<const FrameArray> &frames,
                     sf::Time frameDuration, bool loop)
    : mFrames(), mCurrentFrame(), mFrameDuration(sf::Time::Zero),
      mTimeSincePrevFrame(sf::Time::Zero), mLoop(false), mPlay(false) {
  NOT_NULL(frames);
  CHECK(!frames->empty());
  CHECK(frameDuration > sf::Time::Zero);

  mFrames = frames;
  mCurrentFrame = frames->cend();
  mFrameDuration = frameDuration;

  setLoop(loop);
}

size_t Animation::getFrameCount() const { return mFrames->size(); }

const sf::IntRect &Animation::getCurrentFrame() const {
  CHECK(sailed());

  return *mCurrentFrame;
}

sf::Time Animation::getDuration() const {
  return mFrameDuration * static_cast<float>(mFrames->size());
}

void Animation::setLoop(bool loop) { mLoop = loop; }

bool Animation::isLooped() const { return mLoop; }

void Animation::start() {
  // reset the current frame only for the first start
  if (!sailed()) {
    mCurrentFrame = mFrames->cbegin();
  }

  mPlay = true;
}

void Animation::pause() {
  CHECK(sailed());

  mPlay = false;
}

void Animation::stop() {
  CHECK(sailed());

  mCurrentFrame = mFrames->cbegin();
  mTimeSincePrevFrame = sf::Time::Zero;
  mPlay = false;
}

bool Animation::isPlaying() const { return mPlay; }

void Animation::update(sf::Time dt) {
  CHECK(sailed());

  // do not update while pause
  if (!isPlaying()) {
    return;
  }

  mTimeSincePrevFrame += dt;

  while (mTimeSincePrevFrame > mFrameDuration) {
    mTimeSincePrevFrame -= mFrameDuration;
    const auto nextFrame = mCurrentFrame + 1;

    if (nextFrame < mFrames->cend()) {
      mCurrentFrame = nextFrame;
    } else if (isLooped()) {
      mCurrentFrame = mFrames->cbegin();
    } else {
      mTimeSincePrevFrame = sf::Time::Zero;
      mPlay = false;
    }
  }
}

bool Animation::sailed() const { return mCurrentFrame != mFrames->cend(); }
