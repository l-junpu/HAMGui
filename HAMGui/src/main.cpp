#include "llama.h"

#include "Application.h"

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

class LlamaEngine {
public:
    LlamaEngine(const std::string& model_path, int n_ctx = 2048, int ngl = 99)
        : model_path_(model_path), n_ctx_(n_ctx), ngl_(ngl) {
    }

    bool init() {
        llama_log_set([](enum ggml_log_level level, const char* text, void*) {
            if (level >= GGML_LOG_LEVEL_ERROR) std::cerr << text;
            }, nullptr);

        ggml_backend_load_all();

        llama_model_params model_params = llama_model_default_params();
        model_params.n_gpu_layers = ngl_;
        model_ = llama_model_load_from_file(("C:\\Users\\leeju\\repos\\HAMGui\\HAMGui\\x64\\Release\\" + model_path_).c_str(), model_params);
        if (!model_) {
            std::cerr << "Error: unable to load model: " << model_path_ << "\n";
            return false;
        }

        vocab_ = llama_model_get_vocab(model_);

        llama_context_params ctx_params = llama_context_default_params();
        ctx_params.n_ctx = n_ctx_;
        ctx_params.n_batch = n_ctx_;
        ctx_ = llama_init_from_model(model_, ctx_params);
        if (!ctx_) {
            std::cerr << "Error: failed to create llama context\n";
            return false;
        }

        sampler_ = llama_sampler_chain_init(llama_sampler_chain_default_params());
        llama_sampler_chain_add(sampler_, llama_sampler_init_min_p(0.05f, 1));
        llama_sampler_chain_add(sampler_, llama_sampler_init_temp(0.8f));
        llama_sampler_chain_add(sampler_, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

        return true;
    }

    std::string generate(const std::string& prompt, bool is_first, bool stream = true, std::function<void(const std::string&)> callback = nullptr) {
        std::string response;

        int n_prompt_tokens = -llama_tokenize(vocab_, prompt.c_str(), prompt.size(), nullptr, 0, is_first, true);
        std::vector<llama_token> prompt_tokens(n_prompt_tokens);
        if (llama_tokenize(vocab_, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), is_first, true) < 0) {
            throw std::runtime_error("Failed to tokenize prompt");
        }

        llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());
        llama_token new_token_id;

        while (true) {
            int n_ctx_used = llama_memory_seq_pos_max(llama_get_memory(ctx_), 0) + 1;
            if (n_ctx_used + batch.n_tokens > llama_n_ctx(ctx_)) {
                std::cerr << "Context window exceeded\n";
                break;
            }

            if (llama_decode(ctx_, batch) != 0) {
                throw std::runtime_error("llama_decode failed");
            }

            new_token_id = llama_sampler_sample(sampler_, ctx_, -1);
            if (llama_vocab_is_eog(vocab_, new_token_id)) break;

            char buf[256];
            int n = llama_token_to_piece(vocab_, new_token_id, buf, sizeof(buf), 0, true);
            if (n < 0) throw std::runtime_error("Failed to convert token to text");

            std::string piece(buf, n);
            response += piece;
            if (stream && callback)
                callback(piece);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));

            batch = llama_batch_get_one(&new_token_id, 1);
        }

        return response;
    }

    const llama_vocab* vocab() const { return vocab_; }
    llama_model* model() const { return model_; }
    llama_context* ctx() const { return ctx_; }

    ~LlamaEngine() {
        if (sampler_) llama_sampler_free(sampler_);
        if (ctx_) llama_free(ctx_);
        if (model_) llama_model_free(model_);
    }

private:
    std::string model_path_;
    int n_ctx_;
    int ngl_;

    llama_model* model_ = nullptr;
    llama_context* ctx_ = nullptr;
    llama_sampler* sampler_ = nullptr;
    const llama_vocab* vocab_ = nullptr;

    // chat history
    std::vector<llama_chat_message> messages;
};



























#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <thread>
#include <string>
#include <mutex>
#include <vector>

// Dummy Engine class for demonstration
class Engine {
public:
    // Updated generate to accept a streaming callback
    std::string generate(const std::string& prompt, bool is_first, bool stream = true,
        std::function<void(const std::string&)> callback = nullptr)
    {
        std::string response;

        // Example: just echo each character as if streaming
        for (char c : prompt) {
            std::string piece(1, c);
            response += piece;
            if (stream && callback)
                callback(piece);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }

        return response;
    }
};

class ImGuiLlamaTerminal {
public:
    ImGuiLlamaTerminal(const std::string& modelPath) : engine(modelPath)
    {
        inputBuffer.resize(1024);
        generating = false;
    }

    void Run() {
        if (!Init()) return;

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            DrawTerminal();

            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }

        Cleanup();
    }

private:
    GLFWwindow* window;
    LlamaEngine engine;

    std::string inputBuffer;
    std::string outputBuffer;
    std::mutex outputMutex;
    bool generating;

    std::vector<llama_chat_message> messages;
    std::vector<char> formatted;
    int prev_len;
    const char* tmpl;

    bool Init() {
        if (!glfwInit()) return false;
        window = glfwCreateWindow(800, 600, "ImGui Llama Terminal", NULL, NULL);
        if (!window) return false;
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        // INITIALIZE LLAMA
        if (!engine.init()) {
            std::cerr << "Failed to initialize LlamaEngine\n";
            return false;
        }

        formatted = std::vector<char>(llama_n_ctx(engine.ctx()));
        prev_len = 0;
        const char* tmpl = llama_model_chat_template(engine.model(), nullptr);
        inputBuffer.resize(1024);

        return true;
    }

    void Cleanup() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        if (window) {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    }

    void DrawTerminal() {
        ImGui::Begin("Llama Terminal");

        // Input
        ImGui::InputText("Prompt", &inputBuffer[0], inputBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();
        if (ImGui::Button("Send") && !generating) {
            std::string promptCopy = inputBuffer;
            inputBuffer[0] = '\0';





            // Llama parse
            messages.push_back({ "user", _strdup(promptCopy.c_str()) });
            int new_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), true, formatted.data(), formatted.size());
            if (new_len > (int)formatted.size()) {
                formatted.resize(new_len);
                new_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), true, formatted.data(), formatted.size());
            }
            if (new_len < 0) {
                std::cerr << "Failed to apply chat template\n";
                return;
            }

            std::string prompt(formatted.begin() + prev_len, formatted.begin() + new_len);
            prev_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), false, nullptr, 0);





            generating = true;
            std::thread([this, prompt]() {
                auto callback = [this](const std::string& piece) {
                    std::lock_guard<std::mutex> lock(outputMutex);
                    outputBuffer += piece;
                    };
                auto response = engine.generate(prompt, true, true, callback);
                generating = false;
                messages.push_back({ "assistant", _strdup(response.c_str()) });
                }).detach();
        }

        // Output
        ImGui::Separator();
        {
            std::lock_guard<std::mutex> lock(outputMutex);
            ImGui::TextUnformatted(outputBuffer.c_str());
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }

        ImGui::End();
    }
};

int main() {
    ImGuiLlamaTerminal terminal("models\\Qwen3-4B-Instruct-2507-Q5_K_M.gguf");
    terminal.Run();
    return 0;
}
