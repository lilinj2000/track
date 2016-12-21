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
  
  void queryInvestor();

  void queryExchange();

  void queryInstrument();

  void sqlString(const std::string& name,
                 json::Value& data,
                 std::string& create_sql,
                 std::string& insert_sql);

  void wait() {
    cond_->wait(1000);
  }

 private:
  std::unique_ptr<track::Config> config_;

  std::unique_ptr<cata::TraderService> trader_service_;

  std::unique_ptr<soil::STimer> cond_;

  std::unique_ptr<cppdb::session> db_;
};

};  // namespace track

#endif
