// Copyright (c) 2010
// All rights reserved.

#ifndef TRACK_SERVER_HH
#define TRACK_SERVER_HH

#include <string>
#include "Config.hh"
#include "cata/TraderService.hh"
#include "soil/STimer.hh"

namespace track {

class Server : public cata::ServiceCallback {
 public:
  Server(int argc, char* argv[]);

  virtual ~Server();

  virtual void onRspMessage(const std::string& msg);

  virtual void onRtnMessage(const std::string& msg);

protected:
  void go();
  
  void queryInvestor();

  void queryExchange();

  void queryInstrument();

  void wait() {
    cond_->wait(1000);
  }

 private:
  std::unique_ptr<track::Config> config_;

  std::unique_ptr<cata::TraderService> trader_service_;

  std::unique_ptr<soil::STimer> cond_;
};

};  // namespace track

#endif
