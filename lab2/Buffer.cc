#include <iostream>
#include "Buffer.h"
#include <fstream>

using namespace std;
//TODO: your code here
//implement the functions in Buffer

Buffer::Buffer() {
    currentLineNum=0;
    headLine=new Line();
    headLine->prev= nullptr;
    headLine->next= nullptr;
    headLine->line="被lab折磨的又一天";
    currentLine=headLine;
}

Buffer::~Buffer() {
    Line* cur=headLine;
    while(cur->next) {
        cur=cur->next;
        delete cur->prev;
    }
    delete cur;
}

void Buffer::writeToFile(const string &filename) const {
    int bytes=0;
    ofstream out(filename);
    Line* cur=headLine;
    while(cur->next){
        bytes+=cur->next->line.length()+1;
        out<<cur->next->line<<endl;
        cur=cur->next;
    }
    cout<<bytes<<" byte(s) written"<<endl;
    out.close();
}

void Buffer::showLines(int from, int to) {
    Line* cur=headLine;
    for(int i=1;i<from;++i) cur=cur->next;
    for(int i=from;i<=to;++i){
        cur=cur->next;
        cout<<i<<'\t'<<cur->line<<endl;
    }
    currentLine=cur;
}

void Buffer::deleteLines(int from, int to){
    Line* cur=headLine;
    for(int i=1;i<from;++i) cur=cur->next;
    for(int i=from;i<=to;++i) {
        Line* de=cur->next;
        de->prev->next=de->next;
        if(de->next) de->next->prev=de->prev;
        currentLineNum--;
        delete de;
    }
    if(currentLineNum==0) currentLine=headLine;
    else if(cur->next) currentLine=cur->next;
    else currentLine=cur;
}

void Buffer::insertLine(const string &text){
    if(currentLine==headLine){
        appendLine(text);
        return;
    }
    Line *newLine=new Line();
    newLine->prev=currentLine->prev;
    newLine->next=currentLine;
    newLine->line=text;
    newLine->prev->next=newLine;
    currentLine->prev=newLine;
    currentLine=newLine;
    currentLineNum++;
}

void Buffer::appendLine(const string &text){
    Line *newLine=new Line();
    newLine->prev=currentLine;
    newLine->next=currentLine->next;
    newLine->line=text;
    if(currentLine->next!=nullptr) newLine->next->prev=newLine;
    currentLine->next=newLine;
    currentLine=newLine;
    currentLineNum++;
}

const string &Buffer::moveToLine(int idx){
    Line* cur=headLine;
    for(int i=0;i<idx;++i) cur=cur->next;
    currentLine=cur;
    return cur->line;
}

void Buffer::swap(int a,int b){
    if(a==b) return;
    int left=min(a,b);
    int right=max(a,b);
    Line *s1=headLine,*s2=headLine;
    for(int i=0;i<left;++i) s1=s1->next;
    for(int i=0;i<right;++i) s2=s2->next;
    if(s1->next==s2){
        Line *s1p=s1->prev,*s2n=s2->next;
        s1p->next=s2;
        if(s2n) s2n->prev=s1;
        s1->prev=s2;
        s1->next=s2n;
        s2->prev=s1p;
        s2->next=s1;
    }
    else{
        Line *s1p=s1->prev,*s1n=s1->next,*s2p=s2->prev,*s2n=s2->next;
        s1p->next=s2;
        s1n->prev=s2;
        s2p->next=s1;
        if(s2n) s2n->prev=s1;
        s1->prev=s2p;
        s1->next=s2n;
        s2->prev=s1p;
        s2->next=s1n;
    }
    if(currentLine==s1) currentLine=s2;
    else if(currentLine==s2) currentLine=s1;
}

// for test, Don't modify
static void* aPtr = nullptr;
static void* bPtr = nullptr;

void Buffer::printAddr(int idx) const {

    int curLineNo = 1;
    Line *curLine = headLine;
    while (curLineNo < idx) {
        curLineNo += 1;
        curLine = curLine->next;
    }
    
    std::cout << idx << ":" << curLine << std::endl;
}

void Buffer::loadAddr2(int one, int another) const {
    if (one == another)
        return;

    int first = std::min(one, another);
    int second = std::max(one, another);

    Line *firstLine = nullptr, *secondLine = nullptr;

    int curLineNo = 1;
    Line *curLine = headLine;
    while (curLineNo <= second) {
        if (curLineNo == first)
            firstLine = curLine;
        if (curLineNo == second)
            secondLine = curLine;

        curLineNo += 1;
        curLine = curLine->next;
    }

    if (firstLine && secondLine) {
        aPtr = (void *)firstLine;
        bPtr = (void *)secondLine;
    }
}

void Buffer::testSwap(int one, int another) const {

    if (one == another)
        return;

    int first = std::min(one, another);
    int second = std::max(one, another);

    Line *firstLine = nullptr, *secondLine = nullptr;

    int curLineNo = 1;
    Line *curLine = headLine;
    while (curLineNo <= second) {
        if (curLineNo == first)
            firstLine = curLine;
        if (curLineNo == second)
            secondLine = curLine;

        curLineNo += 1;
        curLine = curLine->next;
    }

    if (firstLine && secondLine) {
        if (aPtr == secondLine && bPtr == firstLine)
            std::cout << "Swap 2 Nodes successfully!" << std::endl;
        else
            std::cout << "Swap 2 Nodes failed!" << std::endl;
        aPtr = nullptr;
        bPtr = nullptr;
    }
}