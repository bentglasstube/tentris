#include "title_screen.h"

#include <algorithm>

#include "util.h"

#include "game_screen.h"
#include "stats_screen.h"

TitleScreen::TitleScreen() :
  background_("title.png"), text_("text.png"),
  stats_("content/stats.txt"),
  difficulty_({"RUSTY", "TRUSTY", "LUSTY"}, 1),
  music_({"FOLK", "FUNK", "FILO", "FEAR"}, 0),
  rng_(Util::random_seed()),
  spawn_timer_(500), choice_(0)
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

  if (input.key_pressed(Input::Button::Up) && choice_ > 0) --choice_;
  if (input.key_pressed(Input::Button::Down) && choice_ < 2) ++choice_;

  if (input.key_pressed(Input::Button::Left)) {
    if (choice_ == 0) difficulty_.prev();
    if (choice_ == 1) music_.prev();
  }

  if (input.key_pressed(Input::Button::Right)) {
    if (choice_ == 0) difficulty_.next();
    if (choice_ == 1) music_.next();
  }

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

  const int menu_x = graphics.width() / 2 - 80;
  const int pointer = 108 + 16 * choice_;

  text_.draw(graphics, "Difficulty: " + difficulty_.current(), menu_x, 108);
  text_.draw(graphics, "Music: " + music_.current(), menu_x, 124);
  text_.draw(graphics, "Statistics", menu_x, 140);
  text_.draw(graphics, ">", menu_x - 16, pointer);
}

Screen* TitleScreen::next_screen() const {
  if (choice_ == 2) {
    return new StatsScreen();
  } else {
    return new GameScreen(
        static_cast<GameScreen::Difficulty>(difficulty_.choice()),
        static_cast<GameScreen::Music>(music_.choice()));
  }
}
