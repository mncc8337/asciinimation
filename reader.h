#ifndef READER_H
#define READER_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "anim.h"

using std::ifstream;
using std::string;
using std::istringstream;
using std::getline;
using std::vector;

enum ERROR_TYPE {
    GOOD,
    BAD_FILE,
    MISSING_FPS,
    MISSING_DIMENSION,
    MISSING_IGNORE,
    MISSING_FRAMES,
    MISSING_FRAME_NUMBER,
    UNKNOWN_KEYWORD
};

string reading_error_match(int err) {
    switch(err) {
        case GOOD:
            return "no errors";
        case BAD_FILE:
            return "cannot read file";
        case MISSING_FPS:
            return "no FPS provided";
        case MISSING_DIMENSION:
            return "no DIMENSION provided";
        case MISSING_IGNORE:
            return "no IGNORE char provided";
        case MISSING_FRAMES:
            return "no FRAMES count provided";
        case MISSING_FRAME_NUMBER:
            return "no FRAME number provided";
        case UNKNOWN_KEYWORD:
            return "unknown keyword";
        default:
            return "this message will never be printed out if all errors were covered";
    }
}

int read_file(string filename, Anim& anim, int& current_line) {
    current_line = 0;

    ifstream file(filename);
    if(file.bad() or file.fail()) return BAD_FILE;

    int fps;
    char empty_char;

    int w, h;

    int frame_count = 0;
    vector<Frame> frames;

    bool fps_defined = false;
    bool dimension_defined = false;
    bool ignore_defined = false;
    bool frames_defined = false;

    string line;
    string keyword;
    string temp;
    while(getline(file, line)) {
        current_line++;
        // ignore comment line
        if(line[0] == '#') continue;
        // ignore empty line
        if(line.length() == 0) continue;

        istringstream iss(line);
        iss >> keyword;

        if(keyword == "FPS") {
            iss >> fps;
            fps_defined = true;
        }
        else if(keyword == "DIMENSION") {
            iss >> w >> h;
            dimension_defined = true;
        }
        else if(keyword == "FRAMES") {
            iss >> frame_count;
            frames = vector<Frame>(frame_count, Frame(w, h));
            frames_defined = true;
        }
        else if(keyword == "IGNORE") {
            iss >> temp;
            if(temp.length() == 1) empty_char = temp[0];
            else if(temp.length() == 3) empty_char = temp[1];
            ignore_defined = true;
        }
        else if(keyword == "FRAME") {
            int frame_no;
            if(!(iss >> frame_no)) return MISSING_FRAME_NUMBER;

            if(!dimension_defined) return MISSING_DIMENSION;
            if(!frames_defined) return MISSING_FRAMES;
            frames[frame_no] = Frame(w, h);

            // start copying frame to mem
            int line_no = 0;
            getline(file, line);
            current_line++;
            while(line != "END") {
                while(line.length() < w) line += ' ';

                frames[frame_no].content[line_no] = line;
                line_no++;
                getline(file, line);
                current_line++;
            }
            string empty_line(w, ' ');
            while(line_no < h) {
                frames[frame_no].content[line_no] = empty_line;
                line_no++;
            }

            if(!ignore_defined) return MISSING_IGNORE;
            frames[frame_no].optimize(empty_char);
        }
        else {
            return UNKNOWN_KEYWORD;
        }
    }

    if(!fps_defined) return MISSING_FPS;
    if(!ignore_defined) return MISSING_IGNORE;
    if(!dimension_defined) return MISSING_DIMENSION;

    anim.clear_frames();
    anim.set_fps(fps);
    anim.set_empty_char(empty_char);
    // fill in blank frame
    Frame blank(w, h);
    blank.content = vector<string>(h, string(w, empty_char));
    for(Frame frame: frames) {
        if(frame.is_optimized) anim.add_frame(frame);
        else anim.add_frame(blank);
    }

    return GOOD;
}

#endif
