// Copyright (c) 2010
// All rights reserved.

#include <functional>
#include "Server.hh"
#include "Log.hh"
#include "boost/regex.hpp"

namespace track {

Server::Server(int argc, char* argv[]) {
  TRACK_TRACE <<"Server::Server()";

  cond_.reset(soil::STimer::create());

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
    if (str_hash(itr->name.GetString())
        == str_hash("queryExchange")) {
          queryExchange(itr->value);
    } else if (str_hash(itr->name.GetString())
               == str_hash("queryProduct")) {
    } else if (str_hash(itr->name.GetString())
               == str_hash("queryInstrument")) {
          queryInstrument(itr->value);
    } else if (str_hash(itr->name.GetString())
               == str_hash("queryInvestor")) {
          queryInvestor(itr->value);
    } else if (str_hash(itr->name.GetString())
               == str_hash("queryAccount")) {
          queryAccount(itr->value);
    } else if (str_hash(itr->name.GetString())
               == str_hash("queryTradingCode")) {
          queryTradingCode(itr->value);
    } else if (str_hash(itr->name.GetString())
               == str_hash("queryOrder")) {
          queryOrder(itr->value);
    } else if (str_hash(itr->name.GetString())
               == str_hash("queryTrade")) {
          queryTrade(itr->value);
    } else if (str_hash(itr->name.GetString())
               == str_hash("queryPosition")) {
          queryPosition(itr->value);
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
  auto itr = value.FindMember("instru");
  if (itr != value.MemberEnd()) {
    instru = itr->value.GetString();
  }

  trader_service_->queryPosition(instru);
}

};  // namespace track
