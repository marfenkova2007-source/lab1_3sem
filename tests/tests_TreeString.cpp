#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include "../include/BinarySearchTree.hpp"

using IntTree = BinarySearchTree<int>;

static IntTree MakeSample() {
    IntTree tree;
    for(int k : {8, 3, 10, 1, 6, 14}) { tree.Add(k); }
    return tree;
}

static IntTree MakeSmall() {
    IntTree tree;
    for(int k : {2, 1, 3}) { tree.Add(k); }
    return tree;
}


TEST(TreeTraversal, AllSixOrders) {
    IntTree tree = MakeSample();
    EXPECT_EQ(tree.ToString(Order::KLP), "8 3 1 6 10 14");
    EXPECT_EQ(tree.ToString(Order::KPL), "8 10 14 3 6 1");
    EXPECT_EQ(tree.ToString(Order::LPK), "1 6 3 14 10 8");
    EXPECT_EQ(tree.ToString(Order::LKP), "1 3 6 8 10 14");
    EXPECT_EQ(tree.ToString(Order::PLK), "14 10 6 1 3 8");
    EXPECT_EQ(tree.ToString(Order::PKL), "14 10 8 6 3 1");
}

TEST(TreeTraversal, EmptyTreeGivesEmptyString) {
    IntTree tree;
    EXPECT_EQ(tree.ToString(), "");
}

TEST(TreeTraversal, VisitorSeesEveryNodeOnce) {
    IntTree tree = MakeSample();
    int sum = 0;
    int visited = 0;
    tree.Traverse(Order::PKL, [&](const int& x) { sum += x; visited++; });
    EXPECT_EQ(sum, 42);
    EXPECT_EQ(visited, 6);
}


TEST(TreeFixedString, RoundTripKeepsShape) {
    IntTree tree = MakeSample();
    IntTree restored = IntTree::FromString(tree.ToString());
    EXPECT_EQ(restored.ToString(), tree.ToString());
    EXPECT_TRUE(restored.ContainsSubtree(tree));
}

TEST(TreeFixedString, EmptyString) {
    EXPECT_TRUE(IntTree::FromString("").IsEmpty());
}

TEST(TreeFixedString, BadValueThrows) {
    EXPECT_THROW(IntTree::FromString("1 2 x"), std::invalid_argument);
}


TEST(TreeFormat, SaveWithBraces) {
    IntTree tree = MakeSmall();
    EXPECT_EQ(tree.ToString("{K}(L)[P]"), "{2}({1}()[])[{3}()[]]");
}

TEST(TreeFormat, SaveWithAnotherOrder) {
    IntTree tree = MakeSmall();
    EXPECT_EQ(tree.ToString("(L)[K]{P}"), "(()[1]{})[2]{()[3]{}}");
}

TEST(TreeFormat, CyrillicLetters) {
    IntTree tree = MakeSmall();
    EXPECT_EQ(tree.ToString("{К}(Л)[П]"), tree.ToString("{K}(L)[P]"));
}

TEST(TreeFormat, EmptyTreeGivesEmptyString) {
    IntTree tree;
    EXPECT_EQ(tree.ToString("{K}(L)[P]"), "");
}

TEST(TreeFormat, RoundTripForDifferentFormats) {
    IntTree tree = MakeSample();
    for(const char* format : {"{K}(L)[P]", "(L)[K]{P}", "[P]{L}(K)", "{L][K}(P)"}) {
        IntTree restored = IntTree::FromString(tree.ToString(format), format);
        EXPECT_EQ(restored.Size(), 6u);
        EXPECT_EQ(restored.ToString(), tree.ToString());
        EXPECT_TRUE(restored.ContainsSubtree(tree));
    }
}

TEST(TreeFormat, BadFormatThrows) {
    IntTree tree = MakeSmall();
    EXPECT_THROW(tree.ToString("{K}(L)"), std::invalid_argument);
    EXPECT_THROW(tree.ToString("{K}(K)[P]"), std::invalid_argument);
    EXPECT_THROW(tree.ToString("{K}(L)[X]"), std::invalid_argument);
    EXPECT_THROW(tree.ToString("K(L)[P]"), std::invalid_argument);
    EXPECT_THROW(tree.ToString("{K}(L)[P]x"), std::invalid_argument);
}

TEST(TreeFormat, BrokenStringThrows) {
    EXPECT_THROW(IntTree::FromString("{2}({1}()[])[{3}()[]", "{K}(L)[P]"), std::invalid_argument);
    EXPECT_THROW(IntTree::FromString("{2}()[]{", "{K}(L)[P]"), std::invalid_argument);
    EXPECT_THROW(IntTree::FromString("{x}()[]", "{K}(L)[P]"), std::invalid_argument);
    EXPECT_THROW(IntTree::FromString("{2}(){}", "{K}(L)[P]"), std::invalid_argument);
}

TEST(TreeFormat, NotSearchTreeThrows) {
    EXPECT_THROW(IntTree::FromString("{1}({5}()[])[]", "{K}(L)[P]"), std::invalid_argument);
}
