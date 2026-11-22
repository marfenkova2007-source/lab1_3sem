#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include "BinarySearchTree.hpp"

using IntTree = BinarySearchTree<int>;
using Clock = std::chrono::steady_clock;

volatile long long sink = 0;


double Seconds(Clock::time_point start) {
    return std::chrono::duration<double>(Clock::now() - start).count();
}

// строка вывода
void Report(const std::string& operation, size_t n, double seconds) {
    std::cout << operation << "," << n << "," << seconds << std::endl;
}

void RunBenchmark(size_t n, std::mt19937& generator) {
    std::vector<int> values(n);
    std::iota(values.begin(), values.end(), 0);
    std::shuffle(values.begin(), values.end(), generator);

    IntTree tree;
    Clock::time_point start = Clock::now();
    for(int value : values) { tree.Add(value); }
    Report("Add", n, Seconds(start));

    std::shuffle(values.begin(), values.end(), generator);
    size_t found = 0;
    start = Clock::now();
    for(int value : values) { if(tree.Contains(value)) { found++; } }
    Report("Contains", n, Seconds(start));
    sink = static_cast<long long>(found);

    start = Clock::now();
    long long sum = tree.Reduce<long long>([](int x, long long acc) { return acc + x; }, 0LL);
    Report("Reduce", n, Seconds(start));
    sink = sum;

    {
        start = Clock::now();
        IntTree mapped = tree.Map([](int x) { return x + 1; });
        Report("Map", n, Seconds(start));
        sink = static_cast<long long>(mapped.Size());
    }
    {
        start = Clock::now();
        IntTree even = tree.Where([](int x) { return x % 2 == 0; });
        Report("Where", n, Seconds(start));
        sink = static_cast<long long>(even.Size());
    }

    // удаляем половину значений, потом возвращаем их (это в замер не входит)
    start = Clock::now();
    for(size_t i = 0; i < n / 2; i++) { tree.Remove(values[i]); }
    Report("Remove (n/2)", n, Seconds(start));
    for(size_t i = 0; i < n / 2; i++) { tree.Add(values[i]); }

    start = Clock::now();
    tree.Balance();
    Report("Balance", n, Seconds(start));
    sink = static_cast<long long>(tree.Height());
}


int main(int argc, char** argv) {
    int maxExp = 6;
    if(argc > 1) { maxExp = std::atoi(argv[1]); }
    if(maxExp < 4 || maxExp > 8) {
        std::cerr << "Использование: ./run_bench [степень десяти от 4 до 8]\n";
        return 1;
    }

    size_t limit = 1;
    for(int i = 0; i < maxExp; i++) { limit *= 10; }
    std::vector<size_t> sizes;
    size_t power = 10000;
    for(int e = 4; e <= maxExp; e++) {
        for(size_t multiplier : {1, 2, 5}) {
            if(multiplier * power <= limit) { sizes.push_back(multiplier * power); }
        }
        power *= 10;
    }

    std::mt19937 generator(12345);
    std::cout << "operation,n,seconds" << std::endl;
    for(size_t n : sizes) {
        std::cerr << "n = " << n << "\n";
        RunBenchmark(n, generator);
    }
    return 0;
}
