#ifndef LIST_SEL_H
#define LIST_SEL_H
#include <ftxui/component/component.hpp>

ftxui::Component listSelect(std::vector<std::string> myVec, bool* &states, bool initState=false, int size = 10);

#endif
