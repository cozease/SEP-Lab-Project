#include "Student.h"
#include <string>
#include <sstream>
#include "Class.h"

std::string Student::toString() const
{
    // TODO: uncomment the following code after implementing class Student.

    std::ostringstream oss;
    oss << "Student Information:"
        << "\n\tid: " << id
        << "\n\tname: " << name
        << "\n\tenrollment year: " << year
        << "\n\tdegree: " << (degree == graduate ? "graduate" : "undergraduate")
        << std::endl;
    return oss.str();

    return "";
}

// TODO: implement functions which are declared in Student.h.
void Student::addClass(Class *c) {classes.push_back(c);}
double Graduate::getGpa() {
    int allpoint=0;
    double gpa=0;
    for(vector<Class*>::iterator it=classes.begin();it!=classes.end();++it){
        allpoint+=(*it)->point;
    }
    for(vector<Class*>::iterator it=classes.begin();it!=classes.end();++it){
        double s=(*it)->getStudentWrapper(id).getScore();
        if(s>=90 & s<=100) gpa+=((double)((*it)->point))/((double)allpoint)*4.0;
        else if(s>=80 & s<=89) gpa+=((double)((*it)->point))/((double)allpoint)*3.5;
        else if(s>=70 & s<=79) gpa+=((double)((*it)->point))/((double)allpoint)*3.0;
        else if(s>=60 & s<=69) gpa+=((double)((*it)->point))/((double)allpoint)*2.5;
        else gpa+=((double)((*it)->point))/((double)allpoint)*2.0;
    }
    return gpa;
}
double Graduate::getAvgScore() {
    int allpoint=0;
    double avg=0;
    for(vector<Class*>::iterator it=classes.begin();it!=classes.end();++it){
        allpoint+=(*it)->point;
    }
    for(vector<Class*>::iterator it=classes.begin();it!=classes.end();++it){
        avg+=((*it)->getStudentWrapper(id).getScore())*((double)((*it)->point))/((double)allpoint);
    }
    return avg;
}
double Undergraduate::getGpa() {
    int allpoint=0;
    double gpa=0;
    for(vector<Class*>::iterator it=classes.begin();it!=classes.end();++it){
        allpoint+=(*it)->point;
    }
    for(vector<Class*>::iterator it=classes.begin();it!=classes.end();++it){
        gpa+=((*it)->getStudentWrapper(id).getScore())/20.0*((double)((*it)->point))/((double)allpoint);
    }
    return gpa;
}
double Undergraduate::getAvgScore() {
    int allpoint=0;
    double avg=0;
    for(vector<Class*>::iterator it=classes.begin();it!=classes.end();++it){
        allpoint+=(*it)->point;
    }
    for(vector<Class*>::iterator it=classes.begin();it!=classes.end();++it){
        avg+=((*it)->getStudentWrapper(id).getScore())*((double)((*it)->point))/((double)allpoint);
    }
    return avg;
}
