// Copyright (c) 2010
// All rights reserved.

#include <functional>
#include "Server.hh"
#include "Log.hh"
#include "boost/regex.hpp"

namespace track {

Server::Server(int argc, char* argv[]) {
  TRACK_TRACE <<"Server::Server()";

  registerFuncs();

  config_.reset(new Config(argc, argv));

  pub_service_.reset(zod::PubService::create(config_->options()->pub_addr));

  trader_service_.reset(cata::TraderService::createService(config_->cataTraderOptions(), this));

  pull_service_.reset(zod::PullService::create(config_->options()->pull_addr, this));

}

Server::~Server() {
  TRACK_TRACE <<"Server::~Server()";

  delete pull_service_.release();

  delete trader_service_.release();

  delete pub_service_.release();
}

void Server::msgCallback(const zod::Msg* msg) {
  TRACK_TRACE <<"Server::msgCallback()";

  std::string mdata(
      reinterpret_cast<const char*>(msg->data_.get()),
      msg->len_);

  TRACK_DEBUG <<mdata;

  json::Document doc;
  json::fromString(mdata, &doc);

  std::hash<std::string> str_hash;

  for (auto itr = doc.MemberBegin();
       itr != doc.MemberEnd(); ++ itr) {
    size_t func_hash = str_hash(itr->name.GetString());

    auto i_itr = funcs_.find(func_hash);
    if (i_itr != funcs_.end()) {
      (this->*i_itr->second)(itr->value);
    }
  }
}

void Server::onRspMessage(const std::string& msg) {
  TRACK_TRACE <<"Server::onRspMessage()";

  TRACK_DEBUG <<msg;

  pub_service_->sendMsg(msg);
}

void Server::onRtnMessage(const std::string& msg) {
  TRACK_TRACE <<"Server::onRtnMessage()";

  TRACK_DEBUG <<msg;

  pub_service_->sendMsg(msg);
}

void Server::queryExchange(const json::Value& value) {
  TRACK_TRACE <<"Server::queryExchange()";

  std::string exchange;
  auto itr = value.FindMember("exchange");
  if (itr != value.MemberEnd()) {
    exchange = itr->value.GetString();
  }
  
  trader_service_->queryExchange(exchange);
}

void Server::queryProduct(const json::Value& value) {
  TRACK_TRACE <<"Server::queryProduct()";

  std::string product_id;
  std::string product_class;
  std::hash<std::string> str_hash;
  for (auto itr = value.MemberBegin();
       itr != value.MemberEnd(); ++ itr) {
    if (str_hash(itr->name.GetString())
        == str_hash(std::string("product_id"))) {
      product_id = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
        == str_hash(std::string("product_class"))) {
      product_class = itr->value.GetString();
    }
  }

  if (!product_class.empty()) {
    trader_service_->queryProduct(product_id,
                                  static_cast<cata::ProductClassType>(product_class[0]));
  }
}

void Server::queryInstrument(const json::Value& value) {
  TRACK_TRACE <<"Server::queryInstrument()";

  std::string instru;
  std::string exchange;
  std::string exchange_instru_id;
  std::string product_id;

  std::hash<std::string> str_hash;
  for (auto itr = value.MemberBegin();
       itr != value.MemberEnd(); ++ itr) {
    if (str_hash(itr->name.GetString())
        == str_hash(std::string("instru"))) {
      instru = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
        == str_hash(std::string("exchange"))) {
      exchange = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
               == str_hash(std::string("exchange_instru_id"))) {
      exchange_instru_id = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
               == str_hash(std::string("product_id"))) {
      product_id = itr->value.GetString();
    }
  }

  trader_service_->queryInstrument(instru, exchange,
                                   exchange_instru_id,
                                   product_id);
}

void Server::queryInvestor(const json::Value& value) {
  TRACK_TRACE <<"Server::queryInvestor()";

  trader_service_->queryInvestor();
}

void Server::queryAccount(const json::Value& value) {
  TRACK_TRACE <<"Server::queryAccount()";

  std::string currency;
  auto itr = value.FindMember("currency");
  if (itr != value.MemberEnd()) {
    currency = itr->value.GetString();
  }

  trader_service_->queryAccount(currency);
}

void Server::queryTradingCode(const json::Value& value) {
  TRACK_TRACE <<"Server::queryTradingCode()";

  std::string exchange;
  std::string client_id;
  std::string client_id_type;

  std::hash<std::string> str_hash;
  for (auto itr = value.MemberBegin();
       itr != value.MemberEnd(); ++ itr) {
    if (str_hash(itr->name.GetString())
        == str_hash(std::string("exchange"))) {
      exchange = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
               == str_hash(std::string("client_id"))) {
      client_id = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
               == str_hash(std::string("client_id_type"))) {
      client_id_type = itr->value.GetString();
    }
  }

  if (!client_id_type.empty()) {
    trader_service_->queryTradingCode(exchange,
                                      client_id,
                                      static_cast<cata::ClientIDType>(client_id_type[0]));
  }
}

void Server::queryOrder(const json::Value& value) {
  TRACK_TRACE <<"Server::queryOrder()";

  std::string instru;
  std::string exchange;
  std::string order_sys_id;
  std::string start_time;
  std::string stop_time;

  std::hash<std::string> str_hash;
  for (auto itr = value.MemberBegin();
       itr != value.MemberEnd(); ++ itr) {
    if (str_hash(itr->name.GetString())
        == str_hash(std::string("instru"))) {
      instru = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
        == str_hash(std::string("exchange"))) {
      exchange = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
               == str_hash(std::string("order_sys_id"))) {
      order_sys_id = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
               == str_hash(std::string("start_time"))) {
      start_time = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
               == str_hash(std::string("stop_time"))) {
      stop_time = itr->value.GetString();
    }

  }

  trader_service_->queryOrder(instru, exchange,
                              order_sys_id,
                              start_time,
                              stop_time);
}

void Server::queryTrade(const json::Value& value) {
  TRACK_TRACE <<"Server::queryTrade()";

  std::string instru;
  std::string exchange;
  std::string trade_id;
  std::string start_time;
  std::string stop_time;

  std::hash<std::string> str_hash;
  for (auto itr = value.MemberBegin();
       itr != value.MemberEnd(); ++ itr) {
    if (str_hash(itr->name.GetString())
        == str_hash(std::string("instru"))) {
      instru = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
        == str_hash(std::string("exchange"))) {
      exchange = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
               == str_hash(std::string("trade_id"))) {
      trade_id = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
               == str_hash(std::string("start_time"))) {
      start_time = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
               == str_hash(std::string("stop_time"))) {
      stop_time = itr->value.GetString();
    }
  }

  trader_service_->queryTrade(instru, exchange,
                              trade_id,
                              start_time,
                              stop_time);
}

void Server::queryPosition(const json::Value& value) {
  TRACK_TRACE <<"Server::queryPosition()";

  std::string instru;
  fetchInstruField(value, &instru);

  trader_service_->queryPosition(instru);
}

void Server::queryInstruMarginRate(const json::Value& value) {
  TRACK_TRACE <<"Server::queryInstruMarginRate()";

  InstruHedgeField req;
  fetchInstruHedgeField(value, &req);

  trader_service_->queryInstruMarginRate(req.instru, req.hedge_flag);
}

void Server::queryInstruCommissionRate(const json::Value& value) {
  TRACK_TRACE <<"Server::queryInstruCommissionRate()";

  std::string instru;
  fetchInstruField(value, &instru);

  trader_service_->queryInstruCommissionRate(instru);
}

void Server::queryExchangeMarginRate(const json::Value& value) {
  TRACK_TRACE <<"Server::queryExchangeMarginRate()";

  InstruHedgeField req;
  fetchInstruHedgeField(value, &req);

  trader_service_->queryExchangeMarginRate(req.instru, req.hedge_flag);
}

void Server::queryExchangeMarginRateAdjust(const json::Value& value) {
  TRACK_TRACE <<"Server::queryExchangeMarginRateAdjust()";

  InstruHedgeField req;
  fetchInstruHedgeField(value, &req);

  trader_service_->queryExchangeMarginRateAdjust(req.instru, req.hedge_flag);
}

void Server::queryDepthMarketData(const json::Value& value) {
  TRACK_TRACE <<"Server::queryDepthMarketData()";

  std::string instru;
  fetchInstruField(value, &instru);

  trader_service_->queryDepthMarketData(instru);
}


void Server::order(const json::Value& value) {
  TRACK_TRACE <<"Server::order()";

  ReqOrderField req;
  fetchReqOrderField(value, &req);
  
  trader_service_->order(req.direct, req.offset_flag,
                         req.hedge_flag, req.instru,
                         req.price, req.volume);
}

void Server::orderFAK(const json::Value& value) {
  TRACK_TRACE <<"Server::orderFAK()";

  ReqOrderField req;
  fetchReqOrderField(value, &req);

  trader_service_->orderFAK(req.direct, req.offset_flag,
                            req.hedge_flag, req.instru,
                            req.price, req.volume);
}

void Server::orderFOK(const json::Value& value) {
  TRACK_TRACE <<"Server::orderFOK()";

  ReqOrderField req;
  fetchReqOrderField(value, &req);

  trader_service_->orderFOK(req.direct, req.offset_flag,
                            req.hedge_flag, req.instru,
                            req.price, req.volume);
}

void Server::registerFuncs() {
  TRACK_TRACE <<"Server::registerFuncs()";

  std::hash<std::string> str_hash;

  funcs_[str_hash("queryExchange")] = &Server::queryExchange;
  funcs_[str_hash("queryProduct")] = &Server::queryProduct;
  funcs_[str_hash("queryInstrument")] = &Server::queryInstrument;
  funcs_[str_hash("queryInvestor")] = &Server::queryInvestor;
  funcs_[str_hash("queryAccount")] = &Server::queryAccount;
  funcs_[str_hash("queryTradingCode")] = &Server::queryTradingCode;
  funcs_[str_hash("queryOrder")] = &Server::queryOrder;
  funcs_[str_hash("queryTrade")] = &Server::queryTrade;
  funcs_[str_hash("queryPosition")] = &Server::queryPosition;
  funcs_[str_hash("queryInstruMarginRate")] = &Server::queryInstruMarginRate;
  funcs_[str_hash("queryInstruCommissionRate")] = &Server::queryInstruCommissionRate;
  funcs_[str_hash("queryExchangeMarginRate")] = &Server::queryExchangeMarginRate;
  funcs_[str_hash("queryExchangeMarginRateAdjust")] = &Server::queryExchangeMarginRateAdjust;
  funcs_[str_hash("queryDepthMarketData")] = &Server::queryDepthMarketData;

  funcs_[str_hash("order")] = &Server::order;
  funcs_[str_hash("orderFAK")] = &Server::orderFAK;
  funcs_[str_hash("orderFOK")] = &Server::orderFOK;
}

void Server::fetchInstruField(const json::Value& value, std::string* instru) {
  TRACK_TRACE <<"Server::fetchInstruField()";

  auto itr = value.FindMember("instru");
  if (itr != value.MemberEnd()) {
    *instru = itr->value.GetString();
  }
}

void Server::fetchInstruHedgeField(const json::Value& value, InstruHedgeField* req) {
  TRACK_TRACE <<"Server::fetchInstruHedgeField()";

  std::hash<std::string> str_hash;
  for (auto itr = value.MemberBegin();
       itr != value.MemberEnd(); ++ itr) {
    if (str_hash(itr->name.GetString())
        == str_hash(std::string("instru"))) {
      req->instru = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
        == str_hash(std::string("hedge_flag"))) {
      std::string hedge_flag = itr->value.GetString();
      if (!hedge_flag.empty()) {
        req->hedge_flag = static_cast<cata::HedgeFlagType>(hedge_flag[0]);
      } else {
        throw std::runtime_error("invalid hedge_flag.");
      }
    }
  }
}

void Server::fetchReqOrderField(const json::Value& value,
                                ReqOrderField* req) {
  TRACK_TRACE <<"Server::fetchReqOrderField()";

  req->price = 0;
  req->volume = 0;
  
  std::hash<std::string> str_hash;
  for (auto itr = value.MemberBegin();
       itr != value.MemberEnd(); ++ itr) {
    if (str_hash(itr->name.GetString())
        == str_hash(std::string("direct"))) {
      std::string direct = itr->value.GetString();

      if (!direct.empty()) {
        req->direct = static_cast<cata::DirectionType>(direct[0]);
      } else {
        throw std::runtime_error("invalid direct.");
      }
    } else if (str_hash(itr->name.GetString())
        == str_hash(std::string("offset_flag"))) {
      std::string offset_flag = itr->value.GetString();

      if (!offset_flag.empty()) {
        req->offset_flag = static_cast<cata::OffsetFlagType>(offset_flag[0]);
      } else {
        throw std::runtime_error("invalid offsete_flag.");
      }
    } else if (str_hash(itr->name.GetString())
        == str_hash(std::string("hedge_flag"))) {
      std::string hedge_flag = itr->value.GetString();

      if (!hedge_flag.empty()) {
        req->hedge_flag = static_cast<cata::HedgeFlagType>(hedge_flag[0]);
      } else {
        throw std::runtime_error("invalid hedge_flag.");
      }
    } else if (str_hash(itr->name.GetString())
        == str_hash(std::string("instru"))) {
      req->instru = itr->value.GetString();
    } else if (str_hash(itr->name.GetString())
        == str_hash(std::string("price"))) {
      req->price = itr->value.GetDouble();
    } else if (str_hash(itr->name.GetString())
        == str_hash(std::string("volume"))) {
      req->volume = itr->value.GetInt();
    } 
  }
}

};  // namespace track
