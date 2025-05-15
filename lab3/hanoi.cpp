#include <climits>
#include <iostream>
#include <string>
#include <sstream>

#include "board.h"

using namespace std;

int main() {
    while (true) {
        cout << "How many disks do you want? (1 ~ 5)" << endl;
        string input;
        getline(cin, input);
        if (input == "Q") {
            break;
        }

        // TODO
        if(input=="1"||input=="2"||input=="3"||input=="4"||input=="5"){
            Board board(input[0]-'0');
            board.draw();
            while(true){
                cout<<"Move a disk. Format: x y"<<endl;
                string cmd;
                getline(cin, cmd);
                if(cmd[0] <= '3' && cmd[0] >= '0'
                && cmd[1] == ' ' && cmd[2] >= '0'
                && cmd[2] <= '3' && cmd.size() == 3){
                    stringstream ss(cmd);
                    int a, b;
                    ss >> a >> b;
                    if(a == 0 && b == 0) {
                        board.autoplay();
                        break;
                    }
                    board.move(a, b);
                }
                board.draw();
                if(board.win()){
                    cout<<"Congratulations! You win!"<<endl;
                    break;
                }
            }
        }
    }
    return 0;
}
