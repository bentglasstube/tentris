#pragma once

#include <functional>
#include <map>
#include <string>

class Stats {
  public:

    enum class Difficulty { Rusty, Trusty, Lusty };
    enum class Music { Folk, Funk, Filo, Fear };

    Stats(const std::string& filename);

    uint64_t high_score()    const { return get("high_score"); }
    uint64_t most_lines()    const { return get("most_lines"); }
    uint64_t biggest_clear() const { return get("biggest_clear"); }
    uint64_t best_edge()     const { return get("best_edge"); }
    uint64_t longest_run()   const { return get("longest_run"); }

    void set_score(uint64_t value)       { set("high_score",    value, Maximize); }
    void set_lines(uint64_t value)       { set("most_lines",    value, Maximize); }
    void set_clear(uint64_t value)       { set("biggest_clear", value, Maximize); }
    void set_edge(uint64_t value)        { set("best_edge",     value, Minimize); }
    void set_run_length(uint64_t value)  { set("longest_run",   value, Maximize); }

    void set_music(uint64_t value)       { set("music",         value, Always); }
    void set_difficulty(uint64_t value)  { set("difficulty",    value, Always); }

    Difficulty difficulty() const { return static_cast<Difficulty>(std::clamp(get("difficulty", 1), 0ul, 2ul)); }
    Music music() const { return static_cast<Music>(std::clamp(get("music"), 0ul, 3ul)); }

    void save(const std::string& filename) const;

  private:

    using Comparator = std::function<bool(uint64_t, uint64_t)>;

    static bool Maximize(uint64_t a, uint64_t b) { return a > b; }
    static bool Minimize(uint64_t a, uint64_t b) { return a < b; }
    static bool Always(uint64_t, uint64_t) { return true; }

    std::map<std::string, uint64_t> data_;

    void set(const std::string& key, uint64_t value) { set(key, value, Maximize); }
    void set(const std::string& key, uint64_t value, Comparator comp);
    uint64_t get(const std::string& key, uint64_t def = 0) const;
};
