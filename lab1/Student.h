#ifndef STUDENT_H_
#define STUDENT_H_

#include <string>
#include <vector>

using namespace std;

class Class;

enum Degree {
    undergraduate,
    graduate
};

class Student {
    // TODO: implement class Student.
private:
    string name;
    string year;
    Degree degree;
protected:
    vector<Class*> classes;
public:
    const std::string id;
    Student(string id, string name, string year, Degree degree):
    id(id), name(name), year(year), degree(degree){classes.clear();}
    std::string toString() const;
    virtual double getGpa()=0;
    virtual double getAvgScore()=0;
    void addClass(Class* c);
};

// TODO: implement class Graduate.
class Graduate: public Student{
public:
    Graduate(string id, string name, string year):Student(id, name, year, graduate){}
    double getGpa();
    double getAvgScore();
};

// TODO: implement class Undergraduate.
class Undergraduate: public Student{
public:
    Undergraduate(string id, string name, string year):Student(id, name, year, undergraduate){}
    double getGpa();
    double getAvgScore();
};

class StudentWrapper {
private:
    const Student &student;
    double score = 0.0;
public:
    const std::string id;
    // TODO: fix error
    StudentWrapper(const std::string &id, const Student &student): id(id), student(student) {}

    void setScore(double score)
    {
        if (score < 0 || score > 100)
            throw "Wrong score!";
        this->score = score;
    }

    double getScore() const
    {
        return this->score;
    }

    std::string toString() const
    {
        return student.toString();
    }
};

#endif // STUDENT_H_
