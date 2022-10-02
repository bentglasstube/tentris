#include "stats.h"

#include <iostream>
#include <fstream>

Stats::Stats(const std::string& filename) :
  high_score_(0), most_lines_(0), longest_run_(0),
  biggest_clear_(0), best_edge_(2000)
{
  std::ifstream reader(filename);
  reader >> high_score_ >> most_lines_ >> longest_run_;
  reader >> biggest_clear_ >> best_edge_;

  std::cerr << "Statistics loaded:" << std::endl;
  std::cerr << "     High Score: " << high_score_    << std::endl;
  std::cerr << "     Most Lines: " << most_lines_    << std::endl;
  std::cerr << "    Longest Run: " << longest_run_   << std::endl;
  std::cerr << "  Biggest Clear: " << biggest_clear_ << std::endl;
  std::cerr << "      Best Edge: " << best_edge_     << std::endl;
}

void Stats::save(const std::string& filename) const {
  std::ofstream writer(filename);
  if (writer) {
    writer << high_score_ << most_lines_ << longest_run_;
    writer << biggest_clear_ << best_edge_;
  }
}
