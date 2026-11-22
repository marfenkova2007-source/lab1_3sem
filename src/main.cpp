#include <cstdlib>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include "BinarySearchTree.hpp"
#include "Person.hpp"

using IntTree = BinarySearchTree<int>;
using StudentTree = BinarySearchTree<Student, PersonIdLess>;
using TeacherTree = BinarySearchTree<Teacher, PersonIdLess>;


int ReadInt(const std::string& text) {
    int number;
    std::cout << text;
    while(!(std::cin >> number)) {
        if(std::cin.eof()) { std::exit(0); }
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Необходимо ввести целое число: ";
    }
    std::cin.ignore(10000, '\n');
    return number;
}

std::string ReadLine(const std::string& text) {
    std::string line;
    std::cout << text;
    if(!std::getline(std::cin, line)) { std::exit(0); }
    return line;
}

Order ReadOrder() {
    std::cout << "1. КЛП  2. КПЛ  3. ЛПК  4. ЛКП  5. ПЛК  6. ПКЛ\n";
    int choice = ReadInt("Обход: ");
    if(choice < 1 || choice > 6) { throw std::invalid_argument("Нет такого обхода"); }
    return static_cast<Order>(choice - 1);
}


void PrintNode(const Node<int>* node, int depth) {
    if(node == nullptr) { return; }

    PrintNode(node->right, depth + 1);
    std::cout << std::string(depth * 6, ' ') << node->value << "\n";
    PrintNode(node->left, depth + 1);
}

void PrintTree(const IntTree& tree) {
    if(tree.IsEmpty()) {
        std::cout << "Дерево пустое\n";
        return;
    }
    // большое дерево на экран не выводим
    if(tree.Size() > 40) {
        std::cout << "Элементов: " << tree.Size() << ", высота: " << tree.Height() << "\n";
        return;
    }
    PrintNode(tree.GetRoot(), 0);
}

void PrintMenu() {
    std::cout << "\n===== Бинарное дерево поиска =====\n";
    std::cout << " 1. Добавить\n";
    std::cout << " 2. Удалить\n";
    std::cout << " 3. Найти\n";
    std::cout << " 4. Показать дерево\n";
    std::cout << " 5. Обход\n";
    std::cout << " 6. Map\n";
    std::cout << " 7. Where\n";
    std::cout << " 8. Reduce\n";
    std::cout << " 9. Балансировка\n";
    std::cout << "10. Извлечь поддерево\n";
    std::cout << "11. Проверить вхождение поддерева\n";
    std::cout << "12. Сохранить в строку (по формату)\n";
    std::cout << "13. Прочитать из строки (по формату)\n";
    std::cout << "14. Заполнить случайными числами\n";
    std::cout << "15. Слияние с другим деревом\n";
    std::cout << "16. Демонстрация: студенты и преподаватели\n";
    std::cout << " 0. Выход\n";
}


void DoAdd(IntTree& tree) {
    int value = ReadInt("Значение: ");
    if(tree.Add(value)) { std::cout << "Добавлено\n"; }
    else { std::cout << "Такое значение уже есть\n"; }
    PrintTree(tree);
}

void DoRemove(IntTree& tree) {
    int value = ReadInt("Значение для удаления: ");
    tree.Remove(value);
    std::cout << "Удалено\n";
    PrintTree(tree);
}

void DoFind(const IntTree& tree) {
    int value = ReadInt("Значение для поиска: ");
    if(tree.Find(value) != nullptr) { std::cout << "Найдено\n"; }
    else { std::cout << "Такого значения нет\n"; }
}

void DoTraversal(const IntTree& tree) {
    Order order = ReadOrder();
    std::cout << OrderToString(order) << ": " << tree.ToString(order) << "\n";
}

void DoMap(const IntTree& tree) {
    std::cout << "1. x * 2   2. x + 1   3. -x\n";
    int choice = ReadInt("Выбор: ");
    if(choice < 1 || choice > 3) {
        std::cout << "Нет такого пункта\n";
        return;
    }

    std::cout << "Результат Map (исходное дерево не изменилось):\n";
    if(choice == 1) { PrintTree(tree.Map([](int x) { return x * 2; })); }
    else if(choice == 2) { PrintTree(tree.Map([](int x) { return x + 1; })); }
    else { PrintTree(tree.Map([](int x) { return -x; })); }
}

