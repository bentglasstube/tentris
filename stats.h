#pragma once

#include <string>

class Stats {
  public:

    Stats(const std::string& filename);

    uint32_t high_score() const { return high_score_; }
    uint8_t most_lines() const { return most_lines_; }
    uint64_t longest_run() const { return longest_run_; }
    uint8_t biggest_clear() const { return biggest_clear_; }
    uint16_t best_edge() const { return best_edge_; }

    void set_score(uint32_t score) { if (score > high_score_) high_score_ = score; }
    void set_lines(uint8_t lines) { if (lines > most_lines_) most_lines_ = lines; }
    void set_run_length(uint64_t length) { if (length > longest_run_) longest_run_ = length; }
    void set_clear(uint8_t clear) { if (clear > biggest_clear_) biggest_clear_ = clear; }
    void set_edge(uint16_t edge) { if (edge < best_edge_) best_edge_ = edge; }

    void save(const std::string& filename) const;

  private:

    uint32_t high_score_;
    uint8_t most_lines_;
    uint64_t longest_run_;
    uint8_t biggest_clear_;
    uint16_t best_edge_;
};
