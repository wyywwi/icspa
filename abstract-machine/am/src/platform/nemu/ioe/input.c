#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t keycode = inl(KBD_ADDR);
  if((keycode & KEYDOWN_MASK) != 0){
    kbd->keydown = true;
    kbd->keycode = AM_KEY_NONE;
  }
  else{
    kbd->keydown = false;
    kbd->keycode = keycode & KEYDOWN_MASK;
  }
  // kbd->keydown = (keycode != KEYDOWN_MASK);
  // kbd->keycode = ((keycode == KEYDOWN_MASK)?AM_KEY_NONE:keycode);
}
