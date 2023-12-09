#pragma once

#include <math.h>

#include <cglm/affine.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>

#include "Terrain/Chunk.h"
#include "Terrain/Block.h"

struct Transform {
  vec3 translation;
  vec3 rotation;
  vec3 forward;
  vec3 right;
  vec3 up;
};

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

void CameraMove(float mouseX, float mouseY, int verticalInput,
                int horizontalInput, float deltaTimeS, struct Camera* camera);
