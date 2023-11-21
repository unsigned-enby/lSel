#include "ListSelect.hpp"
using namespace ftxui;
using std::vector;
using std::string;
Component listSelect(vector<string> myVec, bool* &statez, bool initState, int size) {
   class Impl : public ComponentBase {
      private:
         int count = -1;
         int size = -1;
         bool* states;
         Component List = Container::Vertical({});
      public:
         Impl(vector<string> myVec, bool* &statez, bool initState, int size) : size(size) {
            count = myVec.size();
            states = new bool[count];
            for(int i=0; i < count ; i++) {
              states[i] = initState;
              List->Add(Checkbox(myVec[i], &states[i]));
            }
            statez = &states[0];
         };
         Element Render() override {
            return List->Render() | vscroll_indicator | frame |
                           ftxui::size(HEIGHT, LESS_THAN, size) | border;
         }
         bool OnEvent(Event event) override {
            return List->OnEvent(event);
         }
   };
   return Make<Impl>(myVec, statez, initState, size);
}
