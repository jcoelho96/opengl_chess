#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>
#include <signal.h>

#include "Board.h"

#define ASSERT(x) if (!(x)) raise(SIGTRAP);
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "): " << function << 
            " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[3];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
            if (type == ShaderType::NONE) {
                std::cout << "ERROR: Failed to determine Shader Type." << std::endl;
                return {"", ""};
            }
            ss[(int)type] << line << '\n';
        }
    }

    return {
        ss[(int)ShaderType::VERTEX].str(),
        ss[(int)ShaderType::FRAGMENT].str()
    };
}

std::unordered_map<unsigned int, std::string> shader_type_to_string = {
    {GL_VERTEX_SHADER, "Vertex"},
    {GL_FRAGMENT_SHADER, "Fragment"}
};

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*) alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " << shader_type_to_string[type] << " shader.\n"; 
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));

        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

Board *board;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    std::cout << "callback!\n";
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double cursor_x, cursor_y;
        glfwGetCursorPos(window, &cursor_x, &cursor_y);

        board->ToggleSelected(cursor_x, cursor_y);
    }
}

int main(void) {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(600, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "glew init failed.\n";
        return -1;
    }

    board = new Board();

    unsigned int buffer; // this won't hold the actual data: it holds an id that refers to the buffer
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 9 * 9 * 2 * sizeof(float), board->GetVertexPositions(), GL_STATIC_DRAW));
    
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*) 0));

    ShaderProgramSource source = ParseShader("../res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));

    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
            /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        double cursor_x, cursor_y;
        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        board->ClearHighlights();
        board->CheckHighlights(cursor_x, cursor_y);

        for (auto space: board->GetSpaces()) {
            if (space.GetSpaceColor() == SpaceColor::DARK) {
                if (space.IsSelected()) {
                    GLCall(glUniform4f(location, 0.35f, 0.35f, 0.35f, 1.0f));
                } else if (space.IsHighlighted()) {
                    GLCall(glUniform4f(location, 0.3f, 0.3f, 0.3f, 1.0f));
                } else {
                    GLCall(glUniform4f(location, 0.2f, 0.2f, 0.2f, 1.0f));
                }
            } else {
                if (space.IsSelected()) {
                    GLCall(glUniform4f(location, 0.95f, 0.95f, 0.95f, 1.0f));
                } else if (space.IsHighlighted()) {
                    GLCall(glUniform4f(location, 0.9f, 0.9f, 0.9f, 1.0f));
                } else {
                    GLCall(glUniform4f(location, 0.8f, 0.8f, 0.8f, 1.0f));
                }
            }

            auto bounding_box = space.GetIndices();
            int indices[6] = {
                bounding_box[0], bounding_box[1], bounding_box[2],
                bounding_box[2], bounding_box[3], bounding_box[1]
            };

            unsigned int ibo = space.GetIBO();
            GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
            GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW));
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        // std::this_thread::sleep_for(std::chrono::seconds(3));
        // break;
    }

    GLCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}