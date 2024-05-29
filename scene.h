#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <chrono>
#include "frame.h"

using std::vector;

class Scene {
private:
    int mspf;
public:
    bool fps_defined = false;
    bool dim_defined = false;
    bool empty_char_defined = false;
    bool frame_count_defined = false;

    int width, height;
    int frame_count = -1;
    char empty_char;
    vector<Frame> frames;

    void set_dim(int w, int h) {
        width = w;
        height = h;
        dim_defined = true;
    }
    void set_fps(int fps) {
        mspf = 1.0 / fps * 1000.0 + 0.5;
        fps_defined = true;
    }
    void set_empty_char(char chr) {
        empty_char = chr;
        empty_char_defined = true;
    }
    void set_frame_count(int cnt) {
        frames = vector<Frame>(cnt, Frame(width, height));
        frame_count = cnt;
        frame_count_defined = true;
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
