#pragma once

void breweries_init(void);
void breweries_destroy(void);

void breweries_in_received_handler(DictionaryIterator *iter);
void breweries_out_sent_handler(DictionaryIterator *iter);
void breweries_out_failed_handler(DictionaryIterator *failed, AppMessageResult reason);
