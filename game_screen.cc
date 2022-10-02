#include "game_screen.h"

#include <algorithm>
#include <iostream>

GameScreen::GameScreen(Difficulty difficulty, Music music) :
  background_("background.png"),
  blocks_("blocks.png", 19, 8, 8),
  digits_("digits.png", 10, 12, 21),
  laser_("scanner.png", 0, 0, 80, 4),
  top_("top.png", 0, 0, 96, 16),
  text_("text.png"),
  state_(State::Playing),
  stats_("content/stats.txt"),
  difficulty_(difficulty),
  music_(music),
  rng_(Util::random_seed()),
  duration_(0),
  lines_(0), level_(1), score_(0),
  scan_timer_(10000), scanner_(-1)
{
  board_.fill(0);
  fill_bag();
  spawn_piece();
}

bool GameScreen::update(const Input& input, Audio& audio, unsigned int elapsed) {
  if (state_ == State::GameOver) {
    if (input.key_pressed(Input::Button::Start)) {
      audio.music_volume(10);
      return false;
    }
    return true;
  }

  if (state_ == State::Paused) {
    if (input.key_pressed(Input::Button::Start)) {
      state_ = State::Playing;
      audio.music_volume(10);
    }
    return true;
  }

  if (input.key_pressed(Input::Button::Start)) {
    audio.music_volume(2);
    state_ = State::Paused;
    return true;
  }

  if (input.key_pressed(Input::Button::A)) rotate_left();
  if (input.key_pressed(Input::Button::X)) rotate_left();
  if (input.key_pressed(Input::Button::B)) rotate_right();
  if (input.key_pressed(Input::Button::Y)) rotate_right();

  if (input.key_pressed(Input::Button::Left)) test_move(-1, 0);
  if (input.key_pressed(Input::Button::Right)) test_move(1, 0);

  if (input.key_held(Input::Button::Left)) {
    auto_shift_ += elapsed;
    while (auto_shift_ > kAutoShiftDelay) {
      test_move(-1, 0);
      auto_shift_ -= kAutoShiftRepeat;
    }
  } else if (input.key_held(Input::Button::Right)) {
    auto_shift_ += elapsed;
    while (auto_shift_ > kAutoShiftDelay) {
      test_move(1, 0);
      auto_shift_ -= kAutoShiftRepeat;
    }
  } else {
    auto_shift_ = 0;
  }

  duration_ += elapsed;
  current_.drop += elapsed;

  if (input.key_pressed(Input::Button::Up)) {
    const int dist = hard_drop(audio);
    add_points(2 * dist);
  }

  if (input.key_pressed(Input::Button::Down)) {
    current_.drop = std::min(current_.drop, kSoftDropTime);
  }

  const int drop_target = input.key_held(Input::Button::Down) ? kSoftDropTime : drop_time();
  while (current_.drop > drop_target) {
    if (test_move(0, -1)) {
      tspin_ = false;
      current_.drop -= drop_target;
      if (input.key_held(Input::Button::Down)) ++soft_drop_;
    } else {
      lock_piece(audio);
      // if a freshly spawned piece overlaps the board, you lose
      if (overlap(current_)) {
        game_over(audio);
        return true;
      }
    }
  }

  scan_timer_ -= elapsed;
  if (scan_timer_ < 0) {
    audio.play_sample("laser.wav");
    scan_timer_ += 10000;
    scanner_ = 161;
    scanner_drop_timer_ = 0;
  }

  if (scanner_ >= 0) {
    scanner_drop_timer_ -= 20 * elapsed;
    while (scanner_drop_timer_ < 0) {
      scanner_drop_timer_ += drop_time();
      --scanner_;
      if (check_line(scanner_ / 8) && scanner_ % 8 < 7) {
        const int time_left = (scanner_drop_timer_ + drop_time() * (scanner_ % 8)) / 20;
        stats_.set_edge(time_left);
        std::cerr << "Edged with " << time_left << "ms to go." << std::endl;
        audio.play_sample("edge.wav");
        add_points(level_ * 1000 / time_left);
        floaters_.emplace_back(7, 58, 183 - scanner_);
      }
    }

    if (scanner_ <= 0) {
      // Clear any found lines
      int lines = 0;
      for (int y = 20; y >= 0; --y) {
        if (value(0, y) == 2) {
          drop_lines(y);
          ++lines;
        }
      }

      scanner_ = -1;

      // If no lines are found, there is a penalty
      if (lines == 0) {
        switch (difficulty_) {
          case Difficulty::Rusty:
            audio.play_sample("warning.wav");
            hard_drop(audio);
            break;

          case Difficulty::Trusty:
            audio.play_sample("warning.wav");
            add_trash_line();
            break;

          case Difficulty::Lusty:
            game_over(audio);
            return true;
        }
      } else {
        audio.play_sample(lines > 3 ? "bigclear.wav" : "clear.wav");

        add_points(level_ * 100 * std::pow(2, lines - 1));
        lines_ += lines;
        level_ = (lines_ / 10) + 1;

        stats_.set_clear(lines);
        floaters_.emplace_back(lines > 3 ? 3 : lines - 1, 58, 193);
      }

      if (board_empty()) {
        std::cerr << "Full clear bonus" << std::endl;
        add_points(level_ * 1000);
        floaters_.emplace_back(6, 58, 193);
      }
    }
  }

  for (auto& f : floaters_) {
    f.update(elapsed);
  }

  floaters_.erase(
      std::remove_if(
        floaters_.begin(), floaters_.end(),
        [](const auto& f) { return f.done(); }),
      floaters_.end());

  return true;
}

