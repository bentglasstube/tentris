#pragma once

#include "backdrop.h"
#include "graphics.h"
#include "screen.h"
#include "text.h"

#include "stats.h"
#include "title_screen.h"

class StatsScreen : public Screen {
  public:

    StatsScreen() : background_("title.png"), text_("text.png"), stats_("content/stats.txt") {}

    bool update(const Input& input, Audio&, unsigned int) override;
    void draw(Graphics& graphics) const override;
    std::string get_music_track() const override { return "title.ogg"; }
    Screen* next_screen() const override { return new TitleScreen(); }

  private:

    Backdrop background_;
    Text text_;
    Stats stats_;

    void draw_stat(Graphics& graaphics, int y, const std::string& label, const std::string& value) const;
    void draw_stat(Graphics& graaphics, int y, const std::string& label, int value) const;
    void draw_stat(Graphics& graaphics, int y, const std::string& label, int value, const std::string& units) const;

};
