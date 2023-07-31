#include "Application.h"
#include "fonts/static/Ruda-Bold.h"

#include <filesystem>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

namespace hammer
{
	Application::Application( const ApplicationSpecs& Specs ) :
		m_Specs{ Specs }
	{
		InitializeWindow();
		InitializeTheme();
	}

	Application::Application( const std::string& WinName, uint32_t Width, uint32_t Height ) :
		m_Specs{ ApplicationSpecs{ .m_Name   = WinName
								 , .m_Width  = Width
								 , .m_Height = Height } }
	{
		InitializeWindow();
		InitializeTheme();
	}

	Application::~Application( void )
	{
		TerminateWindow();
	}

	void Application::RemovePanel( const char* PanelName ) noexcept
	{
		if ( PanelExists(PanelName) )
		{
			auto Index      = m_PanelMapping.find( PanelName )->second;
			auto FinalIndex = m_PanelList.size() - 1;
			auto FinalName  = m_PanelList.back()->m_Name;

			if ( Index != FinalIndex )
			{
				m_PanelList[ Index ]->OnDetach();
				m_PanelList[ Index ] = std::move( m_PanelList.back() );
				m_PanelMapping[FinalName] = Index;
			}

			m_PanelList.pop_back();
			m_PanelMapping.erase(PanelName);
		}
	}

	void Application::Loop( void ) noexcept
	{
		// Main loop
		while ( m_Running && !glfwWindowShouldClose(m_pApplication) )
		{
			m_Clock.StartFrame();
			glfwPollEvents();

			// Start the ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// Docking space
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

			// Update user custom panels
			auto Dt = m_Clock.GetDt();
			for (auto& p : m_PanelList) p->PreUpdate (Dt);
			for (auto& p : m_PanelList) p->Update    (Dt);
			for (auto& p : m_PanelList) p->PostUpdate(Dt);

			// Your ImGui interface here
			ImGui::ShowDemoWindow(); // Example ImGui demo window

			// Rendering
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(m_pApplication, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Update width & height
			m_Specs.m_Width  = display_w;
			m_Specs.m_Height = display_h;

			// Swap front and back buffers
			glfwSwapBuffers(m_pApplication);
			m_Clock.EndFrame();
		}
	}

	int Application::InitializeWindow(void)
	{
		// Initialize GLFW
		if (!glfwInit())
		{
			return -1;
		}

		// Create a windowed mode window and its OpenGL context
		m_pApplication = glfwCreateWindow(m_Specs.m_Width, m_Specs.m_Height, m_Specs.m_Name.c_str(), NULL, NULL);
		if (!m_pApplication)
		{
			glfwTerminate();
			return -1;
		}

		// Make the window's context current
		glfwMakeContextCurrent(m_pApplication);

		// Initialize ImGui with GLFW and OpenGL
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui_ImplGlfw_InitForOpenGL(m_pApplication, true);
		ImGui_ImplOpenGL3_Init("#version 410");

		// Enable docking support
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		return 0;
	}

	void Application::InitializeTheme( void )
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImGuiIO*    io    = &ImGui::GetIO();

		// Customizing Panels
		style->WindowPadding = ImVec2(15, 15);
		style->WindowRounding = 5.0f;
		style->FramePadding = ImVec2(5, 5);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(12, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;

		style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Separator] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

		// Loading Compressed Font
		ImFontConfig FontTemplate;
		strcpy_s( FontTemplate.Name, 40, "RudaBold" );
		ImFont* font1 = io->Fonts->AddFontFromMemoryCompressedTTF( RudaBold_compressed_data, RudaBold_compressed_size, 15.0f );
	}

	void Application::TerminateWindow( void )
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwTerminate();
	}
	bool Application::PanelExists( const std::string& PanelName )
	{
		return m_PanelMapping.find( PanelName ) != m_PanelMapping.end();
	}
}