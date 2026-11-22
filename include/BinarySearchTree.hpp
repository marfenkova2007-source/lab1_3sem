#pragma once
#include <cstddef>
#include <functional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

enum class Order { KLP, KPL, LPK, LKP, PLK, PKL };

//  K - корень, L - левое, P - правое
inline const char* OrderToString(Order order) {
    switch(order) {
        case Order::KLP: return "KLP";
        case Order::KPL: return "KPL";
        case Order::LPK: return "LPK";
        case Order::LKP: return "LKP";
        case Order::PLK: return "PLK";
        case Order::PKL: return "PKL";
    }
    throw std::invalid_argument("Unknown traversal order");
}

template <typename T>
class Node {
public:
    T value;
    Node<T>* left;
    Node<T>* right;

    explicit Node(const T& val) : value(val), left(nullptr), right(nullptr) {}
};

template <typename T, typename Compare = std::less<T>>
class BinarySearchTree {
private:
    Node<T>* root;
    size_t count;
    Compare comp;

    struct Format {
        char part[3];
        char open[3];
        char close[3];
    };

    bool Equal(const T& a, const T& b) const {
        return !comp(a, b) && !comp(b, a);
    }

    static void ClearRec(Node<T>* node) {
        if(node == nullptr) { return; }
        ClearRec(node->left);
        ClearRec(node->right);
        delete node;
    }

    static Node<T>* CloneRec(const Node<T>* node) {
        if(node == nullptr) { return nullptr; }
        Node<T>* copy = new Node<T>(node->value);
        copy->left = CloneRec(node->left);
        copy->right = CloneRec(node->right);
        return copy;
    }

    static size_t CountRec(const Node<T>* node) {
        if(node == nullptr) { return 0; }
        return CountRec(node->left) + CountRec(node->right) + 1;
    }

    static size_t HeightRec(const Node<T>* node) {
        if(node == nullptr) { return 0; }
        size_t leftHeight = HeightRec(node->left);
        size_t rightHeight = HeightRec(node->right);
        return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
    }

    Node<T>** FindLink(const T& value) {
        Node<T>** link = &root;
        while(*link != nullptr) {
            if(comp(value, (*link)->value)) { link = &(*link)->left; }
            else if(comp((*link)->value, value)) { link = &(*link)->right; }
            else { break; }
        }
        return link;
    }

    const Node<T>* FindNode(const T& value) const {
        const Node<T>* node = root;
        while(node != nullptr) {
            if(comp(value, node->value)) { node = node->left; }
            else if(comp(node->value, value)) { node = node->right; }
            else { return node; }
        }
        return nullptr;
    }

    // обходы
    static void TraverseRec(const Node<T>* node, const char* order, const std::function<void(const T&)>& visit) {
        if(node == nullptr) { return; }
        for(int i = 0; i < 3; i++) {
            if(order[i] == 'K') { visit(node->value); }
            else if(order[i] == 'L') { TraverseRec(node->left, order, visit); }
            else { TraverseRec(node->right, order, visit); }
        }
    }

    // балансировка
    void CollectNodes(std::vector<Node<T>*>& nodes) const {
        std::vector<Node<T>*> stack;
        Node<T>* node = root;
        while(node != nullptr || !stack.empty()) {
            while(node != nullptr) {
                stack.push_back(node);
                node = node->left;
            }
            node = stack.back();
            stack.pop_back();
            nodes.push_back(node);
            node = node->right;
        }
    }

    // строит дерево из nodes[from, to)
    static Node<T>* BuildBalanced(std::vector<Node<T>*>& nodes, size_t from, size_t to) {
        if(from >= to) { return nullptr; }
        size_t mid = from + (to - from) / 2;
        Node<T>* node = nodes[mid];
        node->left = BuildBalanced(nodes, from, mid);
        node->right = BuildBalanced(nodes, mid + 1, to);
        return node;
    }

