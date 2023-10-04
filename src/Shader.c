#include "Shader.h"

void FileToString(const char* path, char** buffer) {
  FILE* f = fopen(path, "rb");
  if (f == NULL) {
    printf("File not found %s\n", path);
    return;
  }

  fseek(f, 0, SEEK_END);
  // TODO: this isn't recommended and reads in a few extra garbage bytes
  int length = ftell(f);
  fseek(f, 0, SEEK_SET);

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