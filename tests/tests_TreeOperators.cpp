#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "../include/BinarySearchTree.hpp"

using IntTree = BinarySearchTree<int>;

static IntTree MakeSample() {
    IntTree tree;
    for(int k : {8, 3, 10, 1, 6, 14}) { tree.Add(k); }
    return tree;
}

static IntTree MakeTree(std::initializer_list<int> keys) {
    IntTree tree;
    for(int k : keys) { tree.Add(k); }
    return tree;
}


TEST(TreeMerge, UnionOfValues) {
    IntTree merged = MakeSample() + MakeTree({5, 3, 20});
    EXPECT_EQ(merged.Size(), 8u);
    EXPECT_EQ(merged.ToString(Order::LKP), "1 3 5 6 8 10 14 20");
}

TEST(TreeMerge, DoesNotChangeOperands) {
    IntTree a = MakeSample();
    IntTree b = MakeTree({5, 20});
    IntTree merged = a.Merge(b);
    EXPECT_EQ(a.Size(), 6u);
    EXPECT_EQ(b.Size(), 2u);
    EXPECT_EQ(merged.Size(), 8u);
}

TEST(TreeMerge, WithEmpty) {
    IntTree empty;
    IntTree tree = MakeSample();
    EXPECT_TRUE((tree + empty) == tree);
    EXPECT_TRUE((empty + tree) == tree);
    EXPECT_TRUE((empty + empty).IsEmpty());
}

TEST(TreeMerge, SameTreeGivesSameValues) {
    IntTree tree = MakeSample();
    EXPECT_EQ((tree + tree).Size(), 6u);
}


TEST(TreeEquality, CopyIsEqual) {
    IntTree tree = MakeSample();
    IntTree copy(tree);
    EXPECT_TRUE(tree == copy);
    EXPECT_FALSE(tree != copy);
}

TEST(TreeEquality, DifferentValues) {
    EXPECT_TRUE(MakeSample() != MakeTree({8, 3, 10, 1, 6, 15}));
}

TEST(TreeEquality, DifferentSize) {
    EXPECT_TRUE(MakeSample() != MakeTree({8, 3, 10, 1, 6}));
}

TEST(TreeEquality, SameValuesDifferentShape) {
    IntTree chain = MakeTree({1, 3, 6, 8, 10, 14});
    EXPECT_TRUE(MakeSample() != chain);
    chain.Balance();
    EXPECT_EQ(chain.ToString(Order::LKP), MakeSample().ToString(Order::LKP));
}

TEST(TreeEquality, EmptyTrees) {
    IntTree a;
    IntTree b;
    EXPECT_TRUE(a == b);
    EXPECT_TRUE(a != MakeSample());
}


TEST(TreeStream, PrintsInPreOrder) {
    std::ostringstream out;
    out << MakeSample();
    EXPECT_EQ(out.str(), "8 3 1 6 10 14");
}

TEST(TreeStream, EmptyTreePrintsNothing) {
    std::ostringstream out;
    out << IntTree();
    EXPECT_EQ(out.str(), "");
}
