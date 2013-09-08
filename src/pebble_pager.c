#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define PEBBLE_PAGER_MESSAGE 0xA1

#define MY_UUID { 0x38, 0x9D, 0x91, 0xC5, 0xF8, 0x4C, 0x4F, 0xC1, 0xA1, 0xA0, 0xBA, 0xA8, 0xEA, 0x1B, 0x43, 0x6F }
PBL_APP_INFO(MY_UUID,
             "Pebble Pager", "Pebble Pager",
             1, 0, /* App version */
             RESOURCE_ID_PEBBLE_PAGER_ICON_BLACK,
             APP_INFO_STANDARD_APP);

Window window;
TextLayer text_layer;
TextLayer text_layer2;
Layer background_layer;
BitmapLayer arrow_layer;
GBitmap arrow_bitmap;
uint8_t buf[10240];
int started;

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  DictionaryIterator *iter;

  if (app_message_out_get(&iter) != APP_MSG_OK) {
    return;
  }
  if (dict_write_uint8(iter, PEBBLE_PAGER_MESSAGE, 0) != DICT_OK) {
    return;
  }
  app_message_out_send();
  app_message_out_release();
  if (started == 0) {
    text_layer_set_text(&text_layer2, "to stop");
    started = 1;
  } else {
    text_layer_set_text(&text_layer2, "to page");
    started = 0;
  }
}

void config_provider(ClickConfig **config, Window *window) {
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
  config[BUTTON_ID_SELECT]->click.repeat_interval_ms = 100; // "hold-to-repeat" gets overridden if there's a long click handler configured!

  (void)window;
}

void background_layer_draw(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer->bounds, 0, GCornerNone);
}

void handle_init(AppContextRef ctx) {
  started = 0;
  resource_init_current_app(&VERSION);
  window_init(&window, "Pebble Pager");
  window_stack_push(&window, true /* Animated */);

  layer_init(&background_layer, GRect(0, 0, 144, 168));
  background_layer.update_proc = background_layer_draw;
  layer_add_child(&window.layer, &background_layer);

  resource_load(resource_get_handle(RESOURCE_ID_PEBBLE_PAGER_ARROW_BLACK), buf, 10240);
  gbitmap_init_with_data(&arrow_bitmap, buf);
  bitmap_layer_init(&arrow_layer, GRect(110, 60, 24, 24));
  bitmap_layer_set_bitmap(&arrow_layer, &arrow_bitmap);
  bitmap_layer_set_background_color(&arrow_layer, GColorWhite);
  layer_add_child(&background_layer, &arrow_layer.layer);

  text_layer_init(&text_layer, GRect(0, 24, 144, 30));
  text_layer_set_text_color(&text_layer, GColorWhite);
  text_layer_set_background_color(&text_layer, GColorBlack);
  text_layer_set_text_alignment(&text_layer, GTextAlignmentCenter);
  text_layer_set_text(&text_layer, "Click select");
  text_layer_set_font(&text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(&background_layer, &text_layer.layer);

  text_layer_init(&text_layer2, GRect(0, 90, 144, 30));
  text_layer_set_text_color(&text_layer2, GColorWhite);
  text_layer_set_background_color(&text_layer2, GColorBlack);
  text_layer_set_text_alignment(&text_layer2, GTextAlignmentCenter);
  text_layer_set_text(&text_layer2, "to page phone");
  text_layer_set_font(&text_layer2, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(&background_layer, &text_layer2.layer);


  window_set_click_config_provider(&window, (ClickConfigProvider) config_provider);
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered
}
void out_fail_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  // outgoing message failed
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .messaging_info = {
      .buffer_sizes = {
        .inbound = 64,
        .outbound = 16,
      },
      .default_callbacks.callbacks = {
        .out_sent = out_sent_handler,
        .out_failed = out_fail_handler,
      },
    },
  };
  app_event_loop(params, &handlers);
}

