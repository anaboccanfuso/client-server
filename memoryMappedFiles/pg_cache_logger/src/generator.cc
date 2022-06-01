// Copyright 2022 CSCE 311
//

#include <pg_cache_logger/inc/producer.h>

#include <cstddef>
#include <iostream>

int main(int argc, char* argv[]) {

  assert(argc == 2 && "usage: text-client <file_name>");

  logger::Producer log_writer;

  log_writer.Produce(argv[1]);

  return 0;
}
