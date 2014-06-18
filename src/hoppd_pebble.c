#include <pebble.h>
#include "appmessage.h"

#include "windows/beers.h"
#include "windows/breweries.h"
#include "windows/styles.h"

static Window *window;

static void open_beers(int menu_index, void *context);
static void open_breweries(int menu_index, void *context);
static void open_styles(int menu_index, void *context);

static SimpleMenuLayer *simple_menu_layer;
static SimpleMenuItem app_menu_items[] = {
  {
    .title = "Beers",
    .callback = open_beers
  },
  {
    .title = "Breweries",
    .callback = open_breweries
  },
  {
    .title = "Styles",
    .callback = open_styles
  }
};

static SimpleMenuItem about_menu_items[] = {
  {
    .title = "Version"
  },
  {
    .title = "Credits"
  }
};


static SimpleMenuSection app_menu_sections[] = {
  {
    .title = "Hoppd",
    .items = app_menu_items,
    .num_items = ARRAY_LENGTH(app_menu_items),
  },
  {
    .title = "About",
    .items = about_menu_items, 
    .num_items = ARRAY_LENGTH(about_menu_items)
  }
};

//------- Callbacks --------//

static void open_beers(int menu_index, void *context){
  beers_init();
}

static void open_breweries(int menu_index, void *context){
  breweries_init();
}

static void open_styles(int menu_index, void *context){
  styles_init();
}

//------- App --------//

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  simple_menu_layer = simple_menu_layer_create(bounds, window, app_menu_sections, ARRAY_LENGTH(app_menu_sections), NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(simple_menu_layer));
}

static void window_unload(Window *window) {
  simple_menu_layer_destroy(simple_menu_layer);
}

static void init(void) {
  appmessage_init();
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
