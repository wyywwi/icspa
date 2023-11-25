#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  int getkeycode = inl(KBD_ADDR);
  kbd->keydown = (getkeycode != kbd->keycode);
  kbd->keycode = getkeycode;
}
