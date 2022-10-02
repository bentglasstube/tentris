#pragma once

#include <array>
#include <random>
#include <vector>

#include "backdrop.h"
#include "screen.h"
#include "sprite.h"
#include "spritemap.h"
#include "text.h"

#include "floater.h"
#include "piece.h"
#include "stats.h"
#include "title_screen.h"

class GameScreen : public Screen {
  public:

    enum class Difficulty { Rusty, Trusty, Lusty };

    GameScreen(Difficulty difficulty);

    bool update(const Input&, Audio&, unsigned int) override;
    void draw(Graphics& graphics) const override;

    std::string get_music_track() const override { return "theme_a.ogg"; }
    Screen* next_screen() const override { return new TitleScreen; }

  private:

    static constexpr float kMinDropTime = 64.f;
    static constexpr int kAutoShiftDelay = 133;
    static constexpr int kAutoShiftRepeat = 50;

    enum class State { Playing, Paused, GameOver };

    struct PieceData {
      int drop = 0;
      int x = 3, y = 22;
      Piece piece = Piece::Shape::S;
    };

    Backdrop background_;
    SpriteMap blocks_, digits_;
    Sprite laser_, top_;
    Text text_;

    State state_;
    Stats stats_;
    Difficulty difficulty_;
    PieceData current_;
    std::mt19937 rng_;
    std::array<int, 220> board_;
    std::vector<Floater> floaters_;
    uint64_t duration_;
    int lines_, level_, score_, soft_drop_;
    int scan_timer_, scanner_, scanner_drop_timer_;
    int auto_shift_;
    bool tspin_;

    std::vector<Piece::Shape> bag_;

    bool overlap(const PieceData& piece) const;
    int value(int x, int y) const;
    bool filled(int x, int y) const { return value(x, y) > 0; }
    bool board_empty() const;
    void fill(int x, int y, int value);

    void spawn_piece();
    void fill_bag();
    void lock_piece(Audio& audio);

    bool rotate_left();
    bool rotate_right();
    bool test_move(int x, int y);

    int drop_time() const;
    bool check_line(int line);
    void drop_lines(int y);
    void add_trash_line();

    int block_texture() const { return (level_ - 1) % 8; }
    void add_points(int points);
    void game_over(Audio& audio);
};
