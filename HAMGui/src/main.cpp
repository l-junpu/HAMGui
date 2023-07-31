#include "Application.h"

#include "imgui.h"
#include <iostream>

// Just testing locally
#include "Event.h"
#include "EventManager.h"

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

void TestCallback1(int a, float b)
{
    std::cout << "++A: " << ++a << std::endl;
    b += a;
    std::cout << "A + B: " << ++a << std::endl;
}

using MyTestEvent = hammer::StandaloneEvent<int, float>;
struct StructTestEvent : hammer::StandaloneEvent<int, float> {}; // recommended to use this method - define a struct - we should create macros to facilitate the ease of creation
using MyClassEvent = hammer::ClassEvent<int, float>;

struct TestClass
{
    void FunnyConversion(int a, float b)
    {
        std::cout << "++A: " << ++a << std::endl;
        b += a;
        std::cout << "A + B: " << ++a << std::endl;
    }
};

int main()
{
    hammer::EventMgr.RegisterStandaloneEvent<MyTestEvent, TestCallback1>();
    hammer::EventMgr.RegisterStandaloneEvent<StructTestEvent, TestCallback1>();
    hammer::EventMgr.BroadcastEvent<MyTestEvent>(69, 0.0f);
    hammer::EventMgr.BroadcastEvent<StructTestEvent>(69, 0.0f);

    TestClass testttt;
    hammer::EventMgr.RegisterClassEvent<MyClassEvent, &TestClass::FunnyConversion>(&testttt);
    hammer::EventMgr.BroadcastEvent<MyClassEvent>(70, 0.0f);


    //hammer::Application App{ "Pumpkin Chat Window", 1600, 900 };
    //hammer::StandaloneEvent<int> SAL_E;
    //SAL_E.RegisterEvent<TestCallback1>();
    //SAL_E.BroadcastEvent(2);

    //App.AddPanel<BasicButton>("BasicButton");
    //App.Loop();
    
    return 0;
}
