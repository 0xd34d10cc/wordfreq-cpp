CXXFLAGS=-Wall -Wextra -O3 -march=native -flto -fvisibility=hidden -std=c++17

wordfreq: src/main.cpp src/mapped_file.cpp src/count_words.cpp
	$(CXX) src/main.cpp src/mapped_file.cpp src/count_words.cpp $(CXXFLAGS) -o wordfreq

bench: src/bench.cpp src/mapped_file.cpp src/count_words.cpp
	$(CXX) src/bench.cpp src/mapped_file.cpp src/count_words.cpp $(CXXFLAGS) -fno-omit-frame-pointer -gdwarf -lbenchmark -o bench