#define LOG_TAG "ButtonView"

#include "buttonView.h"
#include "button.h"
#include "core.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

ButtonView::ButtonView(std::list<Button> &&buttons)
    : mButtons(), mCurrentButton(mButtons.end()), mMouseOnButton(false) {
  CHECK(!buttons.empty());

  mButtons = std::move(buttons);
  mCurrentButton = mButtons.begin();
  mCurrentButton->select();
}

void ButtonView::handleEvent(const sf::Event &event) {
  CHECK(hasChosenButton());

  if ((mMouseOnButton && event.type == sf::Event::MouseButtonPressed &&
       event.mouseButton.button == sf::Mouse::Left) ||
      (event.type == sf::Event::KeyPressed &&
       event.key.code == sf::Keyboard::Return)) {
    auto &currentButton = getSelectedButton();

    if (currentButton.isActivated()) {
      currentButton.deactivate();
    } else {
      currentButton.activate();
    }
  } else if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::W) {
      mCurrentButton->deselect();

      if (mCurrentButton == mButtons.cbegin()) {
        mCurrentButton = mButtons.end();
      }

      mCurrentButton--;
      mCurrentButton->select();
    } else if (event.key.code == sf::Keyboard::S) {
      mCurrentButton->deselect();
      mCurrentButton++;

      if (mCurrentButton == mButtons.cend()) {
        mCurrentButton = mButtons.begin();
      }

      mCurrentButton->select();
    }
  } else if (event.type == sf::Event::MouseMoved) {
    const sf::Vector2f mousePosition = {
        static_cast<float>(event.mouseMove.x),
        static_cast<float>(event.mouseMove.y),
    };

    const auto it =
        std::find_if(mButtons.begin(), mButtons.end(),
                     [&mousePosition](const ButtonList::value_type &button) {
                       return button.getBoundingRect().contains(mousePosition);
                     });

    mMouseOnButton = (it != mButtons.cend());

    if (mMouseOnButton && it != mCurrentButton) {
      mCurrentButton->deselect();
      mCurrentButton = it;
      mCurrentButton->select();
    }
  }
}

Button &ButtonView::getSelectedButton() const {
  CHECK(hasChosenButton());

  return *mCurrentButton;
}

bool ButtonView::hasChosenButton() const {
  return mCurrentButton != mButtons.cend();
}

void ButtonView::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  for (const auto &button : mButtons) {
    target.draw(button, states);
  }
}
