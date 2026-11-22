#include <gtest/gtest.h>
#include <complex>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include "../include/BinarySearchTree.hpp"

using IntTree = BinarySearchTree<int>;

// дерево: корень 8, слева 3 (дети 1 и 6), справа 10 (правый ребёнок 14)
static IntTree MakeSample() {
    IntTree tree;
    for(int k : {8, 3, 10, 1, 6, 14}) { tree.Add(k); }
    return tree;
}

struct ComplexLess {
    bool operator()(const std::complex<double>& a, const std::complex<double>& b) const {
        if(a.real() != b.real()) { return a.real() < b.real(); }
        return a.imag() < b.imag();
    }
};


TEST(TreeBasic, EmptyTree) {
    IntTree tree;
    EXPECT_EQ(tree.Size(), 0u);
    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_FALSE(tree.Contains(1));
    EXPECT_EQ(tree.Find(1), nullptr);
    EXPECT_EQ(tree.Height(), 0u);
    EXPECT_THROW(tree.Min(), std::out_of_range);
    EXPECT_THROW(tree.Max(), std::out_of_range);
}

TEST(TreeBasic, AddAndContains) {
    IntTree tree = MakeSample();
    EXPECT_EQ(tree.Size(), 6u);
    EXPECT_TRUE(tree.Contains(6));
    EXPECT_FALSE(tree.Contains(7));
}

TEST(TreeBasic, DuplicateIsIgnored) {
    IntTree tree;
    EXPECT_TRUE(tree.Add(5));
    EXPECT_FALSE(tree.Add(5));
    EXPECT_EQ(tree.Size(), 1u);
}

TEST(TreeBasic, FindReturnsPointer) {
    IntTree tree = MakeSample();
    ASSERT_NE(tree.Find(6), nullptr);
    EXPECT_EQ(*tree.Find(6), 6);
    EXPECT_EQ(tree.Find(7), nullptr);
}

TEST(TreeBasic, MinMaxHeight) {
    IntTree tree = MakeSample();
    EXPECT_EQ(tree.Min(), 1);
    EXPECT_EQ(tree.Max(), 14);
    EXPECT_EQ(tree.Height(), 3u);
}


TEST(TreeRemove, Leaf) {
    IntTree tree = MakeSample();
    tree.Remove(1);
    EXPECT_EQ(tree.Size(), 5u);
    EXPECT_FALSE(tree.Contains(1));
    EXPECT_EQ(tree.ToString(), "8 3 6 10 14");
}

TEST(TreeRemove, NodeWithOneChild) {
    IntTree tree = MakeSample();
    tree.Remove(10);
    EXPECT_EQ(tree.ToString(), "8 3 1 6 14");
}

TEST(TreeRemove, NodeWithTwoChildren) {
    IntTree tree = MakeSample();
    tree.Remove(3);
    EXPECT_EQ(tree.Size(), 5u);
    EXPECT_EQ(tree.ToString(), "8 6 1 10 14");
}

TEST(TreeRemove, Root) {
    IntTree tree = MakeSample();
    tree.Remove(8);
    EXPECT_EQ(tree.Size(), 5u);
    EXPECT_EQ(tree.ToString(), "10 3 1 6 14");

    tree.Add(8);
    EXPECT_TRUE(tree.Contains(8));
    EXPECT_EQ(tree.Size(), 6u);
}

TEST(TreeRemove, MissingThrows) {
    IntTree tree = MakeSample();
    EXPECT_THROW(tree.Remove(7), std::out_of_range);
    EXPECT_EQ(tree.Size(), 6u);
}

TEST(TreeRemove, FromEmptyThrows) {
    IntTree tree;
    EXPECT_THROW(tree.Remove(1), std::out_of_range);
}

TEST(TreeRemove, AllValues) {
    IntTree tree = MakeSample();
    for(int k : {8, 3, 10, 1, 6, 14}) { tree.Remove(k); }
    EXPECT_EQ(tree.Size(), 0u);
    EXPECT_TRUE(tree.IsEmpty());
    tree.Add(5);
    EXPECT_TRUE(tree.Contains(5));
}

