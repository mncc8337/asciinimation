#ifndef INSTRUCTOR_H
#define INSTRUCTOR_H

#include <map>
#include <string>
#include <vector>
#include "scene.h"

using std::map;
using std::string;
using std::vector;

struct Method {
    string method_name;
    int scene_id;
    int param;
};

class Instructor {
private:
    void (*draw_word_at)(string, int, int);
    void (*refresh)(void);
    void (*sleep)(int);
public:
    map<string, int> scene_code;
    vector<Scene> scenes;
    vector<Method> instructions;

    void set_play_func_params(void (*_draw_word_at)(string, int, int),
                              void (*_refresh)(void),
                              void (*_sleep)(int)) {
        draw_word_at = _draw_word_at;
        refresh = _refresh;
        sleep = _sleep;
    }

    void add_instruction(string method_name, int scene_id, int param) {
        instructions.push_back({method_name, scene_id, param});
    }

    void do_it() {
        for(int i = 0; i < instructions.size(); i++) {
            Method& m = instructions[i];

            if(m.method_name == "PLAY")
                scenes[m.scene_id].play(draw_word_at, refresh, sleep);
            else if(m.method_name == "REP") {
                int iter = m.param;
                for(int i = 1; i <= iter; i++)
                    scenes[m.scene_id].play(draw_word_at, refresh, sleep);
            }
        }
    }
};

#endif
