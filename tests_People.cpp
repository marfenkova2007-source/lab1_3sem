#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include "../include/BinarySearchTree.hpp"
#include "../include/Person.hpp"

using StudentTree = BinarySearchTree<Student, PersonIdLess>;
using TeacherTree = BinarySearchTree<Teacher, PersonIdLess>;

static Student MakeStudent(int series, int number, const std::string& lastName, const std::string& group) {
    return Student(PersonID(series, number), "Ivan", "Ivanovich", lastName, Date(2005, 3, 14), group, 1000 + number % 1000);
}

// человек только с нужным ID, чтобы искать и удалять по идентификатору
static Student Probe(int series, int number) {
    return MakeStudent(series, number, "?", "?");
}

static Teacher MakeTeacher(int series, int number, const std::string& lastName, const std::string& department) {
    return Teacher(PersonID(series, number), "Petr", "Petrovich", lastName, Date(1975, 9, 1), department, "docent");
}

static StudentTree MakeSample() {
    StudentTree tree;
    tree.Add(MakeStudent(4501, 123456, "Ivanov", "A"));
    tree.Add(MakeStudent(4502, 111111, "Petrov", "B"));
    tree.Add(MakeStudent(4500, 999999, "Sidorov", "B"));
    tree.Add(MakeStudent(4503, 222222, "Smirnov", "A"));
    return tree;
}

static std::string LastNames(const StudentTree& tree) {
    return tree.Reduce<std::string>(
        [](const Student& s, std::string acc) { return acc + s.GetLastName() + ","; },
        std::string(""));
}


TEST(PersonData, DateValidation) {
    EXPECT_NO_THROW(Date(2024, 2, 29));
    EXPECT_THROW(Date(2023, 2, 29), std::invalid_argument);
    EXPECT_THROW(Date(2024, 13, 1), std::invalid_argument);
    EXPECT_THROW(Date(2024, 4, 31), std::invalid_argument);
    EXPECT_THROW(Date(2024, 1, 0), std::invalid_argument);
    EXPECT_EQ(Date(2005, 3, 4).ToString(), "04.03.2005");
}

TEST(PersonData, IdValidationAndOrder) {
    EXPECT_THROW(PersonID(10000, 1), std::invalid_argument);
    EXPECT_THROW(PersonID(-1, 1), std::invalid_argument);
    EXPECT_THROW(PersonID(1, 1000000), std::invalid_argument);
    EXPECT_TRUE(PersonID(4500, 999999) < PersonID(4501, 1));
    EXPECT_TRUE(PersonID(4501, 1) < PersonID(4501, 2));
    EXPECT_TRUE(PersonID(1, 2) == PersonID(1, 2));
    EXPECT_TRUE(PersonID(1, 2) != PersonID(1, 3));
}

TEST(PersonData, PersonValidationAndFullName) {
    PersonID id(4501, 123456);
    Date birth(2000, 1, 1);
    EXPECT_THROW(Person(id, "", "A", "B", birth), std::invalid_argument);
    EXPECT_THROW(Person(id, "A", "B", "", birth), std::invalid_argument);
    EXPECT_EQ(Person(id, "Иван", "Иванович", "Иванов", birth).GetFullName(), "Иван Иванович Иванов");
    EXPECT_EQ(Person(id, "Иван", "", "Иванов", birth).GetFullName(), "Иван Иванов");
}

TEST(PersonData, StudentAndTeacherValidation) {
    PersonID id(1, 1);
    Date birth(2000, 1, 1);
    EXPECT_THROW(Student(id, "A", "B", "C", birth, "", 1), std::invalid_argument);
    EXPECT_THROW(Student(id, "A", "B", "C", birth, "G", 0), std::invalid_argument);
    EXPECT_THROW(Teacher(id, "A", "B", "C", birth, "", "docent"), std::invalid_argument);
    EXPECT_THROW(Teacher(id, "A", "B", "C", birth, "D", ""), std::invalid_argument);
}

TEST(PersonData, StreamOutput) {
    std::ostringstream out;
    out << Student(PersonID(4501, 123456), "Иван", "Иванович", "Иванов", Date(2005, 3, 14), "ИУ7-31", 1001);
    EXPECT_EQ(out.str(), "Иван Иванович Иванов (4501 123456), группа ИУ7-31");
}