void GameScreen::draw(Graphics& graphics) const {
  background_.draw(graphics);

  if (state_ == State::Paused) {
    text_.draw(graphics, "PAUSED", 80, 112, Text::Alignment::Center);
  } else {
    current_.piece.draw(graphics, 40 + 8 * current_.x, 193 - 8 * current_.y, block_texture());

    const Piece next = bag_.back();
    switch (next.shape()) {
      case Piece::Shape::I:
        next.draw(graphics, 176, 60, block_texture());
        break;

      case Piece::Shape::O:
        next.draw(graphics, 176, 64, block_texture());
        break;

      default:
        next.draw(graphics, 180, 64, block_texture());
        break;
    }

    for (int y = 0; y < 20; ++y) {
      for (int x = 0; x < 10; ++x) {
        if (filled(x, y)) {
          blocks_.draw(graphics, board_[y * 10 + x] + 19 * block_texture(), 40 + 8 * x, 193 - 8 * y);
        }
      }
    }

    if (scanner_ >= 0) {
      laser_.draw(graphics, 40, 198 - scanner_);
    }

    if (state_ == State::GameOver) text_.draw(graphics, "GAME OVER", 80, 112, Text::Alignment::Center);
    top_.draw(graphics, 32, 24);

    for (const auto& f : floaters_) {
      f.draw(graphics);
    }
  }

  text_.draw(graphics, "NEXT", 176, 32);
  text_.draw(graphics, "LINES", 160, 128);
  text_.draw(graphics, "LEVEL", 160, 144);
  text_.draw(graphics, "SCORE", 160, 176);

  text_.draw(graphics, std::to_string(lines_), 224, 128, Text::Alignment::Right);
  text_.draw(graphics, std::to_string(level_), 224, 144, Text::Alignment::Right);
  text_.draw(graphics, std::to_string(score_), 224, 192, Text::Alignment::Right);
}

std::string GameScreen::get_music_track() const {
  switch (music_) {
    case Music::Folk: return "folk.ogg";
    case Music::Funk: return "funk.ogg";
    case Music::Filo: return "filo.ogg";
    case Music::Fear: return "fear.ogg";
    default:          return "";
  }
}

bool GameScreen::overlap(const PieceData& piece) const {
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 4; ++x) {
      if (piece.piece.block(x, y) && filled(piece.x + x, piece.y - y)) return true;
    }
  }
  return false;
}

int GameScreen::value(int x, int y) const {
  if (x < 0 || x > 9) return 1;
  if (y < 0) return 1;
  if (y > 21) return 0;

  return board_[y * 10 + x];
}

bool GameScreen::board_empty() const {
  for (int y = 0; y < 22; ++y) {
    for (int x = 0; x < 10; ++x) {
      if (filled(x, y)) return false;
    }
  }
  return true;
}

void GameScreen::fill(int x, int y, int value) {
  if (x < 0 || x > 9) return;
  if (y < 0 || y > 21) return;

  board_[y * 10 + x] = value;
}

void GameScreen::spawn_piece() {
  current_ = { drop_time(), 3, 21, bag_.back() };
  bag_.pop_back();
  if (bag_.empty()) fill_bag();
  soft_drop_ = 0;
}

