$CXX src/main.cpp                                \
     src/mapped_file.cpp                         \
     src/count_words.cpp                         \
     -O3 -march=native -flto -fvisibility=hidden \
     -std=c++17                                  \
     -o wordfreq