TEST(TreeRemove, ClearEmptiesTree) {
    IntTree tree = MakeSample();
    tree.Clear();
    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_FALSE(tree.Contains(8));
}


TEST(TreeCopy, CopyIsIndependent) {
    IntTree tree = MakeSample();
    IntTree copy(tree);
    copy.Remove(3);
    copy.Add(100);
    EXPECT_TRUE(tree.Contains(3));
    EXPECT_FALSE(tree.Contains(100));
    EXPECT_EQ(tree.Size(), 6u);
    EXPECT_EQ(copy.Size(), 6u);
}

TEST(TreeCopy, Assignment) {
    IntTree tree = MakeSample();
    IntTree other;
    other.Add(1);
    other = tree;
    EXPECT_EQ(other.Size(), 6u);
    other.Remove(8);
    EXPECT_TRUE(tree.Contains(8));
}

TEST(TreeCopy, MoveLeavesSourceEmpty) {
    IntTree tree = MakeSample();
    IntTree moved(std::move(tree));
    EXPECT_EQ(moved.Size(), 6u);
    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_EQ(tree.GetRoot(), nullptr);
}


TEST(TreeTypes, Doubles) {
    BinarySearchTree<double> tree;
    for(double x : {2.5, 1.5, 3.5}) { tree.Add(x); }
    EXPECT_EQ(tree.ToString(Order::LKP), "1.5 2.5 3.5");
}

TEST(TreeTypes, Strings) {
    BinarySearchTree<std::string> tree;
    for(const char* s : {"pear", "apple", "kiwi"}) { tree.Add(s); }
    EXPECT_EQ(tree.ToString(Order::LKP), "apple kiwi pear");
    EXPECT_TRUE(tree.Contains("kiwi"));
    EXPECT_FALSE(tree.Contains("plum"));
}

TEST(TreeTypes, ComplexWithComparator) {
    BinarySearchTree<std::complex<double>, ComplexLess> tree;
    tree.Add({1, 2});
    tree.Add({1, -1});
    tree.Add({0, 5});
    EXPECT_EQ(tree.Size(), 3u);
    EXPECT_TRUE(tree.Contains({1, 2}));
    EXPECT_FALSE(tree.Contains({2, 1}));
    EXPECT_EQ(tree.Min(), std::complex<double>(0, 5));
}

TEST(TreeTypes, DescendingComparator) {
    BinarySearchTree<int, std::greater<int>> tree;
    for(int k : {3, 5, 1}) { tree.Add(k); }
    EXPECT_EQ(tree.ToString(Order::LKP), "5 3 1");
}


TEST(TreeBalance, SortedInsertGivesChain) {
    IntTree tree;
    for(int k = 1; k <= 1023; k++) { tree.Add(k); }
    EXPECT_EQ(tree.Height(), 1023u);
}

TEST(TreeBalance, BalanceReducesHeight) {
    IntTree tree;
    for(int k = 1; k <= 1023; k++) { tree.Add(k); }
    tree.Balance();
    EXPECT_EQ(tree.Height(), 10u);
    EXPECT_EQ(tree.Size(), 1023u);
}

TEST(TreeBalance, KeepsContentAndOrder) {
    IntTree tree = MakeSample();
    tree.Balance();
    EXPECT_EQ(tree.ToString(Order::LKP), "1 3 6 8 10 14");
    EXPECT_EQ(tree.Height(), 3u);
}

TEST(TreeBalance, EmptyAndSingle) {
    IntTree empty;
    empty.Balance();
    EXPECT_TRUE(empty.IsEmpty());

    IntTree single;
    single.Add(1);
    single.Balance();
    EXPECT_EQ(single.Size(), 1u);
    EXPECT_TRUE(single.Contains(1));
}
