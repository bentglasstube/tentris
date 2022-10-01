#include "game_screen.h"

#include <algorithm>
#include <iostream>

GameScreen::GameScreen() :
  background_("background.png"),
  blocks_("blocks.png", 19, 8, 8),
  rng_(Util::random_seed())
{
  board_.fill(0);
  fill_bag();
  spawn_piece();
}

bool GameScreen::update(const Input& input, Audio&, unsigned int elapsed) {
  if (input.key_pressed(Input::Button::A)) rotate_left();
  if (input.key_pressed(Input::Button::B)) rotate_right();

  if (input.key_pressed(Input::Button::Left)) test_move(-1, 0);
  if (input.key_pressed(Input::Button::Right)) test_move(1, 0);

  current_.drop -= elapsed * (input.key_held(Input::Button::Down) ? 20 : 1);

  if (input.key_pressed(Input::Button::Up)) {
    while (!overlap(current_)) {
      --current_.y;
    }
    ++current_.y;
    lock_piece();
    spawn_piece();
  }

  while (current_.drop < 0) {
    if (test_move(0, -1)) {
      current_.drop += kDropTime;
    } else {
      lock_piece();
      spawn_piece();

      // if a freshly spawned piece overlaps the board, you lose
      if (overlap(current_)) return false;
    }
  }

  return true;
}

void GameScreen::draw(Graphics& graphics) const {
  background_.draw(graphics);
  current_.piece.draw(graphics, 40 + 8 * current_.x, 192 - 8 * current_.y);

  const Piece next = bag_.back();
  switch (next.shape()) {
    case Piece::Shape::I:
      next.draw(graphics, 176, 76);
      break;

    case Piece::Shape::O:
      next.draw(graphics, 176, 80);
      break;

    default:
      next.draw(graphics, 180, 80);
      break;
  }

  for (int y = 0; y < 20; ++y) {
    for (int x = 0; x < 10; ++x) {
      if (filled(x, y)) {
        blocks_.draw(graphics, board_[y * 10 + x], 40 + 8 * x, 192 - 8 * y);
      }
    }
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

bool GameScreen::filled(int x, int y) const {
  if (x < 0 || x > 9) return true;
  if (y < 0) return true;
  if (y > 21) return false;

  return board_[y * 10 + x] > 0;
}

void GameScreen::fill(int x, int y, int value) {
  if (x < 0 || x > 9) return;
  if (y < 0 || y > 21) return;

  board_[y * 10 + x] = value;
}

void GameScreen::spawn_piece() {
  std::cerr << "Spawning next piece." << std::endl;
  current_ = { 0, 3, 21, bag_.back() };
  bag_.pop_back();
  if (bag_.empty()) fill_bag();
}

void GameScreen::fill_bag() {
  std::cerr << "Filling bag." << std::endl;
  for (int i = 0; i < 7; ++i) {
    bag_.push_back(static_cast<Piece::Shape>(i));
  }
  shuffle(bag_.begin(), bag_.end(), rng_);
}

void GameScreen::lock_piece() {
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 4; ++x) {
      if (current_.piece.block(x, y)) {
        fill(current_.x + x, current_.y - y, current_.piece.color());
      }
    }
  }
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
  return false;
}

bool GameScreen::rotate_right() {
  if (current_.piece.shape() == Piece::Shape::O) return true;

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
  return false;
}
