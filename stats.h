#pragma once

#include <functional>
#include <map>
#include <string>

class Stats {
  public:

    Stats(const std::string& filename);

    uint64_t high_score()    const { return get("high_score"); }
    uint64_t most_lines()    const { return get("most_lines"); }
    uint64_t biggest_clear() const { return get("biggest_clear"); }
    uint64_t best_edge()     const { return get("best_edge"); }
    uint64_t longest_run()   const { return get("longest_run"); }

    void set_score(uint64_t value)       { set("high_score",    value); }
    void set_lines(uint64_t value)       { set("most_lines",    value); }
    void set_clear(uint64_t value)       { set("biggest_clear", value); }
    void set_edge(uint64_t value)        { set("best_edge",     value, [](uint64_t a, uint64_t b) { return a < b; }); }
    void set_run_length(uint64_t value)  { set("longest_run",   value); }

    void save(const std::string& filename) const;

  private:

    std::map<std::string, uint64_t> data_;

    void set(const std::string& key, uint64_t value);
    void set(const std::string& key, uint64_t value, std::function<bool(uint64_t, uint64_t)> comp);
    uint64_t get(const std::string& key) const;
};
