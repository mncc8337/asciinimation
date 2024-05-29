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

inline Anim read_file(string filename) {
    ifstream file(filename);

    int fps;
    char empty_char;

    int w, h;

    int frame_count = 0;
    vector<Frame> frames;

    string line;
    string temp;
    while(getline(file, line)) {
        // ignore comment line
        if(line[0] == '#') continue;

        istringstream iss(line);
        string keyword;
        iss >> keyword;

        if(keyword == "FPS")
            iss >> fps;
        else if(keyword == "DIMENSION") {
            iss >> w >> h;
        }
        else if(keyword == "FRAMES") {
            iss >> frame_count;
            frames = vector<Frame>(frame_count, Frame(w, h));
        }
        else if(keyword == "IGNORE") {
            iss >> temp;
            if(temp.length() == 1) empty_char = temp[0];
            else if(temp.length() == 3) empty_char = temp[1];
        }
        else if(keyword == "FRAME") {
            int frame_no;
            iss >> frame_no;

            frames[frame_no] = Frame(w, h);
            // start copying frame to mem
            int line_no = 0;
            getline(file, line);
            while(line != "END") {
                while(line.length() < w) line += ' ';

                frames[frame_no].content[line_no] = line;
                line_no++;
                getline(file, line);
            }
            string empty_line(w, ' ');
            while(line_no < h) {
                frames[frame_no].content[line_no] = empty_line;
                line_no++;
            }

            frames[frame_no].optimize(empty_char);
        }
    }

    Anim result(fps, empty_char);
    // fill in blank frame
    Frame blank(w, h);
    blank.content = vector<string>(h, string(w, empty_char));
    for(Frame frame: frames) {
        if(frame.is_optimized) result.add_frame(frame);
        else result.add_frame(blank);
    }

    return result;
}

#endif
