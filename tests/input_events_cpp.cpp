//{{{  includes
#include <MiniFB_cpp.h>

#include <stdio.h>
#include <stdint.h>
//}}}
#define WIDTH 800
#define HEIGHT 600
static unsigned int g_buffer[WIDTH * HEIGHT];

int main() {
  int noise, carry, seed = 0xbeef;

  struct mfb_window* window = mfb_open_ex ("Input Events CPP Test", WIDTH, HEIGHT, WF_RESIZABLE);
  if (!window)
    return 0;
  mfb_set_user_data (window, (void*)"Input Events CPP Test");

  // setup lambda callbacks
  mfb_set_active_callback ([](struct mfb_window* window, bool isActive) {
    fprintf (stdout, "%s > active: %d (lambda)\n", window ? (const char*) mfb_get_user_data(window) : "", isActive);
    } , window);
  mfb_set_resize_callback ([](struct mfb_window* window, int width, int height) {
    //{{{  lambda
    fprintf (stdout, "%s > resize: %d, %d\n", window ? (const char*) mfb_get_user_data(window) : "", width, height);

    uint32_t x = 0;
    if (width > WIDTH) {
      x = (width - WIDTH) >> 1;
      width = WIDTH;
      }

    uint32_t y = 0;
    if (height > HEIGHT) {
      y = (height - HEIGHT) >> 1;
      height = HEIGHT;
      }

    mfb_set_viewport (window, x, y, width, height);
    //}}}
    }, window);
  mfb_set_close_callback ([](struct mfb_window* window) {
    //{{{  lambda
    fprintf (stdout, "%s > close\n", window ? (const char*) mfb_get_user_data(window) : "");
    return true; // flase for don't close   
    //}}}
    }, window);

  // key
  mfb_set_keyboard_callback ([](struct mfb_window* window, mfb_key key, mfb_key_mod mod, bool isPressed) {
    //{{{  lambda
    fprintf (stdout, "%s > keyboard: key: %s (pressed: %d) [key_mod: %x]\n", window ? (const char*) mfb_get_user_data(window) : "", mfb_get_key_name(key), isPressed, mod);
    if (key == KB_KEY_ESCAPE)
      mfb_close (window);
    //}}}
    }, window);
  mfb_set_char_input_callback ([](struct mfb_window* window, unsigned int charCode) {
    fprintf (stdout, "%s > charCode: %d\n", window ? (const char*) mfb_get_user_data(window) : "", charCode);
    }, window);

  // mouse
  mfb_set_mouse_button_callback ([](struct mfb_window* window, mfb_mouse_button button, mfb_key_mod mod, bool isPressed) {
    //{{{  lambda
    fprintf (stdout, "%s > mouse_button: button: %d (pressed: %d) (at: %d, %d) [key_mod: %x]\n", 
             window ? (const char*) mfb_get_user_data(window) : "", button, isPressed, 
             mfb_get_mouse_x (window), mfb_get_mouse_y (window), mod);
    //}}}
    }, window);
  mfb_set_mouse_move_callback ([](struct mfb_window* window, int x, int y) {
    fprintf (stdout, "%s > mouse_move: %d, %d\n", window ? (const char*)mfb_get_user_data(window) : "", x, y);
    }, window);
  mfb_set_mouse_scroll_callback ([](struct mfb_window* window, mfb_key_mod mod, float deltaX, float deltaY) {
    //{{{  lambda
    fprintf (stdout, "%s > mouse_scroll: x: %f, y: %f [key_mod: %x]\n", 
             window ? (const char*) mfb_get_user_data(window) : "", deltaX, deltaY, mod);
    //}}}
    }, window);

  do {
    //{{{  generate frame buffer of noise
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
      noise = seed;
      noise >>= 3;
      noise ^= seed;
      carry = noise & 1;
      noise >>= 1;
      seed >>= 1;
      seed |= (carry << 30);
      noise &= 0xFF;
      g_buffer[i] = MFB_RGB(noise, noise, noise);
      }
    //}}}
    if (mfb_update (window, g_buffer) != STATE_OK) {
      window = 0x0;
      break;
      }
    } while (mfb_wait_sync (window));

  return 0;
  }
