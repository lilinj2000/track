// Copyright (c) 2010
// All rights reserved.

#include <memory>
#include "Service.hh"
#include "soil/Pause.hh"

int main(int argc, char* argv[]) {
  std::unique_ptr<track::Service> service;
  service.reset(new track::Service(argc, argv));

  std::unique_ptr<soil::Pause> pause(soil::Pause::create());

  return 0;
}