void DoWhere(const IntTree& tree) {
    std::cout << "1. чётные   2. нечётные   3. больше 10\n";
    int choice = ReadInt("Выбор: ");
    if(choice < 1 || choice > 3) {
        std::cout << "Нет такого пункта\n";
        return;
    }

    std::cout << "Результат Where (исходное дерево не изменилось):\n";
    if(choice == 1) { PrintTree(tree.Where([](int x) { return x % 2 == 0; })); }
    else if(choice == 2) { PrintTree(tree.Where([](int x) { return x % 2 != 0; })); }
    else { PrintTree(tree.Where([](int x) { return x > 10; })); }
}

void DoReduce(const IntTree& tree) {
    std::cout << "1. сумма   2. произведение\n";
    int choice = ReadInt("Выбор: ");

    long long answer = 0;
    if(choice == 1) { answer = tree.Reduce<long long>([](int x, long long acc) { return acc + x; }, 0LL); }
    else if(choice == 2) { answer = tree.Reduce<long long>([](int x, long long acc) { return acc * x; }, 1LL); }
    else {
        std::cout << "Нет такого пункта\n";
        return;
    }
    std::cout << "Результат: " << answer << "\n";
}

void DoBalance(IntTree& tree) {
    std::cout << "Высота до: " << tree.Height() << "\n";
    tree.Balance();
    std::cout << "Высота после: " << tree.Height() << "\n";
    PrintTree(tree);
}

void DoExtract(const IntTree& tree) {
    int value = ReadInt("Значение в корне поддерева: ");
    IntTree sub = tree.ExtractSubtree(value);
    std::cout << "Извлечённое поддерево:\n";
    PrintTree(sub);
}

void DoContains(const IntTree& tree) {
    // шаблон задаётся значениями в порядке добавления, например: 3 1 6
    std::string line = ReadLine("Введите значения шаблона через пробел: ");
    IntTree pattern = IntTree::FromString(line);

    std::cout << "Шаблон:\n";
    PrintTree(pattern);
    if(tree.ContainsSubtree(pattern)) { std::cout << "Шаблон входит в дерево как поддерево\n"; }
    else { std::cout << "Шаблон НЕ входит в дерево как поддерево\n"; }
}

void DoSave(const IntTree& tree) {
    std::string format = ReadLine("Формат (например {K}(L)[P]): ");
    std::cout << tree.ToString(format) << "\n";
}

void DoLoad(IntTree& tree) {
    std::string text = ReadLine("Строка: ");
    std::string format = ReadLine("Формат (например {K}(L)[P]): ");
    tree = IntTree::FromString(text, format);
    std::cout << "Прочитано:\n";
    PrintTree(tree);
}

void DoFill(IntTree& tree) {
    int amount = ReadInt("Сколько чисел добавить: ");
    int maxValue = ReadInt("Числа от 0 до: ");
    if(amount <= 0 || maxValue < 0) { throw std::invalid_argument("Неверные параметры"); }

    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(0, maxValue);
    for(int i = 0; i < amount; i++) { tree.Add(distribution(generator)); }
    PrintTree(tree);
}


void DoMerge(IntTree& tree) {
    std::string line = ReadLine("Значения второго дерева через пробел: ");
    IntTree other = IntTree::FromString(line);

    std::cout << "Второе дерево:\n";
    PrintTree(other);
    tree = tree + other;
    std::cout << "Результат слияния:\n";
    PrintTree(tree);
}


// человек только с нужным ID: дерево сравнивает людей по идентификатору,
// поэтому для поиска и удаления достаточно "пустышки" с тем же ID
Student MakeProbe(const PersonID& id) {
    return Student(id, "-", "", "-", Date(2000, 1, 1), "-", 1);
}

