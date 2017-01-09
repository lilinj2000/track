// Copyright (c) 2010
// All rights reserved.

#include <fstream>
#include <iostream>
#include "boost/program_options.hpp"

#include "Config.hh"
#include "Log.hh"
#include "cata/TraderService.hh"

namespace track {

Options::Options():
    config_options_("TrackConfigOptions") {
  namespace po = boost::program_options;

  config_options_.add_options()
      ("track.pull_addr", po::value<std::string>(&pull_addr),
       "pull address")
      ("track.pub_addr", po::value<std::string>(&pub_addr),
       "pub address")
      ("track.log_cfg", po::value<std::string>(&log_cfg),
       "log config file");

  return;
}

Options::~Options() {
}

po::options_description* Options::configOptions() {
  return &config_options_;
}

Config::Config(int argc, char* argv[]) {
  options_.reset(new Options());
  cata_trader_options_.reset(cata::TraderService::createOptions());

  std::unique_ptr<soil::Config> config(soil::Config::create());
  config->registerOptions(options_.get());
  config->registerOptions(cata_trader_options_.get());

  config->configFile() = "track.cfg";
  config->loadConfig(argc, argv);

  // init the log
  TRACK_LOG_INIT(options_->log_cfg);

  return;
}

Config::~Config() {
}

};  // namespace track
