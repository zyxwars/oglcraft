#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define LOG(label, msg) LOG_REMOTE(label, msg, __FILE__, __LINE__)

#define LOG_REMOTE(label, msg, file, line) \
  printf("[%s] %d in %s:%d\n", label, msg, file, line);

#define CALL_GL(x) \
  x;               \
  LogGLError(__FILE__, __LINE__);

static int LogGLError(const char* file, int line) {
  GLenum e;
  while (e = glGetError()) {
    LOG_REMOTE("OpenGL Error", e, file, line);
    return 1;
  }
  return 0;
}

void FileToString(const char* path, char** buffer) {
  FILE* f = fopen(path, "rb");
  if (f == NULL) {
    printf("File not found %s\n", path);
    return;
  }

  fseek(f, 0, SEEK_END);
  int length = ftell(f);
  rewind(f);

  *buffer = malloc(sizeof(char) * length + 1);
  fread(*buffer, sizeof(char), length, f);
  *(*buffer + length) = '\0';

  fclose(f);
}

int CompileShader(GLenum type, const char* shaderSource) {
  GLuint id = CALL_GL(glCreateShader(type));
  CALL_GL(glShaderSource(id, 1, &shaderSource, NULL));
  CALL_GL(glCompileShader(id));

  int success;
  char infoLog[512];
  CALL_GL(glGetShaderiv(id, GL_COMPILE_STATUS, &success));

  if (!success) {
    CALL_GL(glGetShaderInfoLog(id, 512, NULL, infoLog));
    printf("%s\n", shaderSource);
    printf("%s shader compile error: %s\n",
           type == GL_VERTEX_SHADER ? "Vertex" : "Fragment", infoLog);
    return 0;
  }

  return id;
}

int CreateShaderProgram(const char* vertexSourcePath,
                        const char* fragmentSourcePath) {
  char* vertexSource = NULL;
  FileToString(vertexSourcePath, &vertexSource);
  GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
  free(vertexSource);

  char* fragmentSource = NULL;
  FileToString(fragmentSourcePath, &fragmentSource);
  GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
  free(fragmentSource);

  GLuint id = CALL_GL(glCreateProgram());

  CALL_GL(glAttachShader(id, vertexShader));
  CALL_GL(glAttachShader(id, fragmentShader));

  CALL_GL(glLinkProgram(id));

  CALL_GL(glDeleteShader(vertexShader));
  CALL_GL(glDeleteShader(fragmentShader));

  int success;
  char infoLog[512];
  CALL_GL(glGetProgramiv(id, GL_LINK_STATUS, &success));
  if (!success) {
    CALL_GL(glGetProgramInfoLog(id, 512, NULL, infoLog));
    printf("Shader program error: %s\n", infoLog);
  }

  return id;
}

// TODO: support gl under 4.3
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar* message, const void* userParam) {
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}

struct VertexBufferData {
  float vertices[9];
  float colors[9];
};

int main(void) {
  // Init glfw
  GLFWwindow* window;

  if (!glfwInit()) return -1;

  // display scaling
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Init glew
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  // Init gl
  printf("Status: Using GL %s\n", glGetString(GL_VERSION));

  // TODO: Check gl version over 4.3
  CALL_GL(glEnable(GL_DEBUG_OUTPUT));
  CALL_GL(glDebugMessageCallback(MessageCallback, 0));

  struct VertexBufferData vboData = {
      {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f},
      {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f}};

  GLuint vao;
  CALL_GL(glGenVertexArrays(1, &vao));
  CALL_GL(glBindVertexArray(vao));

  GLuint vbo;
  CALL_GL(glGenBuffers(1, &vbo));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
  CALL_GL(
      glBufferData(GL_ARRAY_BUFFER, sizeof(vboData), &vboData, GL_STATIC_DRAW));

  CALL_GL(glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
      (void*)offsetof(struct VertexBufferData, vertices)));
  CALL_GL(
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void*)offsetof(struct VertexBufferData, colors)));
  CALL_GL(glEnableVertexAttribArray(0));
  CALL_GL(glEnableVertexAttribArray(1));

  GLuint shaderProgram = CreateShaderProgram(
      "C:/Users/Zyxwa/Documents/code/prototypes/oglc/src/shaders/shader.vert",
      "C:/Users/Zyxwa/Documents/code/prototypes/oglc/src/shaders/"
      "shader.frag");
  CALL_GL(glUseProgram(shaderProgram));

  float mvp[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

  CALL_GL(GLint MVPUniformLocation =
              glGetUniformLocation(shaderProgram, "u_MVP"));
  CALL_GL(glUniformMatrix4fv(MVPUniformLocation, 1, GL_FALSE, &mvp[0][0]));

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    CALL_GL(glClear(GL_COLOR_BUFFER_BIT));

    CALL_GL(glDrawArrays(GL_TRIANGLES, 0, 3));

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}