void GameScreen::fill_bag() {
  for (int i = 0; i < 7; ++i) {
    bag_.push_back(static_cast<Piece::Shape>(i));
  }
  shuffle(bag_.begin(), bag_.end(), rng_);
}

void GameScreen::lock_piece(Audio& audio) {
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 4; ++x) {
      if (current_.piece.block(x, y)) {
        fill(current_.x + x, current_.y - y, current_.piece.color());
      }
    }
  }

  if (current_.piece.shape() == Piece::Shape::T && tspin_) {
    int back = 0;
    int front = 0;

    auto check = [&](int x, int y) {
      return filled(current_.x + x, current_.y - y) ? 1 : 0;
    };

    switch (current_.piece.rotation()) {
      case 0: // front up
        back = check(0, 2) + check(2, 2);
        front = check(0, 0) + check(2, 0);
        break;
      case 1: // front right
        back = check(0, 0) + check(0, 2);
        front = check(2, 0) + check(2, 2);
        break;
      case 2: // front down
        back = check(0, 0) + check(2, 0);
        front = check(0, 2) + check(2, 2);
        break;
      case 3: // front left
        back = check(2, 0) + check(2, 2);
        front = check(0, 0) + check(0, 2);
        break;
    }

    if (front == 1 && back == 2) {
      std::cerr << "Mini T-spin" << std::endl;
      audio.play_sample("spin.wav");
      add_points(100 * level_);
      floaters_.emplace_back(4, 30 + 8 * current_.x, 183 - 8 * current_.y);
    } else if (front == 2 && back > 0) {
      std::cerr << "Full T-spin" << std::endl;
      audio.play_sample("fullspin.wav");
      add_points(400 * level_);
      floaters_.emplace_back(5, 30 + 8 * current_.x, 183 - 8 * current_.y);
    }
  }

  audio.play_sample("lock.wav");
  add_points(soft_drop_);
  spawn_piece();
}

bool GameScreen::test_move(int x, int y) {
  current_.x += x;
  current_.y += y;
  if (overlap(current_)) {
    current_.x -= x;
    current_.y -= y;
    return false;
  }
  return true;
}

bool GameScreen::rotate_left() {
  if (current_.piece.shape() == Piece::Shape::O) return true;

  bool old = tspin_;
  tspin_ = true;

  current_.piece.rotate_left();
  if (test_move(0, 0)) return true;

  if (current_.piece.shape() == Piece::Shape::I) {
    switch (current_.piece.rotation()) {
      case 0: // R > 0
        if (test_move(+2,  0)) return true;
        if (test_move(-1,  0)) return true;
        if (test_move(+2, +1)) return true;
        if (test_move(-1, -2)) return true;
        break;

      case 1: // 2 > R
        if (test_move(+1,  0)) return true;
        if (test_move(-2,  0)) return true;
        if (test_move(+1, -2)) return true;
        if (test_move(-2, +1)) return true;
        break;

      case 2: // L > 2
        if (test_move(-2,  0)) return true;
        if (test_move(+1,  0)) return true;
        if (test_move(-2, -1)) return true;
        if (test_move(+1, +2)) return true;
        break;

      case 3: // 0 > L
        if (test_move(-1,  0)) return true;
        if (test_move(+2,  0)) return true;
        if (test_move(-1, +2)) return true;
        if (test_move(+2, -1)) return true;
        break;
    }

  } else {  // Pieces J, L, S, T, and Z

    switch (current_.piece.rotation()) {
      case 0: // R > 0
        if (test_move(+1,  0)) return true;
        if (test_move(+1, -1)) return true;
        if (test_move( 0, +2)) return true;
        if (test_move(+1, +2)) return true;
        break;

      case 1: // 2 > R
        if (test_move(-1,  0)) return true;
        if (test_move(-1, +1)) return true;
        if (test_move( 0, -2)) return true;
        if (test_move(-1, -2)) return true;
        break;

      case 2: // L > 2
        if (test_move(-1,  0)) return true;
        if (test_move(-1, -1)) return true;
        if (test_move( 0, +2)) return true;
        if (test_move(-1, +2)) return true;
        break;

      case 3: // 0 > L
        if (test_move(+1,  0)) return true;
        if (test_move(+1, +1)) return true;
        if (test_move( 0, -2)) return true;
        if (test_move(+1, -2)) return true;
        break;
    }
  }

  current_.piece.rotate_right();
  tspin_ = old;
  return false;
}

