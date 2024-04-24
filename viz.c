#include <stdio.h>
#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define UTILS_IMPLEMENTATION
#include "./utils.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

GLuint compile_shader_file(const char *file_path, GLenum shader_type) {
  GLuint shader = glCreateShader(shader_type);
  GLchar *source = slurp_file(file_path);
  glShaderSource(shader, 1, (const GLchar **)&source, NULL);
  glCompileShader(shader);

  GLint compiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

  if (!compiled) {
    GLchar message[1024];
    GLsizei message_size = 0;
    glGetShaderInfoLog(shader, sizeof(message), &message_size, message);
    log_err("%s:%.*s", file_path, message_size - 1, message);
  }

  free(source);

  return shader;
}

GLuint link_program(GLuint vert_shader, GLuint frag_shader) {
  GLuint program = glCreateProgram();

  glAttachShader(program, vert_shader);
  glAttachShader(program, frag_shader);
  glLinkProgram(program);

  GLint linked = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);

  if (!linked) {
    GLchar message[1024];
    GLsizei message_size = 0;
    glGetProgramInfoLog(program, sizeof(message), &message_size, message);
    log_err("%.*s", message_size - 1, message);
  }

  return program;
}

void window_size_callback(GLFWwindow *window, int width, int height) {
  UNUSED(window);
  glViewport(width / 2 - SCREEN_WIDTH / 2, height / 2 - SCREEN_HEIGHT / 2,
             SCREEN_WIDTH, SCREEN_HEIGHT);
}

GLuint program;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int modes) {
  FOR_EACH(UNUSED, window, scancode, action, modes);
  if (key == GLFW_KEY_F5) {
    glDeleteProgram(program);
    program = link_program(compile_shader_file("viz.vert", GL_VERTEX_SHADER),
                           compile_shader_file("viz.frag", GL_FRAGMENT_SHADER));
    glUseProgram(program);
  }
}

int main(void) {
  size_t result = 0;
  GLFWwindow *window = NULL;

  if (!glfwInit()) {
    log_err("could not initialize GLFW");
    return_defer(1);
  }

  window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Caton", NULL, NULL);
  if (!window) {
    log_err("could not initialize window");
    return_defer(1);
  }

  glfwMakeContextCurrent(window);

  program = link_program(compile_shader_file("viz.vert", GL_VERTEX_SHADER),
                         compile_shader_file("viz.frag", GL_FRAGMENT_SHADER));

  glUseProgram(program);

  glfwSetFramebufferSizeCallback(window, window_size_callback);
  glfwSetKeyCallback(window, key_callback);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_QUADS, 0, 4);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

defer:
  if (glfwInit()) glfwTerminate();
  if (window != NULL) glfwDestroyWindow(window);
  return result;
}
