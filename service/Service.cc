// Copyright (c) 2010
// All rights reserved.

#include <functional>
#include "Service.hh"
#include "Log.hh"
#include "boost/regex.hpp"

namespace track {

Service::Service(int argc, char* argv[]) {
  TRACK_TRACE <<"Service::Service()";

  cond_.reset(soil::STimer::create());

  config_.reset(new Config(argc, argv));

  pub_service_.reset(zod::PubService::create(config_->options()->pub_addr));

  trader_service_.reset(cata::TraderService::createService(config_->cataTraderOptions(), this));

  pull_service_.reset(zod::PullService::create(config_->options()->pull_addr, this));

}

Service::~Service() {
  TRACK_TRACE <<"Service::~Service()";
}

void Service::msgCallback(const zod::Msg* msg) {
  TRACK_TRACE <<"Service::msgCallback()";

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
    }
  }
}

void Service::onRspMessage(const std::string& msg) {
  TRACK_TRACE <<"Service::onRspMessage()";

  TRACK_DEBUG <<msg;

  pub_service_->sendMsg(msg);
}

void Service::onRtnMessage(const std::string& msg) {
  TRACK_TRACE <<"Service::onRtnMessage()";

  TRACK_DEBUG <<msg;
}

void Service::queryExchange(const json::Value& value) {
  TRACK_TRACE <<"Service::queryExchange()";

  std::string exchange;
  auto itr = value.FindMember("exchange");
  if (itr != value.MemberEnd()) {
    exchange = itr->value.GetString();
  }
  
  trader_service_->queryExchange(exchange);
}

void Service::queryInstrument(const json::Value& value) {
  TRACK_TRACE <<"Service::queryInstrument()";

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

void Service::queryInvestor() {
  TRACK_TRACE <<"Service::queryInvestor()";

  trader_service_->queryInvestor();
}

void Service::queryAccount() {
  TRACK_TRACE <<"Service::queryAccount()";

  trader_service_->queryAccount("");
}

void Service::queryTradingCode() {
  TRACK_TRACE <<"Service::queryTradingCode()";

  trader_service_->queryTradingCode("", "", cata::CIDT_Speculation);

  // trader_service_->queryTradingCode("", "", cata::CIDT_Arbitrage);

  // trader_service_->queryTradingCode("", "", cata::CIDT_Hedge);

}

void Service::queryOrder() {
  TRACK_TRACE <<"Service::queryOrder()";

  trader_service_->queryOrder("", "", "", "", "");
}

void Service::queryTrade() {
  TRACK_TRACE <<"Service::queryTrade()";

  trader_service_->queryTrade("", "", "", "", "");
}

void Service::queryPosition() {
  TRACK_TRACE <<"Service::queryPosition()";

  trader_service_->queryPosition("");
}

};  // namespace track
