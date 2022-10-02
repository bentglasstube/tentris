#pragma once

#include <random>

#include "backdrop.h"
#include "screen.h"
#include "text.h"

#include "piece.h"
#include "stats.h"

class TitleScreen : public Screen {
  public:

    TitleScreen();

    struct MovingPiece {
      Piece p;
      int x, y;
      MovingPiece(Piece p, int x, int y) : p(p), x(x), y(y) {}
    };

    bool update(const Input&, Audio&, unsigned int) override;
    void draw(Graphics&) const override;

    std::string get_music_track() const override { return "filo.ogg"; }
    Screen* next_screen() const override;

  private:

    Backdrop background_;
    Text text_;
    Stats stats_;
    std::mt19937 rng_;
    std::vector<MovingPiece> pieces_;
    int spawn_timer_, choice_, music_;

    std::string get_music_title() const;

};
