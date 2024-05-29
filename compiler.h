#ifndef COMPILER_H
#define COMPILER_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "scene.h"
#include "instructor.h"

using std::ifstream;
using std::string;
using std::istringstream;
using std::getline;
using std::vector;
using std::map;

#include <iostream>
using std::cout;

enum ERROR_TYPE {
    GOOD,
    ERR_BAD_FILE,
    ERR_MISSING_SCENE_NAME,
    ERR_SCENE_NOT_DEFINED,
    ERR_NO_SCENE_SELECTED,
    ERR_MISSING_PARAMS,
    ERR_DIMENSION_NOT_DEFINED,
    ERR_INVALID_SYNTAX,
    ERR_IGNORE_NOT_DEFINED,
    ERR_SCENE_SELECTING,
    ERR_UNKNOWN_KEYWORD
};

enum MODE {
    MODE_NORMAL,
    MODE_ADDING_FRAME,
    MODE_ADDING_INSTRUCTION
};

string reading_error_match(int err) {
    switch(err) {
        case GOOD:
            return "no errors";
        case ERR_BAD_FILE:
            return "cannot read file";
        case ERR_MISSING_SCENE_NAME:
            return "missing scene name";
        case ERR_SCENE_NOT_DEFINED:
            return "scene is not defined";
        case ERR_NO_SCENE_SELECTED:
            return "no scene selected";
        case ERR_MISSING_PARAMS:
            return "not enough param(s)";
        case ERR_DIMENSION_NOT_DEFINED:
            return "dimension not defined";
        case ERR_INVALID_SYNTAX:
            return "invalid syntax";
        case ERR_IGNORE_NOT_DEFINED:
            return "ignore char not set";
        case ERR_SCENE_SELECTING:
            return "cannot create instruction while selecting a scene";
        case ERR_UNKNOWN_KEYWORD:
            return "unknown keyword";
        default:
            return "this message will never be printed out if all errors were covered";
    }
}

int compile_file(string filename, Instructor& ins, int& current_line) {
    current_line = 0;

    ifstream file(filename);
    if(file.bad() or file.fail()) return ERR_BAD_FILE;

    vector<Scene> scenes;
    map<string, int> scene_code;
    int selecting_scene = -1;

    int current_mode = MODE_NORMAL;
    int current_frame = -1;
    int frame_line_cnt = 0;

    ins.instructions.clear();

    string line;
    string keyword;
    while(getline(file, line)) {
        current_line++;

        if(current_mode == MODE_NORMAL) {
            // ignore comment line
            if(line[0] == '#') continue;
            // ignore empty line
            if(line.length() == 0) continue;

            istringstream iss(line);
            iss >> keyword;

            if(keyword == "DEF_SCENE") {
                string scene_name;
                if(!(iss >> scene_name)) return ERR_MISSING_SCENE_NAME;

                scenes.push_back(Scene());
                scene_code[scene_name] = (int)scenes.size() - 1;
            }
            else if(keyword == "MOD_SCENE") {
                string scene_name;
                if(!(iss >> scene_name)) return ERR_MISSING_SCENE_NAME;
                
                if(scene_code.find(scene_name) == scene_code.end())
                    return ERR_SCENE_NOT_DEFINED;
                selecting_scene = scene_code[scene_name];
            }
            else if(keyword == "DONE_MOD") {
                selecting_scene = -1;
            }
            else if(keyword == "FPS") {
                if(selecting_scene == -1)
                    return ERR_NO_SCENE_SELECTED;

                int fps;
                if(!(iss >> fps)) return ERR_MISSING_PARAMS;
                scenes[selecting_scene].set_fps(fps);
            }
            else if(keyword == "DIMENSION") {
                if(selecting_scene == -1)
                    return ERR_NO_SCENE_SELECTED;
                
                int w, h;
                if(!(iss >> w >> h)) return ERR_MISSING_PARAMS;
                scenes[selecting_scene].set_dim(w, h);
            }
            else if(keyword == "FRAMES") {
                if(selecting_scene == -1)
                    return ERR_NO_SCENE_SELECTED;
                if(!scenes[selecting_scene].dim_defined)
                    return ERR_DIMENSION_NOT_DEFINED;
                
                int frames;
                if(!(iss >> frames)) return ERR_MISSING_PARAMS;
                scenes[selecting_scene].set_frame_count(frames);
            }
            else if(keyword == "IGNORE") {
                if(selecting_scene == -1)
                    return ERR_NO_SCENE_SELECTED;

                string temp;
                if(!(iss >> temp)) return ERR_MISSING_PARAMS;
                if(temp.length() == 1)
                    scenes[selecting_scene].set_empty_char(temp[0]);
                else if(temp.length() == 3)
                    scenes[selecting_scene].set_empty_char(temp[1]);
                else
                    return ERR_INVALID_SYNTAX;
            }
            else if(keyword == "FRAME") {
                if(selecting_scene == -1)
                    return ERR_NO_SCENE_SELECTED;
                if(!scenes[selecting_scene].empty_char_defined)
                    return ERR_IGNORE_NOT_DEFINED;
                if(!scenes[selecting_scene].dim_defined)
                    return ERR_DIMENSION_NOT_DEFINED;
                
                if(!(iss >> current_frame)) return ERR_MISSING_PARAMS;
                current_mode = MODE_ADDING_FRAME;
            }
            else if(keyword == "INSTRUCTION") {
                if(selecting_scene != -1)
                    return ERR_SCENE_SELECTING;
                
                current_mode = MODE_ADDING_INSTRUCTION;
            }
            else {
                return ERR_UNKNOWN_KEYWORD;
            }
        }
        else if(current_mode == MODE_ADDING_FRAME) {
            if(line == "END") {
                string empty_line(scenes[selecting_scene].width, ' ');
                while(frame_line_cnt < scenes[selecting_scene].height) {
                    scenes[selecting_scene].frames[current_frame].content[frame_line_cnt] = empty_line;
                    frame_line_cnt++;
                }
                scenes[selecting_scene].frames[current_frame].optimize(scenes[selecting_scene].empty_char);

                frame_line_cnt = 0;
                current_frame = -1;
                current_mode = MODE_NORMAL;
            }
            else {
                while(line.length() < scenes[selecting_scene].width) line += ' ';
                scenes[selecting_scene].frames[current_frame].content[frame_line_cnt] = line;
                frame_line_cnt++;
            }
        }
        else if(current_mode == MODE_ADDING_INSTRUCTION) {
            if(line == "END") {
                ins.scene_code = scene_code;
                ins.scenes = scenes;
                current_mode = MODE_NORMAL;
            }
            else {
                // ignore comment line
                if(line[0] == '#') continue;
                // ignore empty line
                if(line.length() == 0) continue;

                istringstream iss(line);
                iss >> keyword;

                if(keyword == "PLAY") {
                    string scene_name;
                    if(!(iss >> scene_name)) return ERR_MISSING_PARAMS;
                    int scene_id = scene_code[scene_name];
                    ins.add_instruction("PLAY", scene_id, 0);
                }
                else if(keyword == "REP") {
                    string scene_name;
                    int iter;
                    if(!(iss >> scene_name >> iter)) return ERR_MISSING_PARAMS;
                    int scene_id = scene_code[scene_name];
                    ins.add_instruction("REP", scene_id, iter);
                }
                else return ERR_UNKNOWN_KEYWORD;
            }
        }
    }

    return GOOD;
}

#endif
