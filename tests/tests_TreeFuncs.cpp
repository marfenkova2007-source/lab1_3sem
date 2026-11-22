#include <gtest/gtest.h>
#include <string>
#include "../include/BinarySearchTree.hpp"

using IntTree = BinarySearchTree<int>;

static IntTree MakeSample() {
    IntTree tree;
    for(int k : {8, 3, 10, 1, 6, 14}) { tree.Add(k); }
    return tree;
}

static int Double(int x) { return x * 2; }
static bool IsEven(int x) { return x % 2 == 0; }
static bool IsGreaterThan100(int x) { return x > 100; }
static int AddToSum(int x, int acc) { return x + acc; }


TEST(TreeMap, DoesNotChangeOriginal) {
    IntTree tree = MakeSample();
    IntTree doubled = tree.Map(Double);
    EXPECT_EQ(doubled.ToString(Order::LKP), "2 6 12 16 20 28");
    EXPECT_EQ(tree.ToString(Order::LKP), "1 3 6 8 10 14");
}

TEST(TreeMap, EmptyTree) {
    IntTree tree;
    EXPECT_EQ(tree.Map(Double).Size(), 0u);
}

TEST(TreeMap, ChangesElementType) {
    IntTree tree = MakeSample();
    BinarySearchTree<std::string> result = tree.Map([](int x) { return std::to_string(x); });
    EXPECT_EQ(result.Size(), 6u);
    EXPECT_TRUE(result.Contains("14"));
}

TEST(TreeMap, EqualResultsMerge) {
    IntTree tree = MakeSample();
    IntTree parity = tree.Map([](int x) { return x % 2; });
    EXPECT_EQ(parity.Size(), 2u);
}

TEST(TreeMap, DecreasingFunctionStillSearchTree) {
    IntTree tree = MakeSample();
    IntTree negated = tree.Map([](int x) { return -x; });
    EXPECT_EQ(negated.ToString(Order::LKP), "-14 -10 -8 -6 -3 -1");
}


TEST(TreeWhere, FiltersEven) {
    IntTree tree = MakeSample();
    IntTree even = tree.Where(IsEven);
    EXPECT_EQ(even.Size(), 4u);
    EXPECT_EQ(even.ToString(Order::LKP), "6 8 10 14");
}

TEST(TreeWhere, DoesNotChangeOriginal) {
    IntTree tree = MakeSample();
    IntTree even = tree.Where(IsEven);
    EXPECT_EQ(tree.Size(), 6u);
    EXPECT_TRUE(tree.Contains(3));
}

TEST(TreeWhere, NothingMatches) {
    IntTree tree = MakeSample();
    EXPECT_EQ(tree.Where(IsGreaterThan100).Size(), 0u);
}

TEST(TreeWhere, EverythingMatches) {
    IntTree tree = MakeSample();
    IntTree all = tree.Where([](int) { return true; });
    EXPECT_EQ(all.Size(), 6u);
    EXPECT_EQ(all.ToString(Order::LKP), tree.ToString(Order::LKP));
}

TEST(TreeWhere, ResultIsBalanced) {
    IntTree tree;
    for(int k = 1; k <= 2000; k++) { tree.Add(k); }
    IntTree odd = tree.Where([](int x) { return x % 2 == 1; });
    EXPECT_EQ(odd.Size(), 1000u);
    EXPECT_EQ(odd.Height(), 10u);
    EXPECT_TRUE(odd.Contains(1999));
    EXPECT_FALSE(odd.Contains(2));
}


TEST(TreeReduce, Sum) {
    IntTree tree = MakeSample();
    EXPECT_EQ(tree.Reduce<int>(AddToSum, 0), 42);
}

TEST(TreeReduce, EmptyTreeReturnsStartValue) {
    IntTree tree;
    EXPECT_EQ(tree.Reduce<int>(AddToSum, 42), 42);
}

// f(x, acc) = 2x + 3acc, f(3, f(2, f(1, 4))) = 144
TEST(TreeReduce, MethodFunc) {
    IntTree tree;
    for(int k : {1, 2, 3}) { tree.Add(k); }
    int result = tree.Reduce<int>([](int x, int acc) { return 2 * x + 3 * acc; }, 4);
    EXPECT_EQ(result, 144);
}

TEST(TreeReduce, OrderMatters) {
    IntTree tree = MakeSample();
    auto concat = [](int x, std::string acc) { return acc + std::to_string(x) + ","; };
    EXPECT_EQ(tree.Reduce<std::string>(concat, std::string(), Order::LKP), "1,3,6,8,10,14,");
    EXPECT_EQ(tree.Reduce<std::string>(concat, std::string(), Order::KLP), "8,3,1,6,10,14,");
}
