// Copyright (c) 2010
// All rights reserved.

#include "Server.hh"
#include "Log.hh"
#include "boost/regex.hpp"

namespace track {

Server::Server(int argc, char* argv[]) {
  TRACK_TRACE <<"Server::Server()";

  cond_.reset(soil::STimer::create());

  config_.reset(new Config(argc, argv));

  db_.reset(new cppdb::session(config_->options()->connection_string));

  trader_service_.reset(cata::TraderService::createService(config_->cataTraderOptions(), this));

  go();
}

Server::~Server() {
  TRACK_TRACE <<"Server::~Server()";
}

void Server::onRspMessage(const std::string& msg) {
  TRACK_TRACE <<"Server::onRspMessage()";

  TRACK_INFO <<msg;
  json::Document doc;
  doc.Parse(msg.data());

  auto itr = doc.MemberBegin();
  std::string key = itr->name.GetString();
  json::Value& data = doc[key.data()];

  // OnRspQryInvestor
  boost::regex re_qry("^OnRspQry(.*)$");
  boost::smatch mat;
  if (boost::regex_match(key, mat, re_qry)) {
    std::string t_name = mat[1];

    boost::regex re_field("^CThostFtdc(.*)Field$");
    for (auto itr = data.MemberBegin();
         itr != data.MemberEnd(); ++itr) {
      boost::smatch mat;
      std::string key = itr->name.GetString();
      if (boost::regex_match(key, mat, re_field)) {
        json::Value& f_data = data[key.data()];

        std::string create_sql;
        std::string insert_sql;
        sqlString(t_name, f_data, create_sql, insert_sql);
        // TRACK_DEBUG <<"create sql: " <<create_sql;
        // TRACK_DEBUG <<"insert sql: " <<insert_sql;

        try {
          (*db_) <<create_sql <<cppdb::exec;
          (*db_) <<insert_sql <<cppdb::exec;
        } catch (std::exception const &e) {
          TRACK_ERROR <<e.what();
        }
      }
    }

  }
}

void Server::onRtnMessage(const std::string& msg) {
  TRACK_TRACE <<"Server::onRtnMessage()";

  TRACK_INFO <<msg;
}

void Server::go() {
  TRACK_TRACE <<"Server::go()";

  queryInvestor();

  queryExchange();

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

void Server::sqlString(const std::string& name,
                       json::Value& data,
                       std::string& create_sql,
                       std::string& insert_sql) {
  TRACK_TRACE <<"Server::sqlString()";

  create_sql = "CREATE TABLE IF NOT EXISTS " + name + " (";

  insert_sql = "INSERT INTO " + name + " VALUES(";

  bool first = true;
  for (auto itr = data.MemberBegin();
       itr != data.MemberEnd(); ++itr) {
    if (!first) {
      create_sql += ", ";
      insert_sql += ", ";
    } else {
      first = false;
    }
    create_sql += itr->name.GetString();
    create_sql += " TEXT";

    insert_sql += "'";
    insert_sql += itr->value.GetString();
    insert_sql += "'";
  }
  create_sql += ");";
  insert_sql += ");";

  return;
}

};  // namespace track
