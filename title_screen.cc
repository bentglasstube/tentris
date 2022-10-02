#include "title_screen.h"

#include <algorithm>

#include "util.h"

#include "game_screen.h"
#include "stats_screen.h"

TitleScreen::TitleScreen() :
  background_("title.png"), text_("text.png"),
  stats_("content/stats.txt"),
  rng_(Util::random_seed()),
  spawn_timer_(500), choice_(1)
{}

bool TitleScreen::update(const Input& input, Audio&, unsigned int elapsed) {
  for (auto& p : pieces_) {
    --p.x;
  }

  pieces_.erase(
      std::remove_if(
        pieces_.begin(), pieces_.end(),
        [](const auto& p) { return p.x < -100; }),
      pieces_.end());

  spawn_timer_ -= elapsed;
  if (spawn_timer_ < 0 && pieces_.size() < 10) {
    spawn_timer_ = std::uniform_int_distribution<int>(500, 1500)(rng_);
    pieces_.emplace_back(
        std::uniform_int_distribution<int>(0, 6)(rng_),
        std::uniform_int_distribution<int>(260, 360)(rng_),
        std::uniform_int_distribution<int>(10, 24)(rng_) * 8);
  }

  if (input.key_pressed(Input::Button::Up)) choice_ = (choice_ + 3) % 4;
  if (input.key_pressed(Input::Button::Down)) choice_ = (choice_ + 1) % 4;

  if (input.key_pressed(Input::Button::Start)) return false;
  if (input.key_pressed(Input::Button::A)) return false;
  if (input.key_pressed(Input::Button::X)) return false;

  return true;
}

void TitleScreen::draw(Graphics& graphics) const {
  background_.draw(graphics);

  for (const auto& p : pieces_) {
    p.p.draw(graphics, p.x, p.y);
  }

  const int center = graphics.width() / 2;
  const int pointer = 108 + 16 * choice_ + (choice_ == 3 ? 16 : 0);

  text_.draw(graphics, "Rusty", center, 108, Text::Alignment::Center);
  text_.draw(graphics, "Trusty", center, 124, Text::Alignment::Center);
  text_.draw(graphics, "Lusty", center, 140, Text::Alignment::Center);
  text_.draw(graphics, "Statistics", center, 172, Text::Alignment::Center);
  text_.draw(graphics, ">              <", center, pointer, Text::Alignment::Center);
}

Screen* TitleScreen::next_screen() const {
  if (choice_ < 3) {
    return new GameScreen(static_cast<GameScreen::Difficulty>(choice_));
  } else {
    return new StatsScreen();
  }
}
