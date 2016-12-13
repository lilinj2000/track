// Copyright (c) 2010
// All rights reserved.

#include "Server.hh"
#include "Log.hh"

namespace track {

Server::Server(int argc, char* argv[]) {
  TRACK_TRACE <<"Server::Server()";

  cond_.reset(soil::STimer::create());

  config_.reset(new Config(argc, argv));

  trader_service_.reset(cata::TraderService::createService(config_->cataTraderOptions(), this));

  go();
}

Server::~Server() {
  TRACK_TRACE <<"Server::~Server()";
}

void Server::onRspMessage(const std::string& msg) {
  TRACK_TRACE <<"Server::onRspMessage()";

  TRACK_INFO <<msg;
}

void Server::onRtnMessage(const std::string& msg) {
  TRACK_TRACE <<"Server::onRtnMessage()";

  TRACK_INFO <<msg;
}

void Server::go() {
  TRACK_TRACE <<"Server::go()";

  // queryInvestor();
  // queryExchange();
  queryInstrument();
}

void Server::queryInvestor() {
  TRACK_TRACE <<"Server::queryInvestor()";

  wait();
  trader_service_->queryInvestor();
}

void Server::queryExchange() {
  TRACK_TRACE <<"Server::queryExchange()";

  wait();
  trader_service_->queryExchange("");
}

void Server::queryInstrument() {
  TRACK_TRACE <<"Server::queryInstrument()";

  wait();
  trader_service_->queryInstrument("", "", "", "");
}

};  // namespace track