TEST(StudentTreeBasic, OrderedByIdNotByName) {
    StudentTree tree = MakeSample();
    EXPECT_EQ(tree.Size(), 4u);
    EXPECT_EQ(LastNames(tree), "Sidorov,Ivanov,Petrov,Smirnov,");
}

TEST(StudentTreeBasic, DuplicateIdIsIgnored) {
    StudentTree tree = MakeSample();
    EXPECT_FALSE(tree.Add(MakeStudent(4501, 123456, "Other", "C")));
    EXPECT_EQ(tree.Size(), 4u);
    EXPECT_EQ(tree.Find(Probe(4501, 123456))->GetLastName(), "Ivanov");
}

TEST(StudentTreeBasic, FindByIdAndMissing) {
    StudentTree tree = MakeSample();
    ASSERT_NE(tree.Find(Probe(4502, 111111)), nullptr);
    EXPECT_EQ(tree.Find(Probe(4502, 111111))->GetLastName(), "Petrov");
    EXPECT_EQ(tree.Find(Probe(1, 1)), nullptr);
    EXPECT_TRUE(tree.Contains(Probe(4503, 222222)));
}

TEST(StudentTreeBasic, RemoveById) {
    StudentTree tree = MakeSample();
    tree.Remove(Probe(4501, 123456));
    EXPECT_EQ(tree.Size(), 3u);
    EXPECT_FALSE(tree.Contains(Probe(4501, 123456)));
    EXPECT_THROW(tree.Remove(Probe(1, 1)), std::out_of_range);
}

TEST(StudentTreeBasic, CopyIsIndependent) {
    StudentTree tree = MakeSample();
    StudentTree copy(tree);
    EXPECT_TRUE(tree == copy);
    copy.Remove(Probe(4501, 123456));
    EXPECT_EQ(tree.Size(), 4u);
    EXPECT_TRUE(tree != copy);
}

TEST(StudentTreeFuncs, WhereGroup) {
    StudentTree tree = MakeSample();
    StudentTree groupA = tree.Where([](const Student& s) { return s.GetGroup() == "A"; });
    EXPECT_EQ(groupA.Size(), 2u);
    EXPECT_EQ(LastNames(groupA), "Ivanov,Smirnov,");
    EXPECT_EQ(tree.Size(), 4u);
}

TEST(StudentTreeFuncs, MapToNames) {
    StudentTree tree = MakeSample();
    BinarySearchTree<std::string> names = tree.Map([](const Student& s) { return s.GetLastName(); });
    EXPECT_EQ(names.ToString(Order::LKP), "Ivanov Petrov Sidorov Smirnov");
}

TEST(StudentTreeFuncs, ExtractAndContainsSubtree) {
    StudentTree tree = MakeSample();
    StudentTree sub = tree.ExtractSubtree(Probe(4502, 111111));
    EXPECT_EQ(LastNames(sub), "Petrov,Smirnov,");
    EXPECT_TRUE(tree.ContainsSubtree(sub));
    EXPECT_THROW(tree.ExtractSubtree(Probe(1, 1)), std::invalid_argument);
}

TEST(StudentTreeFuncs, BalanceKeepsOrder) {
    StudentTree tree = MakeSample();
    tree.Balance();
    EXPECT_EQ(LastNames(tree), "Sidorov,Ivanov,Petrov,Smirnov,");
}


TEST(TeacherTree, BasicOperations) {
    TeacherTree tree;
    tree.Add(MakeTeacher(100, 3, "Kuznetsov", "IU7"));
    tree.Add(MakeTeacher(100, 1, "Orlov", "IU6"));
    tree.Add(MakeTeacher(100, 2, "Volkov", "IU7"));
    EXPECT_EQ(tree.Size(), 3u);
    EXPECT_EQ(tree.Min().GetLastName(), "Orlov");
    EXPECT_EQ(tree.Max().GetLastName(), "Kuznetsov");

    TeacherTree iu7 = tree.Where([](const Teacher& t) { return t.GetDepartment() == "IU7"; });
    EXPECT_EQ(iu7.Size(), 2u);
}