    // строит сбалансированное дерево из значений values[from, to), уже упорядоченных по возрастанию
    static Node<T>* BuildFromSorted(const std::vector<const T*>& values, size_t from, size_t to) {
        if(from >= to) { return nullptr; }
        size_t mid = from + (to - from) / 2;
        Node<T>* node = new Node<T>(*values[mid]);
        node->left = BuildFromSorted(values, from, mid);
        node->right = BuildFromSorted(values, mid + 1, to);
        return node;
    }

    // поиск на вхождение поддерева
    bool SameShapeRec(const Node<T>* a, const Node<T>* b) const {
        if(a == nullptr && b == nullptr) { return true; }
        if(a == nullptr || b == nullptr) { return false; }
        if(!Equal(a->value, b->value)) { return false; }
        return SameShapeRec(a->left, b->left) && SameShapeRec(a->right, b->right);
    }

    bool IsSearchTreeRec(const Node<T>* node, const T* low, const T* high) const {
        if(node == nullptr) { return true; }
        if(low != nullptr && !comp(*low, node->value)) { return false; }
        if(high != nullptr && !comp(node->value, *high)) { return false; }
        return IsSearchTreeRec(node->left, low, &node->value)
            && IsSearchTreeRec(node->right, &node->value, high);
    }

    // сохранение в строку по формату
    static bool IsOpenBracket(char c) { return c == '{' || c == '(' || c == '['; }

    static char PartLetter(const std::string& token) {
        if(token == "K" || token == "К") { return 'K'; }
        if(token == "L" || token == "Л") { return 'L'; }
        if(token == "P" || token == "П") { return 'P'; }
        return 0;
    }

    static Format ParseFormat(const std::string& format) {
        Format result;
        size_t pos = 0;
        for(int i = 0; i < 3; i++) {
            if(pos >= format.size() || !IsOpenBracket(format[pos])) {
                throw std::invalid_argument("Bad format: opening bracket is expected");
            }
            size_t end = format.find_first_of("})]", pos + 1);
            if(end == std::string::npos) {
                throw std::invalid_argument("Bad format: closing bracket is missing");
            }
            char letter = PartLetter(format.substr(pos + 1, end - pos - 1));
            if(letter == 0) { throw std::invalid_argument("Bad format: expected K, L or P in brackets"); }
            for(int j = 0; j < i; j++) {
                if(result.part[j] == letter) { throw std::invalid_argument("Bad format: repeated part"); }
            }
            result.part[i] = letter;
            result.open[i] = format[pos];
            result.close[i] = format[end];
            pos = end + 1;
        }
        if(pos != format.size()) { throw std::invalid_argument("Bad format: extra characters"); }
        return result;
    }

    void ToStringRec(const Node<T>* node, const Format& format, std::ostringstream& out) const {
        if(node == nullptr) { return; }
        for(int i = 0; i < 3; i++) {
            out << format.open[i];
            if(format.part[i] == 'K') { out << node->value; }
            else if(format.part[i] == 'L') { ToStringRec(node->left, format, out); }
            else { ToStringRec(node->right, format, out); }
            out << format.close[i];
        }
    }

    // чтение из строки по формату
    static void ExpectChar(const std::string& text, size_t& pos, char c) {
        if(pos >= text.size() || text[pos] != c) {
            throw std::invalid_argument(std::string("Expected '") + c + "' in the string");
        }
        pos++;
    }

    static Node<T>* ParseRec(const std::string& text, size_t& pos, const Format& format) {
        T value = T();
        Node<T>* left = nullptr;
        Node<T>* right = nullptr;
        try {
            for(int i = 0; i < 3; i++) {
                ExpectChar(text, pos, format.open[i]);
                if(format.part[i] == 'K') {
                    size_t end = text.find(format.close[i], pos);
                    if(end == std::string::npos) { throw std::invalid_argument("Closing bracket is missing"); }
                    std::istringstream in(text.substr(pos, end - pos));
                    if(!(in >> value)) { throw std::invalid_argument("Cannot parse the value"); }
                    in >> std::ws;
                    if(!in.eof()) { throw std::invalid_argument("Cannot parse the value"); }
                    pos = end;
                }
                else if(pos < text.size() && text[pos] != format.close[i]) {
                    Node<T>*& child = (format.part[i] == 'L') ? left : right;
                    child = ParseRec(text, pos, format);
                }
                ExpectChar(text, pos, format.close[i]);
            }
        }
        catch(...) {
            ClearRec(left);
            ClearRec(right);
            throw;
        }
        Node<T>* node = new Node<T>(value);
        node->left = left;
        node->right = right;
        return node;
    }

