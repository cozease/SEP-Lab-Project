#include "lexicon.h"

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int n;
pair<char, bool> **board;  // pair的第二个变量用来保存该格子是否已被使用
int score1 = 0, score2 = 0;
vector<string> found;  // 用于保存已被找到过的单词
Lexicon lex("EnglishWords.txt");

// 寻找某个符合的字母周围是否有符合的下一个字符
bool isValidChar(string str, int row, int col) {
    if (str[0] == '\0') return true;
    board[row][col].second = true;
    bool flag = false;

    if (row - 1 >= 0 && !board[row - 1][col].second && board[row - 1][col].first == str[0])
        if (isValidChar(str.substr(1), row - 1, col))
            flag = true;

    if (!flag && row - 1 >= 0 && col + 1 < n && !board[row - 1][col + 1].second && board[row - 1][col + 1].first == str[0])
        if (isValidChar(str.substr(1), row - 1, col + 1))
            flag = true;

    if (!flag && col + 1 < n && !board[row][col + 1].second && board[row][col + 1].first == str[0])
        if (isValidChar(str.substr(1), row, col + 1))
            flag = true;

    if (!flag && row + 1 < n && col + 1 < n && !board[row + 1][col + 1].second && board[row + 1][col + 1].first == str[0])
        if (isValidChar(str.substr(1), row + 1, col + 1))
            flag = true;

    if (!flag && row + 1 < n && !board[row + 1][col].second && board[row + 1][col].first == str[0])
        if (isValidChar(str.substr(1), row + 1, col))
            flag = true;

    if (!flag && row + 1 < n && col - 1 >= 0 && !board[row + 1][col - 1].second && board[row + 1][col - 1].first == str[0])
        if (isValidChar(str.substr(1), row + 1, col - 1))
            flag = true;

    if (!flag && col - 1 >= 0 && !board[row][col - 1].second && board[row][col - 1].first == str[0])
        if (isValidChar(str.substr(1), row, col - 1))
            flag = true;

    if (!flag && row - 1 >= 0 && col - 1 >= 0 && !board[row - 1][col - 1].second && board[row - 1][col - 1].first == str[0])
        if (isValidChar(str.substr(1), row - 1, col - 1))
            flag = true;

    board[row][col].second = false;
    return flag;
}

// 寻找棋盘上是否有组成该单词的路径
bool isValidPath(string str) {
    for (int i = 0; i < str.length(); ++i) str[i] = char(toupper(str[i]));
    int row = 0, col = 0;
    bool flag = false;
    while (!flag && row < n) {
        if (str[0] == board[row][col].first)
            if (isValidChar(str.substr(1), row, col))
                flag = true;
        if (++col == n) {
            ++row;
            col = 0;
        }
    }
    return flag;
}

void allWords(string str, int row, int col) {
    if (str.length() > 3 && lex.contains(str) && find(found.begin(), found.end(), str) == found.end())
        found.push_back(str);
    if (!lex.containsPrefix(str)) return;

    board[row][col].second = true;

    if (row - 1 >= 0 && !board[row - 1][col].second)
        allWords(str + board[row - 1][col].first, row - 1, col);

    if (row - 1 >= 0 && col + 1 < n && !board[row - 1][col + 1].second)
        allWords(str + board[row - 1][col + 1].first, row - 1, col + 1);

    if (col + 1 < n && !board[row][col + 1].second)
        allWords(str + board[row][col + 1].first, row, col + 1);

    if (row + 1 < n && col + 1 < n && !board[row + 1][col + 1].second)
        allWords(str + board[row + 1][col + 1].first, row + 1, col + 1);

    if (row + 1 < n && !board[row + 1][col].second)
        allWords(str + board[row + 1][col].first, row + 1, col);

    if (row + 1 < n && col - 1 >= 0 && !board[row + 1][col - 1].second)
        allWords(str + board[row + 1][col - 1].first, row + 1, col - 1);

    if (col - 1 >= 0 && !board[row][col - 1].second)
        allWords(str + board[row][col - 1].first, row, col - 1);

    if (row - 1 >= 0 && col - 1 >= 0 && !board[row - 1][col - 1].second)
        allWords(str + board[row - 1][col - 1].first, row - 1, col - 1);

    board[row][col].second = false;
}

int main() {
    // TODO
    cin >> n;
    board = new pair<char, bool>*[n];
    for (int i = 0; i < n; ++i) {
        board[i] = new pair<char, bool>[n];
        for (int j = 0; j < n; ++j) {
            cin >> board[i][j].first;
            board[i][j].second = false;
        }
    }

    string str;
    while (true) {
        cout << "Player 1 Score: " << score1 << endl;
        cin >> str;
        if (str == "???") break;
        if (str.length() <= 3) {
            cout << str << " is too short." << endl;
            continue;
        }
        if (!lex.contains(str)) {
            cout << str << " is not a word." << endl;
            continue;
        }
        if (!isValidPath(str)) {
            cout << str << " is not on board." << endl;
            continue;
        } else {
            string word;
            for (int i = 0; i < str.length(); ++i) word.push_back(char(toupper(str[i])));
            if (find(found.begin(), found.end(), word) != found.end()) {
                cout << str << " is already found." << endl;
                continue;
            } else {
                found.push_back(word);
                score1 += str.length() - 3;
                cout << "Correct." << endl;
            }
        }
    }
    found.clear();
    while (true) {
        cout << "Player 2 Score: " << score2 << endl;
        cin >> str;
        if (str == "???") break;
        if (str.length() <= 3) {
            cout << str << " is too short." << endl;
            continue;
        }
        if (!lex.contains(str)) {
            cout << str << " is not a word." << endl;
            continue;
        }
        if (!isValidPath(str)) {
            cout << str << " is not on board." << endl;
            continue;
        } else {
            string word;
            for (int i = 0; i < str.length(); ++i) word.push_back(char(toupper(str[i])));
            if (find(found.begin(), found.end(), word) != found.end()) {
                cout << str << " is already found." << endl;
                continue;
            } else {
                found.push_back(word);
                score2 += str.length() - 3;
                cout << "Correct." << endl;
            }
        }
    }
    cout << "Player 1 Score: " << score1 << endl;
    cout << "Player 2 Score: " << score2 << endl;
    if (score1 > score2) cout << "Player 1 wins!" << endl;
    else if (score1 < score2) cout << "Player 2 wins!" << endl;
    else cout << "It's a tie!" << endl;

    found.clear();
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            allWords(string(1, board[i][j].first), i, j);
    sort(found.begin(), found.end());
    cout << "All Possible Words:";
    for (const auto& word : found) cout << ' ' << word;

    return 0;
}
