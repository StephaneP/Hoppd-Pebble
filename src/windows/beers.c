#include <pebble.h>
#include "beers.h"
#include "../common.h"

static Beer beers[50];

static int num_beers = 0;
static bool no_beers = 0;
static bool out_failed = false;

static void refresh();
static void request_data();

static Window *window;
static MenuLayer *menu_layer;

//--- menu layer callbacks ---//

static uint16_t menu_get_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context);
static uint16_t menu_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static int16_t menu_get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static int16_t menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context);
static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context);
static void menu_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void menu_select_long_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

//--- end menu layer callbacks ---//

void beers_init(void){
	window = window_create();

	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	menu_layer = menu_layer_create(bounds);
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.get_header_height = menu_get_header_height_callback,
		.get_cell_height = menu_get_cell_height_callback,
		.draw_header = menu_draw_header_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
		.select_long_click = menu_select_long_callback,
	});

	menu_layer_set_click_config_onto_window(menu_layer, window);
	layer_add_child(window_layer, menu_layer_get_layer(menu_layer));

	window_stack_push(window, true);
	request_data();
}

void beers_destroy(void){
	menu_layer_destroy(menu_layer);
	window_destroy(window);
}

static void refresh(){
	memset(beers, 0x0, sizeof(beers));
	num_beers = 0;
	no_beers = false;
	menu_layer_set_selected_index(menu_layer, (MenuIndex){ .row = 0, .section = 0}, MenuRowAlignBottom, false);
	menu_layer_reload_data(menu_layer);
	layer_mark_dirty(menu_layer_get_layer(menu_layer));
	request_data();
}

static void request_data(){
	Tuplet beers_tuplet = TupletInteger(KEY_TYPE, KEY_TYPE_BEER);

	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	if (iter == NULL){
		return;
	}

	dict_write_tuplet(iter, &beers_tuplet);
	dict_write_end(iter);
	app_message_outbox_send();
}

//-- AppMessage --//

void beers_in_received_handler(DictionaryIterator *iter){
	Tuple *index_tuple = dict_find(iter, KEY_INDEX);
	Tuple *name_tuple = dict_find(iter, KEY_NAME);

	if (index_tuple && name_tuple){		
		if (index_tuple->value->int16 == 0){
			num_beers = 0;
			no_beers = false;
		}

		Beer beer;
		beer.beerId = index_tuple->value->int16;
		strncpy(beer.name, name_tuple->value->cstring, sizeof(beer.name));
		beers[beer.beerId] = beer;
		num_beers++;			
		menu_layer_reload_data(menu_layer);
		layer_mark_dirty(menu_layer_get_layer(menu_layer));		
	} else if(index_tuple) {
		no_beers = true;
		menu_layer_reload_data(menu_layer);
		layer_mark_dirty(menu_layer_get_layer(menu_layer));		
	}
}

void beers_out_sent_handler(DictionaryIterator *iter){

}

void beers_out_failed_handler(DictionaryIterator *failed, AppMessageResult reason){

}

//-- Menu callbacks implementation ---//
static uint16_t menu_get_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context){
	return 1;
}

static uint16_t menu_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context){
	return (num_beers) ? num_beers : 1;
}

static int16_t menu_get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context){
	return MENU_HEADER_HEIGHT;
}

static int16_t menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context){
	return MENU_CELL_HEIGHT;
}

static void menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context){
	menu_cell_basic_header_draw(ctx, cell_layer, "Beers");
}

static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context){
	graphics_context_set_text_color(ctx, GColorBlack);

	GRect origin = { 
					.origin = {4 , 4},
					.size = { PEBBLE_WIDTH - 8, 22}
				   };

	if (num_beers == 0){
		graphics_draw_text(ctx, "Loading beers...", fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), origin, GTextOverflowModeFill, GTextAlignmentLeft, NULL);
	} else if (no_beers) {
		graphics_draw_text(ctx, "No beers found...", fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), origin, GTextOverflowModeFill, GTextAlignmentLeft, NULL);
	} else {
		menu_cell_basic_draw(ctx, cell_layer, beers[cell_index->row].name, NULL, NULL);
	}
}

static void menu_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context){

}

static void menu_select_long_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context){
	refresh();
}