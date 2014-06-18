#include <pebble.h>
#include "appmessage.h"
#include "common.h"
#include "windows/beers.h"
#include "windows/breweries.h"
#include "windows/styles.h"

#define LOG(...) app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

static void in_received_handler(DictionaryIterator *iter, void *context);
static void in_dropped_handler(AppMessageResult reason, void *context);
static void out_sent_handler(DictionaryIterator *sent, void *context);
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);

void appmessage_init(void){
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
};

static void in_received_handler(DictionaryIterator *iter, void *context){
	Tuple *type = dict_find(iter, KEY_TYPE);
	switch(type->value->uint8){
		case KEY_TYPE_BEER:
			beers_in_received_handler(iter);
			break;
		case KEY_TYPE_BREWERY:
			breweries_in_received_handler(iter);
			break;
		case KEY_TYPE_STYLE:
			styles_in_received_handler(iter);
			break;
	}
}

static void in_dropped_handler(AppMessageResult reason, void *context){
}

static void out_sent_handler(DictionaryIterator *sent, void *context){

}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context){

}
