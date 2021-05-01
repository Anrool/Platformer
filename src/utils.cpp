#define LOG_TAG "Utils"

#include "utils.h"
#include "animationType.h"
#include "core.h"
#include "inputManager.h"
#include "objectType.h"
#include "stateType.h"

#include <Box2D/Box2D.h>

#include <cmath>
#include <dirent.h>
#include <fstream>

#define DECLARE_CASE(base, label)                                              \
  case base::label:                                                            \
    return (#label)

UserData *getFixtureUserData(const b2Fixture *const fixture) {
  NOT_NULL(fixture);

  UserData *userData = static_cast<UserData *>(fixture->GetUserData());

  NOT_NULL(userData);

  return userData;
}

unsigned int strToUintSave(const std::string &str, size_t *const pos,
                           int base) {
  try {
    const auto res = std::stoi(str, pos, base);

    CHECK(res >= 0);
    CHECK(pos == nullptr || *pos > 0);

    return static_cast<unsigned int>(res);
  } catch (...) {
    CHECK(false);
  }
}

bool validateFile(const char *const pathToFile) {
  NOT_NULL(pathToFile);

  const std::ifstream input(pathToFile);

  return input.is_open();
}

float toMeters(float pixels) { return pixels / PIXELS_IN_METER; }

float toPixels(float meters) { return meters * PIXELS_IN_METER; }

float toDegree(float radian) { return radian * 180.f / b2_pi; }

float toRadian(float degree) { return degree * b2_pi / 180.f; }

void createTriangle(b2Vec2 (&vertices)[TRIANGLE_VERTEX_NUM], bool isSlopeLeft,
                    const b2Vec2 &size) {
  const auto offsetX = isSlopeLeft ? +size.x : -size.x;

  vertices[0] = {0.f, 0.f};
  vertices[1] = {0.f, -size.y};
  vertices[2] = {offsetX, -size.y};
}

void createRectangle(b2Vec2 (&vertices)[RECTANGLE_VERTEX_NUM],
                     const b2Vec2 &size) {
  const b2Vec2 halfSize{size.x / 2.f, size.y / 2.f};

  vertices[0] = {-halfSize.x, +halfSize.y};
  vertices[1] = {+halfSize.x, +halfSize.y};
  vertices[2] = {+halfSize.x, -halfSize.y};
  vertices[3] = {-halfSize.x, -halfSize.y};
}

std::list<std::string> listDirectory(const std::string &dirName) {
  CHECK(!dirName.empty());

  DIR *const dir = opendir(dirName.c_str());

  NOT_NULL(dir);

  std::list<std::string> filenames;
  const dirent *dirEntry = nullptr;

  while ((dirEntry = readdir(dir)) != nullptr) {
    if (dirEntry->d_name[0] != '.') {
      filenames.push_back(dirEntry->d_name);
    }
  }

  CHECK(closedir(dir) == 0);

  return filenames;
}

const char *toString(OBJECT_TYPE objectType) {
  switch (objectType) {
    DECLARE_CASE(OBJECT_TYPE, PLAYER);
    DECLARE_CASE(OBJECT_TYPE, RUNNER);
    DECLARE_CASE(OBJECT_TYPE, ARCHER);
    DECLARE_CASE(OBJECT_TYPE, ALLIED_BULLET);
    DECLARE_CASE(OBJECT_TYPE, ENEMY_BULLET);
    DECLARE_CASE(OBJECT_TYPE, EXPLODED_BULLET);
    DECLARE_CASE(OBJECT_TYPE, SOLID);
    DECLARE_CASE(OBJECT_TYPE, VERTICAL_PLATFORM);
    DECLARE_CASE(OBJECT_TYPE, HORIZONTAL_PLATFORM);
    DECLARE_CASE(OBJECT_TYPE, SLOPE_LEFT);
    DECLARE_CASE(OBJECT_TYPE, SLOPE_RIGHT);
    DECLARE_CASE(OBJECT_TYPE, SOLID_ON_LADDER);
    DECLARE_CASE(OBJECT_TYPE, LADDER);
    DECLARE_CASE(OBJECT_TYPE, HAZARD);
    DECLARE_CASE(OBJECT_TYPE, FINISH);
    DECLARE_CASE(OBJECT_TYPE, PLAYER_SENSOR);
    DECLARE_CASE(OBJECT_TYPE, RAVEN);
  default:
    return "UNKNOWN";
  }
}

const char *toString(ANIMATION_TYPE animationType) {
  switch (animationType) {
    DECLARE_CASE(ANIMATION_TYPE, STAND);
    DECLARE_CASE(ANIMATION_TYPE, WALK);
    DECLARE_CASE(ANIMATION_TYPE, JUMP);
    DECLARE_CASE(ANIMATION_TYPE, SHOOT);
    DECLARE_CASE(ANIMATION_TYPE, CRAWL);
    DECLARE_CASE(ANIMATION_TYPE, SPIN);
    DECLARE_CASE(ANIMATION_TYPE, HURT);
    DECLARE_CASE(ANIMATION_TYPE, DEAD);
    DECLARE_CASE(ANIMATION_TYPE, FALL);
    DECLARE_CASE(ANIMATION_TYPE, CLIMB);
    DECLARE_CASE(ANIMATION_TYPE, IDLE);
    DECLARE_CASE(ANIMATION_TYPE, RUNNER);
    DECLARE_CASE(ANIMATION_TYPE, ARCHER);
    DECLARE_CASE(ANIMATION_TYPE, ALLIED_BULLET);
    DECLARE_CASE(ANIMATION_TYPE, ENEMY_BULLET);
    DECLARE_CASE(ANIMATION_TYPE, EXPLODED_BULLET);
    DECLARE_CASE(ANIMATION_TYPE, PLATFORM);
    DECLARE_CASE(ANIMATION_TYPE, RAVEN_SPEAK);
    DECLARE_CASE(ANIMATION_TYPE, RAVEN_LOOK);
    DECLARE_CASE(ANIMATION_TYPE, RAVEN_TURN);
    DECLARE_CASE(ANIMATION_TYPE, RAVEN_FLY);
  default:
    return "UNKNOWN";
  }
}

const char *toString(STATE_TYPE stateType) {
  switch (stateType) {
    DECLARE_CASE(STATE_TYPE, INTRO);
    DECLARE_CASE(STATE_TYPE, MENU);
    DECLARE_CASE(STATE_TYPE, SETTINGS);
    DECLARE_CASE(STATE_TYPE, GAME);
    DECLARE_CASE(STATE_TYPE, PAUSE);
    DECLARE_CASE(STATE_TYPE, INTERMEDIATE);
    DECLARE_CASE(STATE_TYPE, GAME_OVER);
    DECLARE_CASE(STATE_TYPE, EXIT);
  default:
    return "UNKNOWN";
  }
}

#define DECLARE_KEY_TYPE_CASE(keyType)                                         \
  case KEY_TYPE::keyType:                                                      \
    return (#keyType);

const char *toString(KEY_TYPE keyType) {
  switch (keyType) {
    DECLARE_KEY_TYPE_CASE(RIGHT);
    DECLARE_KEY_TYPE_CASE(LEFT);
    DECLARE_KEY_TYPE_CASE(UP);
    DECLARE_KEY_TYPE_CASE(DOWN);
    DECLARE_KEY_TYPE_CASE(SHOOT);
  default:
    return "UNKNOWN";
  }
}

#define DECLARE_KEY_CASE(keyName)                                              \
  case sf::Keyboard::keyName:                                                  \
    return (#keyName)

const char *toString(sf::Keyboard::Key key) {
  switch (key) {
    DECLARE_KEY_CASE(Unknown);
    DECLARE_KEY_CASE(A);
    DECLARE_KEY_CASE(B);
    DECLARE_KEY_CASE(C);
    DECLARE_KEY_CASE(D);
    DECLARE_KEY_CASE(E);
    DECLARE_KEY_CASE(F);
    DECLARE_KEY_CASE(G);
    DECLARE_KEY_CASE(H);
    DECLARE_KEY_CASE(I);
    DECLARE_KEY_CASE(J);
    DECLARE_KEY_CASE(K);
    DECLARE_KEY_CASE(L);
    DECLARE_KEY_CASE(M);
    DECLARE_KEY_CASE(N);
    DECLARE_KEY_CASE(O);
    DECLARE_KEY_CASE(P);
    DECLARE_KEY_CASE(Q);
    DECLARE_KEY_CASE(R);
    DECLARE_KEY_CASE(S);
    DECLARE_KEY_CASE(T);
    DECLARE_KEY_CASE(U);
    DECLARE_KEY_CASE(V);
    DECLARE_KEY_CASE(W);
    DECLARE_KEY_CASE(X);
    DECLARE_KEY_CASE(Y);
    DECLARE_KEY_CASE(Z);
    DECLARE_KEY_CASE(Num0);
    DECLARE_KEY_CASE(Num1);
    DECLARE_KEY_CASE(Num2);
    DECLARE_KEY_CASE(Num3);
    DECLARE_KEY_CASE(Num4);
    DECLARE_KEY_CASE(Num5);
    DECLARE_KEY_CASE(Num6);
    DECLARE_KEY_CASE(Num7);
    DECLARE_KEY_CASE(Num8);
    DECLARE_KEY_CASE(Num9);
    DECLARE_KEY_CASE(Escape);
    DECLARE_KEY_CASE(LControl);
    DECLARE_KEY_CASE(LShift);
    DECLARE_KEY_CASE(LAlt);
    DECLARE_KEY_CASE(LSystem);
    DECLARE_KEY_CASE(RControl);
    DECLARE_KEY_CASE(RShift);
    DECLARE_KEY_CASE(RAlt);
    DECLARE_KEY_CASE(RSystem);
    DECLARE_KEY_CASE(Menu);
    DECLARE_KEY_CASE(LBracket);
    DECLARE_KEY_CASE(RBracket);
    DECLARE_KEY_CASE(SemiColon);
    DECLARE_KEY_CASE(Comma);
    DECLARE_KEY_CASE(Period);
    DECLARE_KEY_CASE(Quote);
    DECLARE_KEY_CASE(Slash);
    DECLARE_KEY_CASE(BackSlash);
    DECLARE_KEY_CASE(Tilde);
    DECLARE_KEY_CASE(Equal);
    DECLARE_KEY_CASE(Dash);
    DECLARE_KEY_CASE(Space);
    DECLARE_KEY_CASE(Return);
    DECLARE_KEY_CASE(BackSpace);
    DECLARE_KEY_CASE(Tab);
    DECLARE_KEY_CASE(PageUp);
    DECLARE_KEY_CASE(PageDown);
    DECLARE_KEY_CASE(End);
    DECLARE_KEY_CASE(Home);
    DECLARE_KEY_CASE(Insert);
    DECLARE_KEY_CASE(Delete);
    DECLARE_KEY_CASE(Add);
    DECLARE_KEY_CASE(Subtract);
    DECLARE_KEY_CASE(Multiply);
    DECLARE_KEY_CASE(Divide);
    DECLARE_KEY_CASE(Left);
    DECLARE_KEY_CASE(Right);
    DECLARE_KEY_CASE(Up);
    DECLARE_KEY_CASE(Down);
    DECLARE_KEY_CASE(Numpad0);
    DECLARE_KEY_CASE(Numpad1);
    DECLARE_KEY_CASE(Numpad2);
    DECLARE_KEY_CASE(Numpad3);
    DECLARE_KEY_CASE(Numpad4);
    DECLARE_KEY_CASE(Numpad5);
    DECLARE_KEY_CASE(Numpad6);
    DECLARE_KEY_CASE(Numpad7);
    DECLARE_KEY_CASE(Numpad8);
    DECLARE_KEY_CASE(Numpad9);
    DECLARE_KEY_CASE(F1);
    DECLARE_KEY_CASE(F2);
    DECLARE_KEY_CASE(F3);
    DECLARE_KEY_CASE(F4);
    DECLARE_KEY_CASE(F5);
    DECLARE_KEY_CASE(F6);
    DECLARE_KEY_CASE(F7);
    DECLARE_KEY_CASE(F8);
    DECLARE_KEY_CASE(F9);
    DECLARE_KEY_CASE(F10);
    DECLARE_KEY_CASE(F11);
    DECLARE_KEY_CASE(F12);
    DECLARE_KEY_CASE(F13);
    DECLARE_KEY_CASE(F14);
    DECLARE_KEY_CASE(F15);
    DECLARE_KEY_CASE(Pause);
    DECLARE_KEY_CASE(KeyCount);
  default:
    return "UNKNOWN";
  }
}
