#include "stats.h"

#include <iostream>
#include <fstream>

Stats::Stats(const std::string& filename) {
  std::ifstream reader(filename);
  std::string k;
  uint64_t v;

  while (reader) {
    reader >> k >> v;
    if (reader.eof()) break;
    std::cerr << "Loaded value " << v << " for " << k << std::endl;
    data_[k] = v;
  }
}

void Stats::save(const std::string& filename) const {
  std::ofstream writer(filename);
  if (writer) {
    for (const auto& kv : data_) {
      writer << kv.first << " " << kv.second << std::endl;
    }
  }
}

void Stats::set(const std::string& key, uint64_t value, Comparator comp) {
  auto it = data_.find(key);
  if (it == data_.end()) {
    std::cerr << "Saving " << value << " for '" << key << "'" << std::endl;
    data_.insert(std::make_pair(key, value));
  } else {
    if (comp(value, it->second)) {
      std::cerr << "Replacing " << it->second << " with " << value << " for '" << key << "'" << std::endl;
      it->second = value;
    }
  }
}

uint64_t Stats::get(const std::string& key, uint64_t def) const {
  const auto& it = data_.find(key);
  return it == data_.end() ? def : it->second;
}

uint64_t Stats::get(const std::string& key, uint64_t min, uint64_t max, uint64_t def) const {
  return std::clamp(get(key, def), min, max);
}
