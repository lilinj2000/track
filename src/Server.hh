// Copyright (c) 2010
// All rights reserved.

#ifndef TRACK_SERVER_HH
#define TRACK_SERVER_HH

#include <string>
#include "Config.hh"
#include "cata/TraderService.hh"
#include "soil/STimer.hh"
#include "zod/PullService.hh"
#include "zod/PubService.hh"
#include "json/json.hh"

namespace track {

class Server : public zod::MsgCallback,
                public cata::ServiceCallback {
 public:
  Server(int argc, char* argv[]);

  virtual ~Server();

  // from zod::MsgCallback
  virtual void msgCallback(const zod::Msg*);

  // from cata::ServerCallback
  virtual void onRspMessage(const std::string& msg);

  virtual void onRtnMessage(const std::string& msg);

protected:
  void queryExchange(const json::Value&);

  void queryInstrument(const json::Value&);

  void queryInvestor(const json::Value&);

  void queryAccount(const json::Value&);

  void queryTradingCode(const json::Value&);

  void queryOrder(const json::Value&);

  void queryTrade(const json::Value&);

  void queryPosition(const json::Value&);

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
