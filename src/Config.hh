// Copyright (c) 2010
// All rights reserved.

#ifndef TRACK_CONFIG_HH
#define TRACK_CONFIG_HH

#include <string>
#include <memory>
#include "soil/Config.hh"

namespace track {

namespace po = boost::program_options;

class Options : public soil::Options {
 public:
  Options();

  virtual ~Options();

  virtual po::options_description* configOptions();

  std::string pull_addr;
  std::string pub_addr;

  std::string log_cfg;

 private:
  boost::program_options::options_description config_options_;
};

class Config {
 public:
  explicit Config(int argc = 0, char* argv[] = nullptr);
  ~Config();

  Options* options() {
    return options_.get();
  }

  soil::Options* cataTraderOptions() {
    return cata_trader_options_.get();
  }

 private:
  std::unique_ptr<Options> options_;

  std::unique_ptr<soil::Options> cata_trader_options_;
};

}  // namespace track
#endif