void PrintStudents(const StudentTree& tree) {
    if(tree.IsEmpty()) {
        std::cout << "  (пусто)\n";
        return;
    }
    tree.Traverse(Order::LKP, [](const Student& student) {
        std::cout << "  " << student << ", родился " << student.GetBirthDate().ToString()
                  << ", зачётка " << student.GetRecordBookNumber() << "\n";
    });
}

void DoPeopleDemo() {
    StudentTree students;
    students.Add(Student(PersonID(4501, 123456), "Иван", "Иванович", "Иванов", Date(2005, 3, 14), "ИУ7-31", 1001));
    students.Add(Student(PersonID(4502, 111111), "Мария", "Сергеевна", "Петрова", Date(2004, 11, 2), "ИУ7-32", 1002));
    students.Add(Student(PersonID(4500, 999999), "Олег", "", "Сидоров", Date(2005, 7, 21), "ИУ7-31", 1003));
    students.Add(Student(PersonID(4503, 222222), "Анна", "Игоревна", "Смирнова", Date(2005, 1, 30), "ИУ7-32", 1004));

    std::cout << "Студенты по возрастанию паспортного номера:\n";
    PrintStudents(students);

    std::cout << "\nТолько группа ИУ7-31 (Where):\n";
    PrintStudents(students.Where([](const Student& s) { return s.GetGroup() == "ИУ7-31"; }));

    std::cout << "\nПолные имена (Map в дерево строк, по алфавиту):\n";
    BinarySearchTree<std::string> names = students.Map([](const Student& s) { return s.GetFullName(); });
    names.Traverse(Order::LKP, [](const std::string& name) { std::cout << "  " << name << "\n"; });

    std::cout << "\nПоиск по паспорту 4502 111111: ";
    const Student* found = students.Find(MakeProbe(PersonID(4502, 111111)));
    if(found != nullptr) { std::cout << found->GetFullName() << "\n"; }
    else { std::cout << "не найден\n"; }

    students.Remove(MakeProbe(PersonID(4501, 123456)));
    std::cout << "\nПосле удаления студента 4501 123456:\n";
    PrintStudents(students);

    TeacherTree teachers;
    teachers.Add(Teacher(PersonID(1200, 300001), "Пётр", "Петрович", "Кузнецов", Date(1975, 9, 1), "ИУ7", "доцент"));
    teachers.Add(Teacher(PersonID(1200, 100002), "Елена", "Андреевна", "Орлова", Date(1982, 4, 17), "ИУ6", "профессор"));
    teachers.Add(Teacher(PersonID(1201, 500003), "Сергей", "Олегович", "Волков", Date(1968, 12, 5), "ИУ7", "старший преподаватель"));

    std::cout << "\nПреподаватели по возрастанию паспортного номера:\n";
    teachers.Traverse(Order::LKP, [](const Teacher& teacher) {
        std::cout << "  " << teacher << ", " << teacher.GetPosition() << "\n";
    });
}


int main() {
    IntTree tree;

    while(true) {
        PrintMenu();
        int choice = ReadInt("Выбор: ");

        try {
            if(choice == 0) { break; }
            else if(choice == 1) { DoAdd(tree); }
            else if(choice == 2) { DoRemove(tree); }
            else if(choice == 3) { DoFind(tree); }
            else if(choice == 4) { PrintTree(tree); }
            else if(choice == 5) { DoTraversal(tree); }
            else if(choice == 6) { DoMap(tree); }
            else if(choice == 7) { DoWhere(tree); }
            else if(choice == 8) { DoReduce(tree); }
            else if(choice == 9) { DoBalance(tree); }
            else if(choice == 10) { DoExtract(tree); }
            else if(choice == 11) { DoContains(tree); }
            else if(choice == 12) { DoSave(tree); }
            else if(choice == 13) { DoLoad(tree); }
            else if(choice == 14) { DoFill(tree); }
            else if(choice == 15) { DoMerge(tree); }
            else if(choice == 16) { DoPeopleDemo(); }
            else { std::cout << "Нет такого пункта\n"; }
        }
        catch(const std::exception& error) {
            std::cout << "Ошибка: " << error.what() << "\n";
        }
    }
    return 0;
}
