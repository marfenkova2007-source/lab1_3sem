#pragma once
#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <string>

// число с ведущими нулями до заданной ширины
inline std::string PadNumber(int value, size_t width) {
    std::string text = std::to_string(value);
    if(text.size() < width) { text.insert(0, width - text.size(), '0'); }
    return text;
}

inline bool IsLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

inline bool IsValidDate(int year, int month, int day) {
    if(year < 1 || month < 1 || month > 12 || day < 1) { return false; }
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int limit = daysInMonth[month - 1];
    if(month == 2 && IsLeapYear(year)) { limit = 29; }
    return day <= limit;
}


class Date {
private:
    int year;
    int month;
    int day;

public:
    Date(int y, int m, int d) : year(y), month(m), day(d) {
        if(!IsValidDate(y, m, d)) { throw std::invalid_argument("Incorrect date"); }
    }

    int GetYear() const { return year; }
    int GetMonth() const { return month; }
    int GetDay() const { return day; }

    // ДД.ММ.ГГГГ
    std::string ToString() const {
        return PadNumber(day, 2) + "." + PadNumber(month, 2) + "." + PadNumber(year, 4);
    }
};


// идентификатор персоны: серия и номер паспорта, например 4501 123456
class PersonID {
private:
    int series;
    int number;

public:
    PersonID(int s, int n) : series(s), number(n) {
        if(s < 0 || s > 9999) { throw std::invalid_argument("Passport series must be from 0 to 9999"); }
        if(n < 0 || n > 999999) { throw std::invalid_argument("Passport number must be from 0 to 999999"); }
    }

    int GetSeries() const { return series; }
    int GetNumber() const { return number; }

    bool operator<(const PersonID& other) const {
        if(series != other.series) { return series < other.series; }
        return number < other.number;
    }
    bool operator==(const PersonID& other) const {
        return series == other.series && number == other.number;
    }
    bool operator!=(const PersonID& other) const { return !(*this == other); }
};

inline std::ostream& operator<<(std::ostream& out, const PersonID& id) {
    return out << PadNumber(id.GetSeries(), 4) << ' ' << PadNumber(id.GetNumber(), 6);
}


class Person {
private:
    PersonID id;
    std::string firstName;
    std::string middleName;
    std::string lastName;
    Date birthDate;

public:
    // отчество может быть пустым, имя и фамилия нет
    Person(const PersonID& personId, const std::string& first, const std::string& middle,
           const std::string& last, const Date& birth)
        : id(personId), firstName(first), middleName(middle), lastName(last), birthDate(birth) {
        if(first.empty()) { throw std::invalid_argument("First name is empty"); }
        if(last.empty()) { throw std::invalid_argument("Last name is empty"); }
    }

    const PersonID& GetID() const { return id; }
    const std::string& GetFirstName() const { return firstName; }
    const std::string& GetMiddleName() const { return middleName; }
    const std::string& GetLastName() const { return lastName; }
    const Date& GetBirthDate() const { return birthDate; }

    // вычислимый атрибут: "Иван Иванович Иванов"
    std::string GetFullName() const {
        std::string name = firstName;
        if(!middleName.empty()) { name += " " + middleName; }
        return name + " " + lastName;
    }
};


class Student : public Person {
private:
    std::string group;
    int recordBookNumber;

public:
    Student(const PersonID& personId, const std::string& first, const std::string& middle,
            const std::string& last, const Date& birth, const std::string& groupName, int recordBook)
        : Person(personId, first, middle, last, birth), group(groupName), recordBookNumber(recordBook) {
        if(groupName.empty()) { throw std::invalid_argument("Group is empty"); }
        if(recordBook <= 0) { throw std::invalid_argument("Record book number must be positive"); }
    }

    const std::string& GetGroup() const { return group; }
    int GetRecordBookNumber() const { return recordBookNumber; }
};


class Teacher : public Person {
private:
    std::string department;
    std::string position;

public:
    Teacher(const PersonID& personId, const std::string& first, const std::string& middle,
            const std::string& last, const Date& birth, const std::string& departmentName,
            const std::string& positionName)
        : Person(personId, first, middle, last, birth), department(departmentName), position(positionName) {
        if(departmentName.empty()) { throw std::invalid_argument("Department is empty"); }
        if(positionName.empty()) { throw std::invalid_argument("Position is empty"); }
    }

    const std::string& GetDepartment() const { return department; }
    const std::string& GetPosition() const { return position; }
};


// порядок для дерева: по идентификатору, два человека с одним ID считаются одним и тем же
struct PersonIdLess {
    bool operator()(const Person& a, const Person& b) const {
        return a.GetID() < b.GetID();
    }
};

inline std::ostream& operator<<(std::ostream& out, const Person& person) {
    return out << person.GetFullName() << " (" << person.GetID() << ")";
}

inline std::ostream& operator<<(std::ostream& out, const Student& student) {
    return out << static_cast<const Person&>(student) << ", группа " << student.GetGroup();
}

inline std::ostream& operator<<(std::ostream& out, const Teacher& teacher) {
    return out << static_cast<const Person&>(teacher) << ", кафедра " << teacher.GetDepartment();
}
