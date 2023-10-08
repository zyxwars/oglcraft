#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/affine.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>

#include "GlWrapper.h"

struct Transform {
  vec3 translation;
  vec3 rotation;
  vec3 forward;
  vec3 right;
  vec3 up;
};

void UpdateVectors(struct Transform* transform);

void SetTranslation(float x, float y, float z, struct Transform* transform);

void SetRotation(float x, float y, float z, struct Transform* transform);

struct Camera {
  mat4 projectionMatrix;
  mat4 viewMatrix;
  struct Transform transform;

  // For now camera is the only thing dealilng with input so we handle it here
  float movementSpeed;
  float speedMultiplier;
  int lastVerticalInput;
  int lastHorizontalInput;

  int mouseHasMoved;
  float lastMouseX;
  float lastMouseY;
  float mouseSens;
};

struct Camera* CreateCamera();

void MoveCamera(float mouseX, float mouseY, int verticalInput,
                int horizontalInput, float deltaTimeS, struct Camera* camera);
