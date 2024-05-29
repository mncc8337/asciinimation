#include <iostream>
#include <string>
#include "frame.h"

using std::cout;
using std::cin;
using std::getline;
using std::string;

int main() {
    cout << "testing frame optimizer ...\n";
    cout << "input: <width> <height>\n";
    cout << "       <\n";
    cout << "        the frame content\n";
    cout << "        ...\n";
    cout << "       >\n";
    cout << "output: ------------------\n";
    cout << "        <segment start pos> <segment content>\n";
    cout << "        ------------------\n";
    cout << "        ...\n\n>>> ";

    int w, h;
    cin >> w >> h;

    Frame frame(w, h);

    cin.ignore();
    for(int i = 0; i < h; i++) {
        string s;
        getline(cin, s);
        s.erase(s.begin() + w, s.end());

        frame.content[i] = s;
    }

    frame.optimize(' ');

    cout << "-----------------------\n";
    for(auto row: frame.optimized) {
        for(auto segment: row)
            cout << segment.x << ' ' << segment.content << '\n';
        cout << "-----------------------\n";
    }
}
