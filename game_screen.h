#pragma once

#include <array>
#include <random>
#include <vector>

#include "backdrop.h"
#include "screen.h"

#include "piece.h"

class GameScreen : public Screen {
  public:

    GameScreen();

    bool update(const Input&, Audio&, unsigned int) override;
    void draw(Graphics& graphics) const override;

  private:

    static constexpr int kDropTime = 64 * 1000 / 60;

    struct PieceData {
      int drop = 0;
      int x = 3, y = 22;
      Piece piece = Piece::Shape::S;
    };

    Backdrop background_;
    SpriteMap blocks_;
    PieceData current_;
    std::mt19937 rng_;
    std::array<int, 220> board_;

    std::vector<Piece::Shape> bag_;

    bool overlap(const PieceData& piece) const;
    bool filled(int x, int y) const;
    void fill(int x, int y, int value);

    void spawn_piece();
    void fill_bag();
    void lock_piece();
};
