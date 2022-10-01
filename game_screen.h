#pragma once

#include <array>
#include <random>
#include <vector>

#include "backdrop.h"
#include "screen.h"
#include "sprite.h"
#include "spritemap.h"

#include "piece.h"

class GameScreen : public Screen {
  public:

    GameScreen();

    bool update(const Input&, Audio&, unsigned int) override;
    void draw(Graphics& graphics) const override;

  private:

    static constexpr float kMinDropTime = 64.f;

    struct PieceData {
      int drop = 0;
      int x = 3, y = 22;
      Piece piece = Piece::Shape::S;
    };

    Backdrop background_;
    SpriteMap blocks_, digits_;
    Sprite laser_;
    PieceData current_;
    std::mt19937 rng_;
    std::array<int, 220> board_;
    int level_;
    int scan_timer_, scanner_, scanner_drop_timer_;
    bool scanner_line_found_;

    std::vector<Piece::Shape> bag_;

    bool overlap(const PieceData& piece) const;
    int value(int x, int y) const;
    bool filled(int x, int y) const { return value(x, y) > 0; }
    void fill(int x, int y, int value);

    void spawn_piece();
    void fill_bag();
    void lock_piece();

    bool rotate_left();
    bool rotate_right();
    bool test_move(int x, int y);

    int drop_time() const;
    bool check_line(int line);
    void drop_lines(int y);
};