    void Swap(BinarySearchTree& other) noexcept {
        using std::swap;
        swap(root, other.root);
        swap(count, other.count);
        swap(comp, other.comp);
    }

public:
    explicit BinarySearchTree(Compare compare = Compare()) : root(nullptr), count(0), comp(compare) {}
    ~BinarySearchTree() { ClearRec(root); }

    BinarySearchTree(const BinarySearchTree& other)
        : root(CloneRec(other.root)), count(other.count), comp(other.comp) {}

    BinarySearchTree(BinarySearchTree&& other) noexcept
        : root(other.root), count(other.count), comp(std::move(other.comp)) {
        other.root = nullptr;
        other.count = 0;
    }

    // и копирующее, и перемещающее присваивание (copy-and-swap)
    BinarySearchTree& operator=(BinarySearchTree other) noexcept {
        Swap(other);
        return *this;
    }

    // базовые операции
    // возвращает false, если такое значение уже есть
    bool Add(const T& value) {
        Node<T>** link = FindLink(value);
        if(*link != nullptr) { return false; }
        *link = new Node<T>(value);
        count++;
        return true;
    }

    void Remove(const T& value) {
        Node<T>** link = FindLink(value);
        Node<T>* node = *link;
        if(node == nullptr) { throw std::out_of_range("Value is not found in the tree"); }

        if(node->left == nullptr) { *link = node->right; }
        else if(node->right == nullptr) { *link = node->left; }
        else {
            // на место узла ставим минимум правого поддерева, значения не копируем
            Node<T>** minLink = &node->right;
            while((*minLink)->left != nullptr) { minLink = &(*minLink)->left; }
            Node<T>* minNode = *minLink;
            *minLink = minNode->right;
            minNode->left = node->left;
            minNode->right = node->right;
            *link = minNode;
        }
        delete node;
        count--;
    }

    bool Contains(const T& value) const {
        return FindNode(value) != nullptr;
    }

    const T* Find(const T& value) const {
        const Node<T>* node = FindNode(value);
        if(node == nullptr) { return nullptr; }
        return &node->value;
    }

    size_t Size() const { return count; }
    bool IsEmpty() const { return count == 0; }
    size_t Height() const { return HeightRec(root); }

    const T& Min() const {
        if(root == nullptr) { throw std::out_of_range("The tree is empty"); }
        const Node<T>* node = root;
        while(node->left != nullptr) { node = node->left; }
        return node->value;
    }

    const T& Max() const {
        if(root == nullptr) { throw std::out_of_range("The tree is empty"); }
        const Node<T>* node = root;
        while(node->right != nullptr) { node = node->right; }
        return node->value;
    }

    void Clear() {
        ClearRec(root);
        root = nullptr;
        count = 0;
    }

    void Traverse(Order order, const std::function<void(const T&)>& visit) const {
        TraverseRec(root, OrderToString(order), visit);
    }

    // балансировка: перестраиваем те же узлы в дерево минимальной высоты
    void Balance() {
        std::vector<Node<T>*> nodes;
        nodes.reserve(count);
        CollectNodes(nodes);
        root = BuildBalanced(nodes, 0, nodes.size());
    }

    // map-where-reduce
    // при отображении разные значения могут совпасть, тогда в результате останется одно
    template <typename F>
    auto Map(F func) const -> BinarySearchTree<std::decay_t<decltype(func(std::declval<const T&>()))>> {
        using U = std::decay_t<decltype(func(std::declval<const T&>()))>;
        BinarySearchTree<U> result;
        Traverse(Order::KLP, [&](const T& x) { result.Add(func(x)); });
        return result;
    }