bool GameScreen::rotate_right() {
  if (current_.piece.shape() == Piece::Shape::O) return true;

  bool old = tspin_;
  tspin_ = true;

  current_.piece.rotate_right();
  if (test_move(0, 0)) return true;

  if (current_.piece.shape() == Piece::Shape::I) {

    switch (current_.piece.rotation()) {
      case 0: // L > 0
        if (test_move(+1,  0)) return true;
        if (test_move(-2,  0)) return true;
        if (test_move(+1, -2)) return true;
        if (test_move(-2, +1)) return true;
        break;

      case 1: // 0 > R
        if (test_move(-2,  0)) return true;
        if (test_move(+1,  0)) return true;
        if (test_move(-2, -1)) return true;
        if (test_move(+1, +2)) return true;
        break;

      case 2: // R > 2
        if (test_move(-1,  0)) return true;
        if (test_move(+2,  0)) return true;
        if (test_move(-1, +2)) return true;
        if (test_move(+2, -1)) return true;
        break;

      case 3: // 2 > L
        if (test_move(+2,  0)) return true;
        if (test_move(-1,  0)) return true;
        if (test_move(+2, +1)) return true;
        if (test_move(-1, -2)) return true;
        break;
    }

  } else {  // Pieces J, L, S, T, and Z

    switch (current_.piece.rotation()) {
      case 0: // L > 0
        if (test_move(-1,  0)) return true;
        if (test_move(-1, -1)) return true;
        if (test_move( 0, +2)) return true;
        if (test_move(-1, +2)) return true;
        break;

      case 1: // 0 > R
        if (test_move(-1,  0)) return true;
        if (test_move(-1, +1)) return true;
        if (test_move( 0, -2)) return true;
        if (test_move(-1, -2)) return true;
        break;

      case 2: // R > 2
        if (test_move(+1,  0)) return true;
        if (test_move(+1, -1)) return true;
        if (test_move( 0, +2)) return true;
        if (test_move(+1, +2)) return true;
        break;

      case 3: // 2 > L
        if (test_move(+1,  0)) return true;
        if (test_move(+1, +1)) return true;
        if (test_move( 0, -2)) return true;
        if (test_move(+1, -2)) return true;
        break;
    }
  }

  current_.piece.rotate_left();
  tspin_ = old;
  return false;
}

int GameScreen::drop_time() const {
  const float time = std::pow(0.8 - ((level_ - 1) * 0.007), level_ - 1);
  return std::max(kMinDropTime, std::round(1000.f * time));
}

bool GameScreen::check_line(int line) {
  if (value(0, line) == 2) return false;

  for (int x = 0; x < 10; ++x) {
    if (!filled(x, line)) return false;
  }

  for (int x = 0; x < 10; ++x) {
    fill(x, line, 2);
  }

  return true;
}

void GameScreen::drop_lines(int y) {
  for (int iy = y; iy < 22; ++iy) {
    for (int ix = 0; ix < 10; ++ix) {
      fill(ix, iy, value(ix, iy + 1));
    }
  }
}

void GameScreen::add_trash_line() {
  for (int y = 19; y > 0; --y) {
    for (int x = 0; x < 10; ++x) {
      fill(x, y, value(x, y - 1));
    }
  }

  for (int x = 0; x < 10; ++x) {
    fill(x, 0, 0);
  }

  const int garbage_count = std::max(5, 9 - level_ / 2);

  std::vector<int> fills = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  shuffle(fills.begin(), fills.end(), rng_);
  for (int i = 0; i < garbage_count; ++i) {
    fill(fills[i], 0, 18);
  }

  if (overlap(current_)) test_move(0, 1);
}

void GameScreen::add_points(int points) {
  score_ += points * std::pow(2, static_cast<int>(difficulty_) - 1);
}

void GameScreen::game_over(Audio& audio) {
  audio.play_sample("dead.wav");
  audio.music_volume(1);
  state_ = State::GameOver;
  stats_.set_score(score_);
  stats_.set_lines(lines_);
  stats_.set_run_length(duration_);
  stats_.save("content/stats.txt");
}

int GameScreen::hard_drop(Audio& audio) {
  int distance = 0;
  while (!overlap(current_)) {
    --current_.y;
    ++distance;
  }
  ++current_.y;
  if (distance > 0) tspin_ = false;
  lock_piece(audio);
  return distance;
}
