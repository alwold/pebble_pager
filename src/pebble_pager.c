#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x38, 0x9D, 0x91, 0xC5, 0xF8, 0x4C, 0x4F, 0xC1, 0xA1, 0xA0, 0xBA, 0xA8, 0xEA, 0x1B, 0x43, 0x6F }
PBL_APP_INFO(MY_UUID,
             "Pebble Pager", "Pebble Pager",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;
TextLayer hello_layer;

void handle_init(AppContextRef ctx) {
  window_init(&window, "Pebble Pager");
  window_stack_push(&window, true /* Animated */);

  text_layer_init(&hello_layer, GRect(0, 65, 144, 30));
  text_layer_set_text_alignment(&hello_layer, GTextAlignmentCenter);
  text_layer_set_text(&hello_layer, "Press select to page!");
  text_layer_set_font(&hello_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(&window.layer, &hello_layer.layer);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {

}

void config_provider(ClickConfig **config, Window *window) {
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
  config[BUTTON_ID_SELECT]->click.repeat_interval_ms = 1000; // "hold-to-repeat" gets overridden if there's a long click handler configured!

  (void)window;
}

