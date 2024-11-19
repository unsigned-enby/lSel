#include "ListToggle.hpp"
#include<fstream>
                     
using namespace ftxui;

using std::vector;
using std::string;
//Significant portions of BooleanItem were copied from the ftxui code
//base (files: src/ftxui/component/{menu,checkbox}.cpp)
//Component BooleanItem(vector<bool>* states, string* label, int idx) {
class BooleanItemImpl : public ComponentBase {
   friend class ListToggleImpl;
   private:
      static vector<bool>* States;
      string *Item;
      const int Idx;
   public:
      BooleanItemImpl(string* label, int idx) 
      :  Item(label), 
         Idx(idx) {}
      Element Render() override {
         string retLabel;
         retLabel += (Active() ? ">" : " ");
         retLabel += (States->at(Idx) ? ">" : " " );
         retLabel += *Item;
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
//   return Make<Impl>(states, label, idx);
//}
Component BooleanItem(string* item, int idx) {
   return Make<BooleanItemImpl>(item, idx);
}
class ListToggleImpl : public ComponentBase {
   private:
      vector<bool>*   Statez = nullptr;
      vector<string>* Items  = nullptr;
      bool InitState = false;
      int  MaxLimit  = false;
      
      Component List = Container::Vertical({});

      void remakeList() {
         List->DetachAllChildren();
         Statez->clear();
         Statez->resize(Items->size(), InitState);
         int i = 0;
         for(auto &choice : *Items) {
            List->Add(BooleanItem(&choice, i++));
         }
      }
   public:
      ListToggleImpl(vector<string>* items, vector<bool>* states, bool initState, int maxLimit) 
       : Items(items), 
         Statez(states), 
         MaxLimit(maxLimit),
         InitState(initState) {
         BooleanItemImpl::States = states;
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
            if(!MaxLimit)
               return true;
            int numSelected = 0;
            for(bool item : *Statez)
               if(item != InitState) {
                  numSelected++;
               }
            if(numSelected == MaxLimit || numSelected == Items->size()) {
               Parent()->OnEvent(Event::F1); //tell parent to quit
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

Component ListToggle(vector<string>* items, vector<bool>* states, bool initState, int maxLimit) {
   return Make<ListToggleImpl>(items, states, initState, maxLimit);
}