    // подходящие значения берём по возрастанию и строим из них сбалансированное дерево:
    // вставка в обходе КЛП после выбрасывания узлов даёт сильно перекошенное дерево
    template <typename F>
    BinarySearchTree Where(F predicat) const {
        std::vector<const T*> kept;
        Traverse(Order::LKP, [&](const T& x) {
            if(predicat(x)) { kept.push_back(&x); }
        });
        BinarySearchTree result(comp);
        result.root = BuildFromSorted(kept, 0, kept.size());
        result.count = kept.size();
        return result;
    }

    // func(элемент, накопитель), по умолчанию обход ЛКП (по возрастанию)
    template <typename U, typename F>
    U Reduce(F func, U startValue, Order order = Order::LKP) const {
        U acc = startValue;
        Traverse(order, [&](const T& x) { acc = func(x, acc); });
        return acc;
    }

    // извлечение поддерева (по заданному корню)
    BinarySearchTree ExtractSubtree(const T& rootValue) const {
        const Node<T>* node = FindNode(rootValue);
        if(node == nullptr) { throw std::invalid_argument("Node is not found"); }
        BinarySearchTree result(comp);
        result.root = CloneRec(node);
        result.count = CountRec(node);
        return result;
    }

    // поиск на вхождение поддерева
    bool ContainsSubtree(const BinarySearchTree& subTree) const {
        if(subTree.root == nullptr) { return true; }
        const Node<T>* node = FindNode(subTree.root->value);
        return node != nullptr && SameShapeRec(node, subTree.root);
    }

    // сохранение в строку
    // значения через пробел, обход КЛП восстанавливает то же самое дерево
    std::string ToString(Order order = Order::KLP) const {
        std::ostringstream out;
        bool first = true;
        Traverse(order, [&](const T& x) {
            if(!first) { out << ' '; }
            out << x;
            first = false;
        });
        return out.str();
    }

    // по формату, например "{K}(L)[P]": скобки задают порядок обхода
    std::string ToString(const std::string& format) const {
        Format parsed = ParseFormat(format);
        std::ostringstream out;
        ToStringRec(root, parsed, out);
        return out.str();
    }

    // чтение из строки
    static BinarySearchTree FromString(const std::string& text, Compare compare = Compare()) {
        BinarySearchTree tree(compare);
        std::istringstream in(text);
        T value = T();
        while(in >> value) { tree.Add(value); }
        if(!in.eof()) { throw std::invalid_argument("Cannot parse a value in the string"); }
        return tree;
    }

    static BinarySearchTree FromString(const std::string& text, const std::string& format, Compare compare = Compare()) {
        Format parsed = ParseFormat(format);
        BinarySearchTree tree(compare);
        if(text.empty()) { return tree; }

        size_t pos = 0;
        tree.root = ParseRec(text, pos, parsed);
        tree.count = CountRec(tree.root);
        if(pos != text.size()) { throw std::invalid_argument("Extra characters after the tree"); }
        if(!tree.IsSearchTreeRec(tree.root, nullptr, nullptr)) {
            throw std::invalid_argument("The string does not describe a search tree");
        }
        return tree;
    }

    // слияние: в результат входят значения обоих деревьев, повторы хранятся один раз
    BinarySearchTree Merge(const BinarySearchTree& other) const {
        BinarySearchTree result(*this);
        other.Traverse(Order::KLP, [&](const T& x) { result.Add(x); });
        return result;
    }

    // перегрузка операторов
    BinarySearchTree operator+(const BinarySearchTree& other) const {
        return Merge(other);
    }

    // деревья равны
    bool operator==(const BinarySearchTree& other) const {
        return count == other.count && SameShapeRec(root, other.root);
    }

    bool operator!=(const BinarySearchTree& other) const {
        return !(*this == other);
    }

    // вывод в поток: значения в обходе КЛП
    friend std::ostream& operator<<(std::ostream& out, const BinarySearchTree& tree) {
        return out << tree.ToString();
    }

    const Node<T>* GetRoot() const { return root; }
};
