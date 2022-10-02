#include "stats_screen.h"

bool StatsScreen::update(const Input& input, Audio&, unsigned int) {
  return !input.any_pressed();
}

namespace {
  std::string format_time(uint64_t ms) {
    if (ms == 0) return "N/A";

    int s = ms / 1000 % 60;
    int m = ms / 60000 % 60;
    int h = ms / 3600000;

    std::string time = std::to_string(s);
    if (m > 0 || h > 0) time = std::to_string(m) + (s < 10 ? ":0" : ":") + time;
    if (h > 0) time = std::to_string(h) + (m < 10 ? ":0" : ":") + time;
    return time;
  }
}

void StatsScreen::draw(Graphics& graphics) const {
  background_.draw(graphics);

  draw_stat(graphics, 120, "High Score:", stats_.high_score());
  draw_stat(graphics, 136, "Most Lines:", stats_.most_lines());
  draw_stat(graphics, 152, "Biggest Clear:", stats_.biggest_clear());
  draw_stat(graphics, 168, "Nearest Edge:", stats_.best_edge(), "ms");
  draw_stat(graphics, 184, "Longest Run:", format_time(stats_.longest_run()));
}

void StatsScreen::draw_stat(Graphics& graphics, int y, const std::string& label, const std::string& value) const {
  text_.draw(graphics, label, 24, y);
  text_.draw(graphics, value, 232, y, Text::Alignment::Right);
}

void StatsScreen::draw_stat(Graphics& graphics, int y, const std::string& label, int value) const {
  draw_stat(graphics, y, label, value ? std::to_string(value) : "N/A");
}

void StatsScreen::draw_stat(Graphics& graphics, int y, const std::string& label, int value, const std::string& units) const {
  draw_stat(graphics, y, label, value ? std::to_string(value) + units : "N/A");
}
