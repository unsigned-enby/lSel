#include "ListSelect.hpp"
using namespace ftxui;
using std::vector;
using std::string;
Component listSelect(vector<string> myVec, bool* &states, int max, bool initState, int lines) {
   class Impl : public ComponentBase {
      private:
         bool* States;
         int Max  = -1;
         bool InitState = false;
         int Lines = -1;
         int Count = -1;
         Component List = Container::Vertical({});
         bool maxSelections() {
            if(Max == -1)
               return false;
            int numSelected = 0;
            for(int i=0; i < Count ; i++) {
               if(States[i] != InitState)
                  numSelected++;
            }
            if(numSelected >= Max)
               return true;
            else
               return false;
         }
      public:
         Impl(vector<string> myVec, bool* &states, int max, bool negate, int lines) : InitState(negate), Lines(lines) {
            Count = myVec.size();
            if(max > Count)
               Max = Count;
            else
               Max = max;
            States = new bool[Count];
            for(int i=0; i < Count ; i++) {
              States[i] = InitState;
              List->Add(Checkbox(myVec[i], &States[i]));
            }
            states = &States[0];
         };
         Element Render() override {
            return List->Render() | vscroll_indicator | frame |
                           ftxui::size(HEIGHT, LESS_THAN, Lines) | border;
         }
         bool OnEvent(Event event) override {
            bool retBool = List->OnEvent(event);
            if(event == Event::Return || event == Event::Character(' '))
               if(maxSelections())
                  Parent()->OnEvent(Event::Character('y')); //exit the screen loop
            return retBool;
         }
   };
   return Make<Impl>(myVec, states, max, initState, lines);
}
