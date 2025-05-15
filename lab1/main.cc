#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "Class.h"
#include "Student.h"

using namespace std;

class AppX {
private:
    vector<Student *> studentVec;
    vector<Class *> classVec;

    void loadFiles();
    void inputScore();
    void printHighestScore();
    void printGrade();

public:
    AppX();
    ~AppX();
    int run();
};

AppX::~AppX()
{
    // You can use the traditional loop, which is more clear.
    for (vector<Class *>::iterator it = classVec.begin();
         it != classVec.end();
         ++it) {
        if (*it) delete (*it);
    }
    // You can use modern and simpler loops only if you know what it is doing.
    for (const auto &s: studentVec) {
        if (s) delete (s);
    }
}

AppX::AppX()
{
    loadFiles();
}

void AppX::loadFiles()
{
    string line;
    size_t pos1, pos2;
    vector<string> bufv;
    Student *st = nullptr;
    string clsname;
    int point;
    Class *cl = nullptr;

    // Open a file as a input stream.
    ifstream stfile("./Students.txt");

    while (getline(stfile, line)) {
        if (line.empty()) // It's an empty line.
            continue;
        if (line[0] == '#') // It's a comment line.
            continue;

        // The bufv vector stores each column in the line.
        bufv.clear();
        // Split the line into columns.
        //   pos1: beginning of the column.
        //   pos2: end of the column.
        pos1 = 0;
        while (true) {
            pos2 = line.find(';', pos1 + 1);
            if (pos2 == string::npos) { // No more columns.
                // Save the last column (pos1 ~ eol).
                bufv.push_back(line.substr(pos1, string::npos));
                break;
            } else {
                // Save the column (pos1 ~ pos2).
                bufv.push_back(line.substr(pos1, pos2 - pos1));
            }
            pos1 = pos2 + 1;
        }

        // TODO: uncomment next lines after implementing class Undergraduate and Graduate.

        if (bufv[3] == "U")
            st = new Undergraduate(bufv[0], bufv[1], bufv[2]);
        else
            st = new Graduate(bufv[0], bufv[1], bufv[2]);

        studentVec.push_back(st);
    }
    stfile.close();

    // TODO: load data from ./Classes.txt and push objects to the vector.
    ifstream clsfile("./Classes.txt");
    while(getline(clsfile,line)){
        if(line[0]=='#') continue;
        if(line[5]==':') {
            clsname = line.substr(6);
            continue;
        }
        if(line[6]==':'){
            point=(int)(line[7]-'0');
            cl=new Class(clsname,point);
            continue;
        }
        if(line.empty()){
            classVec.push_back(cl);
            cl= nullptr;
            continue;
        }
        for(vector<Student *>::iterator it=studentVec.begin();it!=studentVec.end();++it){
            if(line==(*it)->id){
                (*it)->addClass(cl);
                cl->addStudent(**it);
                break;
            }
        }
    }
    classVec.push_back(cl);
    clsfile.close();
}

void AppX::inputScore()
{
    // TODO: implement inputScore.
    string cls;
    while(true){
        cout<<"Please input the class name (or input q to quit): ";
        cin>>cls;
        if(cls=="q") break;
        while(true){
            Class* c=nullptr;
            for(vector<Class*>::iterator it=classVec.begin();it!=classVec.end();++it){
                if((*it)->name==cls){
                    c=*it;
                    break;
                }
            }
            if(!c){
                cout<<"No match class!"<<endl;
                break;
            }
            cout<<"Please input the student id and score (or input q to quit): ";
            string line;
            bool flag=false;
            while(cin>>line){
                if(line=="q"){
                    flag=true;
                    break;
                }
                string id;
                double score;
                id=line.substr(0,10);
                score=stod(line.substr(11,string::npos));
                try{
                    StudentWrapper &st=c->getStudentWrapper(id);
                    st.setScore(score);
                }
                catch(const char* e){
                    cerr<<e<<endl;
                }
            }
            if(flag) break;
        }
    }
    // Hint: Take a look at printHighestScore().
}

void AppX::printHighestScore()
{
    string sbuf;
    Class *cl;
    double highest;

    while (true) {
        cout << "Please input the class name (or input q to quit): ";
        cin >> sbuf;
        if (sbuf == "q")
            break;

        cl = nullptr;
        for (vector<Class*>::iterator it = classVec.begin();
                it != classVec.end();
                ++ it) {
            if ((*it)->name == sbuf) {
                cl = *it;
                break;
            }
        }
        if (cl == nullptr) {
            cout << "No match class!" << endl;
            continue;
        }

        highest = cl->getHighestScore();
        cout << setiosflags(ios::fixed) << setprecision(2)<< "The highest score is: " << highest << endl;
    }
}

void AppX::printGrade()
{
    // TODO: implement printGrade.
    string id;
    cout<<"Please input the student id (or input q to quit): "<<endl;
    while(cin>>id){
        if(id=="q") break;
        bool flag=false;
        for(vector<Student*>::iterator it=studentVec.begin();it!=studentVec.end();++it){
            if((*it)->id==id){
                flag=true;
                cout<<"GPA,AVG = "<<setiosflags(ios::fixed) << setprecision(2)<<(*it)->getGpa()<<','<<(*it)->getAvgScore()<<endl;
            }
        }
        if(!flag) cout<<"No match student!"<<endl;
    }
    // Hint: Take a look at printHighestScore().
}

int AppX::run()
{
    char cmd;
    while (true) {
        cout << "Command menu:\n"
            << "\ti: Input score\n"
            << "\ta: Compute highest score of a class\n"
            << "\tg: Compute grade of a student\n"
            << "\tq: Quit\n"
            << "Please input the command: ";
        cin >> cmd;
        if (cmd == 'i') {
            inputScore();
        } else if (cmd == 'a') {
            printHighestScore();
        } else if (cmd == 'g') {
            printGrade();
        } else if (cmd == 'q') {
            break;
        } else {
            cout << "Invalid command!\n" << endl;
        }
    }
    return 0;
}

int main()
{
    AppX app;
    return app.run();
}
