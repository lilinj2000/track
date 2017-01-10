// Copyright (c) 2010
// All rights reserved.

#ifndef TRACK_SERVER_HH
#define TRACK_SERVER_HH

#include <string>
#include <map>
#include "Config.hh"
#include "cata/TraderService.hh"
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

  void queryProduct(const json::Value&);

  void queryInstrument(const json::Value&);

  void queryInvestor(const json::Value&);

  void queryAccount(const json::Value&);

  void queryTradingCode(const json::Value&);

  void queryOrder(const json::Value&);

  void queryTrade(const json::Value&);

  void queryPosition(const json::Value&);

  void queryInstruMarginRate(const json::Value&);

  void queryInstruCommissionRate(const json::Value&);

  void queryExchangeMarginRate(const json::Value&);

  void queryExchangeMarginRateAdjust(const json::Value&);

  void queryDepthMarketData(const json::Value&);

  void order(const json::Value&);

  void orderFAK(const json::Value&);

  void orderFOK(const json::Value&);

  void registerFuncs();

  void fetchInstruField(const json::Value&, std::string* instru);

  typedef struct {
    std::string instru;
    cata::HedgeFlagType hedge_flag;
  }InstruHedgeField;
  void fetchInstruHedgeField(const json::Value&, InstruHedgeField*);
      
  typedef struct {
    cata::DirectionType direct;
    cata::OffsetFlagType offset_flag;
    cata::HedgeFlagType hedge_flag;
    std::string instru;
    double price;
    int volume;
  }ReqOrderField;
  void fetchReqOrderField(const json::Value&,
                          ReqOrderField*);

 private:
  std::unique_ptr<track::Config> config_;

  std::unique_ptr<cata::TraderService> trader_service_;

  std::unique_ptr<zod::PullService> pull_service_;

  std::unique_ptr<zod::PubService> pub_service_;

  typedef void (Server::*FuncPtr)(const json::Value&);
  std::map<size_t, FuncPtr> funcs_;
};

};  // namespace track

#endif
