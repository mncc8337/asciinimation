#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <string>

using std::vector;
using std::string;

struct row_segment {
    int x;
    string content;
};

class Frame {
public:
    int width;
    int height;
    vector<string> content;
    vector<vector<row_segment>> optimized;
    bool is_optimized = false;

    Frame(int w, int h) {
        width = w;
        height = h;

        content = vector<string>(h);
        optimized = vector<vector<row_segment>>(h);
    }

    // break rows into segments for fast printing
    void optimize(char empty_char) {
        for(int y = 0; y < height; y++) {
            string row = content[y];

            row_segment current_segment;
            bool start_new = false;
            for(int x = 0; x < width; x++) {
                if(row[x] != empty_char) {
                    if(!start_new) {
                        current_segment.x = x;
                        current_segment.content = row[x];
                        start_new = true;
                    }
                    else current_segment.content += row[x];
                }
                else {
                    if(start_new) {
                        optimized[y].push_back(current_segment);
                        current_segment.content.clear();
                        start_new = false;
                    }
                    else continue;
                }
            }

            if(current_segment.content.length() > 0) 
                optimized[y].push_back(current_segment);
        }

        // discard content
        content.clear();
        is_optimized = true;
    }
};

#endif
