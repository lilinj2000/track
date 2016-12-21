// Copyright (c) 2010
// All rights reserved.

#ifndef TRACK_SERVER_HH
#define TRACK_SERVER_HH

#include <string>
#include "Config.hh"
#include "cata/TraderService.hh"
#include "soil/STimer.hh"
#include "json/json.hh"
#include "cppdb/frontend.h"

namespace track {

class Server : public cata::ServiceCallback {
 public:
  Server(int argc, char* argv[]);

  virtual ~Server();

  virtual void onRspMessage(const std::string& msg);

  virtual void onRtnMessage(const std::string& msg);

protected:
  void go();
  
  void queryExchange();

  void queryInstrument();

  void queryInvestor();

  void queryAccount();

  void queryTradingCode();

  void queryOrder();

  void queryTrade();

  void queryPosition();

  void sqlString(const std::string& name,
                 json::Value& data,
                 std::string& create_sql,
                 std::string& insert_sql);

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

  std::unique_ptr<cppdb::session> db_;
};

};  // namespace track

#endif
