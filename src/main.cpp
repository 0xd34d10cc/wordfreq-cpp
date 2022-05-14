#include <cstdlib>
#include <iostream>

#include "mapped_file.hpp"
#include "count_words.hpp"


static char lower(char c) {
  return c | (1 << 5);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: wordfreq <path>" << std::endl;
    return EXIT_FAILURE;
  }

  const char* path = argv[1];
  MappedFile file;
  if (const auto ec = file.open(path)) {
    std::cerr << "Failed to open file: " << ec.message() << std::endl;
    return EXIT_FAILURE;
  }

  file.hint_sequential();
  const auto words = count_words(file.data(), file.size());
  std::string buffer;
  std::size_t n = 0;
  buffer.resize(1024, 0);

  for (auto [freq, word]: words) {
    n = word.len();
    if (n > buffer.size()) {
      buffer.resize(n, 0);
    }

    for (std::size_t i = 0; i < n; ++i) {
      buffer[i] = lower(word.data()[i]);
    }

    std::cout << freq << ' ';
    std::cout.write(buffer.data(), n);
    std::cout << '\n';
  }

  return EXIT_SUCCESS;
}