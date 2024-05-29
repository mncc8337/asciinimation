#include <ncurses.h>

// sleep
#include <chrono>
#include <thread>

#include <string>
using std::string;
using std::to_string;

#include <iostream>
using std::cout;

#include "compiler.h"

void sleep(int ms);
void draw_word_at(string word, int x, int y);
void myrefresh();

Instructor ins;

int main(int argc, char** argv) {
    if(argc < 2) {
        cout << "no input file\n";
        return 1;
    }

    int error_line;
    int err = compile_file(string(argv[1]), ins, error_line);
    if(err != GOOD) {
        cout << "error on line " << error_line << '\n';
        cout << reading_error_match(err) << '\n';
        return 1;
    }

    ins.set_play_func_params(draw_word_at, myrefresh, sleep);

    initscr();
    curs_set(0);

    ins.do_it();

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
