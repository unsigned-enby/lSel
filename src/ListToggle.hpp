#ifndef LIST_SEL_H
#define LIST_SEL_H
#include <ftxui/component/component.hpp>

ftxui::Component ListToggle(std::vector<std::string>* items,
                            std::vector<bool>*        states,
                            bool initState = false,
                            int  maxLimit  = false);

#endif
