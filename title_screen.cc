#include "title_screen.h"

#include <algorithm>

#include "util.h"

TitleScreen::TitleScreen() : background_("title.png"), text_("text.png"), rng_(Util::random_seed()) {}

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

  return !input.any_pressed();
}

void TitleScreen::draw(Graphics& graphics) const {
  background_.draw(graphics);
  text_.draw(graphics, "Press any key", graphics.width() / 2, graphics.height() - 32, Text::Alignment::Center);

  for (const auto& p : pieces_) {
    p.p.draw(graphics, p.x, p.y);
  }
}

Screen* TitleScreen::next_screen() const {
  return nullptr;
}
