$CXX src/bench.cpp                               \
     src/mapped_file.cpp                         \
     src/count_words.cpp                         \
     -Wall -Wextra                               \
     -O3 -march=native -flto -fvisibility=hidden \
     -fno-omit-frame-pointer                     \
     -gdwarf                                     \
     -std=c++17                                  \
     -lbenchmark                                 \
     -o bench
