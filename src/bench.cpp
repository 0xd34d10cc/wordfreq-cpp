#include <iostream>

#include <benchmark/benchmark.h>

#include "mapped_file.hpp"
#include "count_words.hpp"


static void shakespeare(benchmark::State& state) {
  MappedFile file;
  if (const auto ec = file.open("shakespeare.txt")) {
    std::cerr << "Failed to open file: " << ec.message() << std::endl;
    std::abort();
  }

  file.hint_sequential();
  for (auto _: state) {
    count_words(file.data(), file.size());
  }

  state.SetBytesProcessed(state.iterations() * file.size());
}

BENCHMARK(shakespeare);
BENCHMARK_MAIN();