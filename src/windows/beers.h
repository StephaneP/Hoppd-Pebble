#pragma once

void beers_init(void);
void beers_destroy(void);

void beers_in_received_handler(DictionaryIterator *iter);
void beers_out_sent_handler(DictionaryIterator *iter);
void beers_out_failed_handler(DictionaryIterator *failed, AppMessageResult reason);
