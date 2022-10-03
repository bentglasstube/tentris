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

    class Menu {
      public:
        Menu(std::vector<std::string> options, int choice=0) : options_(options), choice_(choice) {}

        std::string current() const { return options_[choice_]; }
        int choice() const { return choice_; }

        void next() { if (choice_ < options_.size() - 1) ++choice_; }
        void prev() { if (choice_ > 0) --choice_; }

      private:

        std::vector<std::string> options_;
        size_t choice_;
    };

    Backdrop background_;
    Text text_;
    Stats stats_;
    Menu difficulty_, music_;
    std::mt19937 rng_;
    std::vector<MovingPiece> pieces_;
    int spawn_timer_, choice_;
};
