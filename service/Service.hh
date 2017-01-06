// Copyright (c) 2010
// All rights reserved.

#ifndef TRACK_SERVICE_HH
#define TRACK_SERVICE_HH

#include <string>
#include "Config.hh"
#include "cata/TraderService.hh"
#include "soil/STimer.hh"
#include "zod/PullService.hh"
#include "zod/PubService.hh"
#include "json/json.hh"

namespace track {

class Service : public zod::MsgCallback,
                public cata::ServiceCallback {
 public:
  Service(int argc, char* argv[]);

  virtual ~Service();

  // from zod::MsgCallback
  virtual void msgCallback(const zod::Msg*);

  // from cata::ServiceCallback
  virtual void onRspMessage(const std::string& msg);

  virtual void onRtnMessage(const std::string& msg);

protected:
  void queryExchange(const json::Value&);

  void queryInstrument(const json::Value&);

  void queryInvestor();

  void queryAccount();

  void queryTradingCode();

  void queryOrder();

  void queryTrade();

  void queryPosition();

  void wait(int mill_second = -1) {
    cond_->wait(mill_second);
  }

  void notify() {
    cond_->notifyAll();
  }

 private:
  std::unique_ptr<track::Config> config_;

  std::unique_ptr<cata::TraderService> trader_service_;

  std::unique_ptr<soil::STimer> cond_;

  std::unique_ptr<zod::PullService> pull_service_;

  std::unique_ptr<zod::PubService> pub_service_;
};

};  // namespace track

#endif
