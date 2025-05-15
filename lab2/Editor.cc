#include <iostream>
#include <sstream>
#include "Editor.h"

using namespace std;

Editor::Editor()
{
    buffer = new Buffer();
}
Editor::~Editor()
{
    // TODO: Implement destructor
    delete buffer;
}

void Editor::run()
{
    string cmd;
    while (true)
    {
        cout << "cmd> ";
        cout.flush();
        getline(cin, cmd);
        if (cmd == "Q")
            break;
        try {
            dispatchCmd(cmd);
        } catch (const char *e) {
            cout << "? " << e << endl;
        } catch (const out_of_range &oor) {
            cout << "? " << oor.what() << endl;
        } catch (const range_error &re) {
            cout << "? " << re.what() << endl;
        }
    }
}
void Editor::cmdAppend()
{
    cout << "It's input mode now. Quit with a line with a single dot(.)" << endl;
    // TODO: finish cmdAppend.
    string text;
    while(true){
        getline(cin,text);
        if(text==".") break;
        else buffer->appendLine(text);
    }
}

void Editor::cmdInsert()
{
    cout << "It's input mode now. Quit with a line with a single dot(.)" << endl;
    bool firstLine = true;
    while (true)
    {
        string text;
        getline(cin, text);
        if (text == ".")
            break;
        if (firstLine) {
            buffer->insertLine(text);
            firstLine = false;
        }  else {
            buffer->appendLine(text);
        }
    }
}

void Editor::cmdDelete(int start, int end)
{
    buffer->deleteLines(start, end);
}

void Editor::cmdNull(int line)
{
    cout << buffer->moveToLine(line) << endl;
}

void Editor::cmdNumber(int start, int end)
{
    buffer->showLines(start, end);
}

void Editor::cmdWrite(const string &filename)
{
    buffer->writeToFile(filename);
}

void Editor::dispatchCmd(const string &cmd)
{
    if (cmd == "a") {
        cmdAppend();
        return;
    }
    if (cmd == "i") {
        cmdInsert();
        return;
    }
    if (cmd[0] == 'w' && cmd[1] == ' ') {
        // TODO: call cmdWrite with proper arguments
        if(cmd[2]=='\0') throw "Filename not specified";
        string filename=cmd.substr(2);
        cmdWrite(filename);
        return;
    }
    // TODO: handle special case "1,$n".
    if(cmd=="1,$n"){
        if(buffer->currentLineNum==0) throw "Number range error";
        buffer->showLines(1,buffer->currentLineNum);
        return;
    }
    int start, end;
    char comma, type = ' ';
    stringstream ss(cmd);
    ss >> start;
    if (ss.eof()) {
        if(start>buffer->currentLineNum) throw "Line number out of range";
        cmdNull(start);
        return;
    }
    ss >> comma >> end >> type;
    if (ss.good()) {
        //char flag;
        //if(ss>>flag) throw "Bad/Unknown command";
        if (type == 'n') {
            if(end<start) throw "Number range error";
            if(end>buffer->currentLineNum||start<1) throw "Line number out of range";
            cmdNumber(start, end);
            return;
        } else if (type == 'd') {
            if(end<start) throw "Delete range error";
            if(end>buffer->currentLineNum||start<1) throw "Line number out of range";
            cmdDelete(start, end);
            return;
        }  else if (type == 'l') {
            // for test, please use ?,?l -> ?,?s -> ?,?t
            buffer->loadAddr2(start+1, end+1);
            // buffer->printAddr(start);
            // buffer->printAddr(end);
            return;
        } else if (type == 's') {
            //if(end<start) throw "Number range error";
            if(end>buffer->currentLineNum||start<1) throw "Line number out of range";
            buffer->swap(start,end);
            return;
        } else if (type == 't') {
            buffer->testSwap(start+1, end+1);
            // buffer->printAddr(start);
            // buffer->printAddr(end);
            return;
        }
    }
    throw "Bad/Unknown command";
}
