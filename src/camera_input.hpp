///
/// BRIEF: Camera controller that uses Input structure.
///

namespace {

#include "input.hpp"
#include "camera.hpp"


void camera_input_apply(Camera *camera, Input *input) {
  camera->rotate(-input->mouse_delta.y/6*(MATH_TAU/360), -input->mouse_delta.x/6*(MATH_TAU/360));
  float speed = sapp_frame_duration()*10;

  if (input->key_states[SAPP_KEYCODE_LEFT_SHIFT].held) {
    camera->move(0, 0, -speed);
  }

  if (input->key_states[SAPP_KEYCODE_SPACE].held) {
    camera->move(0, 0, speed);
  }

  if (input->key_states[SAPP_KEYCODE_W].held) {
    camera->move(-speed, 0, 0);
  }

  if (input->key_states[SAPP_KEYCODE_S].held) {
    camera->move(speed, 0, 0);
  }

  if (input->key_states[SAPP_KEYCODE_A].held) {
    camera->move(0, -speed, 0);
  }

  if (input->key_states[SAPP_KEYCODE_D].held) {
    camera->move(0, speed, 0);
  }
}


}