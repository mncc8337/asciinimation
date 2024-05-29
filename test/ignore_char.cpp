#include <ncurses.h>

// sleep
#include <chrono>
#include <thread>

#include <string>
using std::string;
using std::to_string;

#include "reader.h"

void sleep(int ms);
void draw_word_at(string word, int x, int y);
void myrefresh();

Anim animation(1, ' ');

int main(int argc, char** argv) {
    animation = read_file("../ignore_char.asn");

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
