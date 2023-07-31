#include "Application.h"

#include "imgui.h"
#include <iostream>

#include "Event.h"

struct BasicButton final : public hammer::Panel
{
    BasicButton( const char* Name ) :
        hammer::Panel( Name )
    {

    }

    void Update(float Dt)
    {
        // Create a simple window with a button and a label
        ImGui::Begin("Hello, ImGui!");
        if (ImGui::Button("Click Me!")) {
            std::cout << "Button clicked!" << std::endl;
        }
        ImGui::Text("Hello, world!");
        ImGui::End();
    }
};

void TestCallback1(int a)
{
    std::cout << "Inc A: " << ++a << std::endl;
}

int main()
{
    //hammer::Application App{ "Pumpkin Chat Window", 1600, 900 };

    hammer::StandaloneEvent<int> SAL_E;

    SAL_E.RegisterEvent<TestCallback1>();
    SAL_E.BroadcastEvent(2);

    //App.AddPanel<BasicButton>("BasicButton");
    //App.Loop();
    
    return 0;
}
