#include "Camera.h"

void UpdateVectors(struct Transform* transform) {
  transform->forward[0] = (float)(cos(glm_rad(transform->rotation[0])) *
                                  cos(glm_rad(transform->rotation[1])));
  transform->forward[1] = (float)sin(glm_rad(transform->rotation[1]));
  transform->forward[2] = (float)(sin(glm_rad(transform->rotation[0])) *
                                  cos(glm_rad(transform->rotation[1])));

  glm_vec3_normalize(transform->forward);

  vec3 up = {0.f, 1.f, 0.f};

  glm_vec3_cross(up, transform->forward, transform->right);
  glm_vec3_normalize(transform->right);
  glm_vec3_cross(transform->forward, transform->right, transform->up);
}

void SetTranslation(float x, float y, float z, struct Transform* transform) {
  transform->translation[0] = x;
  transform->translation[1] = y;
  transform->translation[2] = z;
};

void SetRotation(float x, float y, float z, struct Transform* transform) {
  transform->rotation[0] = x;
  transform->rotation[1] = y;
  transform->rotation[2] = z;

  UpdateVectors(transform);
};

struct Camera* CreateCamera() {
  // init everything to zero
  struct Camera* camera = calloc(1, sizeof(struct Camera));

  camera->movementSpeed = 7.5f;
  camera->mouseSens = 0.01f;

  glm_perspective(45.f, 640.f / 480.f, 1.f, 1000.f, camera->projectionMatrix);

  // TODO: set in transform initializer ?
  // The position vectors get updated with each translation and rotation,
  // but need to be set to a default first
  camera->transform.forward[0] = 0.f;
  camera->transform.forward[1] = 0.f;
  camera->transform.forward[2] = -1.f;

  SetTranslation(0, 20.f, 0, &(camera->transform));
  // The y rotation can exceed (-90,90) range
  SetRotation(-90.f, -89.f, 0, &(camera->transform));

  return camera;
};

void MoveCamera(float mouseX, float mouseY, int verticalInput,
                int horizontalInput, float deltaTimeS, struct Camera* camera) {
  // camera rotation
  if (camera->mouseHasMoved) {
    float relativeMouseX = mouseX - camera->lastMouseX;
    float relativeMouseY = mouseY - camera->lastMouseY;

    float rotX =
        camera->transform.rotation[0] + relativeMouseX * camera->mouseSens;
    float rotY =
        camera->transform.rotation[1] + -relativeMouseY * camera->mouseSens;

    // clamp y rotation
    if (rotY > 89.9f) {
      rotY = 89.9f;
    } else if (rotY < -89.9f) {
      rotY = -89.9f;
    }

    SetRotation(rotX, rotY, 0, &(camera->transform));

  } else {
    camera->mouseHasMoved = 1;
  }

  camera->lastMouseX = mouseX;
  camera->lastMouseY = mouseY;

  // camera translation
  vec3 zMovement;
  glm_vec3_scale(camera->transform.forward,
                 camera->movementSpeed * verticalInput * deltaTimeS *
                     camera->speedMultiplier,
                 zMovement);
  glm_vec3_add(camera->transform.translation, zMovement,
               camera->transform.translation);

  vec3 xMovement;
  glm_vec3_scale(camera->transform.right,
                 camera->movementSpeed * horizontalInput * deltaTimeS *
                     camera->speedMultiplier,
                 xMovement);
  glm_vec3_add(camera->transform.translation, xMovement,
               camera->transform.translation);

  if ((verticalInput == camera->lastVerticalInput && verticalInput != 0) ||
      (horizontalInput == camera->lastHorizontalInput &&
       horizontalInput != 0)) {
    camera->speedMultiplier += deltaTimeS;
  } else {
    camera->speedMultiplier = 0;
  }

  camera->lastVerticalInput = verticalInput;
  camera->lastHorizontalInput = horizontalInput;

  // view matrix
  vec3 cameraTarget;
  glm_vec3_add(camera->transform.translation, camera->transform.forward,
               cameraTarget);
  glm_lookat(camera->transform.translation, cameraTarget, camera->transform.up,
             camera->viewMatrix);
}