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

static IntTree MakePattern(std::initializer_list<int> keys) {
    IntTree tree;
    for(int k : keys) { tree.Add(k); }
    return tree;
}


TEST(TreeExtract, CopiesSubtree) {
    IntTree tree = MakeSample();
    IntTree sub = tree.ExtractSubtree(3);
    EXPECT_EQ(sub.Size(), 3u);
    EXPECT_EQ(sub.ToString(), "3 1 6");
}

TEST(TreeExtract, LeafGivesSingleNode) {
    IntTree tree = MakeSample();
    IntTree sub = tree.ExtractSubtree(14);
    EXPECT_EQ(sub.Size(), 1u);
    EXPECT_TRUE(sub.Contains(14));
}

TEST(TreeExtract, RootGivesWholeTree) {
    IntTree tree = MakeSample();
    EXPECT_EQ(tree.ExtractSubtree(8).ToString(), tree.ToString());
}

TEST(TreeExtract, CopyIsIndependentFromOriginal) {
    IntTree tree = MakeSample();
    IntTree sub = tree.ExtractSubtree(3);
    sub.Remove(3);
    sub.Add(100);
    EXPECT_TRUE(tree.Contains(3));
    EXPECT_FALSE(tree.Contains(100));
    EXPECT_EQ(tree.Size(), 6u);
}

TEST(TreeExtract, MissingValueThrows) {
    IntTree tree = MakeSample();
    EXPECT_THROW(tree.ExtractSubtree(7), std::invalid_argument);
}

TEST(TreeExtract, FromEmptyThrows) {
    IntTree tree;
    EXPECT_THROW(tree.ExtractSubtree(1), std::invalid_argument);
}


TEST(TreeSubtree, FoundWithMatchingShape) {
    IntTree tree = MakeSample();
    EXPECT_TRUE(tree.ContainsSubtree(MakePattern({3, 1, 6})));
}

TEST(TreeSubtree, FoundLeaf) {
    IntTree tree = MakeSample();
    EXPECT_TRUE(tree.ContainsSubtree(MakePattern({14})));
}

TEST(TreeSubtree, NotFoundWhenChildIsMissingInPattern) {
    IntTree tree = MakeSample();
    EXPECT_FALSE(tree.ContainsSubtree(MakePattern({3, 1})));
}

TEST(TreeSubtree, NotFoundWhenPatternHasExtraChild) {
    IntTree tree = MakeSample();
    EXPECT_FALSE(tree.ContainsSubtree(MakePattern({10, 9, 14})));
}

TEST(TreeSubtree, NotFoundWhenRootIsMissing) {
    IntTree tree = MakeSample();
    EXPECT_FALSE(tree.ContainsSubtree(MakePattern({7})));
}

TEST(TreeSubtree, EmptyPatternIsAlwaysFound) {
    IntTree tree = MakeSample();
    IntTree empty;
    EXPECT_TRUE(tree.ContainsSubtree(empty));
    EXPECT_TRUE(empty.ContainsSubtree(empty));
}

TEST(TreeSubtree, EmptyTreeDoesNotContainNonEmpty) {
    IntTree tree;
    EXPECT_FALSE(tree.ContainsSubtree(MakePattern({1})));
}

TEST(TreeSubtree, WholeTreeAndExtracted) {
    IntTree tree = MakeSample();
    EXPECT_TRUE(tree.ContainsSubtree(tree.ExtractSubtree(8)));
    EXPECT_TRUE(tree.ContainsSubtree(tree.ExtractSubtree(10)));
}
