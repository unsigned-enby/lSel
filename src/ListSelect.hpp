#ifndef LIST_SEL_H
#define LIST_SEL_H
#include <ftxui/component/component.hpp>

ftxui::Component listSelect(std::vector<std::string> myVec, 
                            bool* &states,
                            int max = -1,
                            bool initState=false,
                            int lines = 10);

#endif
