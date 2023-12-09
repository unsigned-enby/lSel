#ifndef LIST_SEL_H
#define LIST_SEL_H
#include <ftxui/component/component.hpp>

ftxui::Component ListToggle(std::vector<std::string>* choices,
                            std::vector<bool>*        states,
                            bool initState = false,
                            int  max = -1);

#endif
