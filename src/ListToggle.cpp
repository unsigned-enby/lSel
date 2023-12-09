#include "ListToggle.hpp"
using namespace ftxui;
using std::vector;
using std::string;

Component ListToggle(vector<string>* choices, vector<bool>* states, bool initState, int max) {
class Impl : public ComponentBase {
   private:
      vector<bool>*   States;
      vector<string>* Choices;
      bool InitState;
      int  Max = -1;
      
      //this nonsense is temporary until missing Checkbox(CheckboxOption) func call
      //is fixed
      bool*  StatesInternal = nullptr;
      
      int    Selected = 0;
      Component List = Container::Vertical({}, &Selected);

      void remakeList() {
         List->DetachAllChildren();
         delete [] StatesInternal;
         States->resize(Choices->size(), InitState);
         StatesInternal = new bool[Choices->size()];
         List = Container::Vertical({}, &Selected);
         bool* stateInternal = &StatesInternal[0];
         for(auto &choice : *Choices) {
            *stateInternal = InitState;
            List->Add(Checkbox(choice, stateInternal++));
         }
      }
   public:
      Impl(vector<string>* &choices, vector<bool>* &states, bool initState, int max) 
       : Choices(choices), States(states) {
         Max = max;
         InitState = initState;
         if(Max != -1)
            Max = Max > Choices->size() ? Choices->size() : Max;
         remakeList();
         Add(List);
      };
      Element Render() override {
         return vbox(ChildAt(0)->Render() | vscroll_indicator | frame);
      }
      bool OnEvent(Event event) override {
         if(!Focused() || !ActiveChild()) {
            return false;
         }          
         if(event == Event::Return) {
            States->at(Selected) = !States->at(Selected);
            StatesInternal[Selected] = States->at(Selected);
            if(Max == -1)
               return true;
            int numSelected = 0;
            for(bool item : *States) {
               if(item != InitState) {
                  numSelected++;
               }
            }
            if(numSelected == Max) {
               Parent()->OnEvent(Event::F1);
            }
            return true;
         }          
         if(ActiveChild()->OnEvent(event)) { //standard handling
            return true;
         } else {
            return false; //not reached
         }
      }
      bool Focusable() const override {return true;}
};
   return Make<Impl>(choices, states, initState, max);
}
