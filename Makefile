.PHONY: all run test test-asan bench clean

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I include
GTEST = -isystem googletest/googletest/include -I googletest/googletest -pthread googletest/googletest/src/gtest-all.cc
TESTS = tests/run_tests.cpp $(wildcard tests/tests_*.cpp)

all: test

lab: src/main.cpp
	$(CXX) $(CXXFLAGS) $< -o lab

run: lab
	./lab

test: $(TESTS)
	$(CXX) $(CXXFLAGS) $(GTEST) $^ -o run_tests
	./run_tests

test-asan: $(TESTS)
	$(CXX) $(CXXFLAGS) -fsanitize=address,undefined -g -O1 -fno-omit-frame-pointer $(GTEST) $^ -o run_tests_asan
	./run_tests_asan

run_bench: bench/bench.cpp
	$(CXX) -std=c++17 -O2 -Wall -Wextra -I include $< -o run_bench

# make bench EXP=7 - замеры до 10^7 элементов, результаты в bench_results.csv и .png
EXP ?= 6
bench: run_bench
	./run_bench $(EXP) > bench_results.csv
	python3 bench/plot.py bench_results.csv bench_results.png

clean:
	rm -f lab run_tests run_tests_asan run_bench
