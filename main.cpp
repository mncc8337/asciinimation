#include <ncurses.h>

// sleep
#include <chrono>
#include <thread>

#include <string>
using std::string;
using std::to_string;

#include <iostream>
using std::cout;

#include "reader.h"

void sleep(int ms);
void draw_word_at(string word, int x, int y);
void myrefresh();

Anim animation;

int main(int argc, char** argv) {
    if(argc < 2) {
        cout << "no input file\n";
        return 1;
    }

    int error_line;
    int err = read_file(string(argv[1]), animation, error_line);
    if(err != GOOD) {
        cout << "error on line " << error_line << '\n';
        cout << reading_error_match(err) << '\n';
        return 1;
    }

    initscr();
    curs_set(0);

    animation.play(draw_word_at, myrefresh, sleep);

    endwin();
    return 0;
}

void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
void draw_word_at(string word, int x, int y) {
    mvprintw(y, x, word.c_str());
}
void myrefresh() {
    refresh();
}
