#include "ListToggle.hpp"
#include<fstream>
                     
using namespace ftxui;

using std::vector;
using std::string;
//Significant portions of BooleanItem were copied from the ftxui code
//base (files: src/ftxui/component/{menu,checkbox}.cpp)
Component BooleanItem(vector<bool>* states, string* label, int idx) {
class Impl : public ComponentBase {
   private:
      vector<bool>* States;
      string *Label;
      
      const int Idx;
   public:
      Impl(vector<bool>* states, string* label, int idx) 
      :  States(states), 
         Label(label), 
         Idx(idx) {}
      Element Render() override {
         string retLabel = (Active() ? ">" : " ");
         retLabel += (States->at(Idx) ? ">" : " " ) + *Label;
         Element retElem = text(std::move(retLabel));
         if(Focused()) {
            retElem |= inverted;
         }
         if(Active()) {
            retElem |= bold;
         }
         const bool is_focused = Focused();
         const bool is_active = Active();
         auto focus_management = is_focused ? focus : is_active ? ftxui::select : nothing;
         return retElem | focus_management | reflect(box_);
      }
      bool OnEvent(Event event) override {
         if(!Focused() || !Active()) {
            return false;
         }
         if(event == Event::Character(' ') || event == Event::Return) {
            States->at(Idx) = !States->at(Idx);
            return true;
         }
         return false;
      }
      bool Focusable() const final { return true; };
      Box box_;
};
   return Make<Impl>(states, label, idx);
}

Component ListToggle(vector<string>* choices, vector<bool>* states, bool initState, int* max) {
class Impl : public ComponentBase {
   private:
      vector<bool>*   States;
      vector<string>* Choices;
      bool InitState = false;
      int* Max = nullptr;
      
      Component List = Container::Vertical({});

      void remakeList() {
         List->DetachAllChildren();
         States->clear();
         States->resize(Choices->size(), InitState);
         int i = 0;
         for(auto &choice : *Choices) {
            List->Add(BooleanItem(States, &choice, i++));
         }
      }
   public:
      Impl(vector<string>* &choices, vector<bool>* &states, bool initState, int* max) 
       : Choices(choices), 
         States(states), 
         Max(max) {
         InitState = initState;
         remakeList();
         Add(List);
      };
      Element Render() override {
         return vbox(List->Render() | vscroll_indicator | frame);
      }
      bool OnEvent(Event event) override {
         if(!Focused() || !ActiveChild()) {
            return false;
         }          
         if(event == Event::Character(' ') || event == Event::Return) {
            List->OnEvent(event);
            if(!Max )
               return true;
            int numSelected = 0;
            for(bool item : *States) {
               if(item != InitState) {
                  numSelected++;
               }
            }
            if(numSelected == *Max || numSelected == Choices->size()) {
               Parent()->OnEvent(Event::F1);
            }
            return true;
         } else if(List->OnEvent(event)) { //standard handling
            return true;
         } else {
            return false; //not reached
         }
      }
      bool Focusable() const override {return true;}
};
   return Make<Impl>(choices, states, initState, max);
}
