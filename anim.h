#ifndef ANIM_H
#define ANIM_H

#include <vector>
#include <chrono>
#include "frame.h"

using std::vector;

class Anim {
private:
    int mspf;
    vector<Frame> frames;
    char empty_char;
public:
    void set_fps(int fps) {
        mspf = 1.0 / fps * 1000.0 + 0.5;
    }
    void set_empty_char(char chr) {
        empty_char = chr;
    }
    void add_frame(Frame f) {
        frames.push_back(f);
    }
    void clear_frames() {
        frames.clear();
    }
    void play(void (*draw_word_at)(string, int, int),
              void (*refresh)(void),
              void (*sleep)(int)) {
        auto start = std::chrono::system_clock::now();
        std::chrono::duration<double> draw_time;

        for(Frame frame: frames) {
            start = std::chrono::system_clock::now();

            for(int y = 0; y < frame.height; y++) {
                for(auto segment: frame.optimized[y])
                    draw_word_at(segment.content, segment.x, y);
            }
            refresh();

            draw_time = std::chrono::system_clock::now() - start;
            
            // accounting draw time to minimize delay
            int sleep_duration = mspf - draw_time.count() * 1000;
            if(sleep_duration > 0) sleep(sleep_duration);
        }
    }
};

#endif
