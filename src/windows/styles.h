#pragma once

void styles_init(void);
void styles_destroy(void);

void styles_in_received_handler(DictionaryIterator *iter);
void styles_out_sent_handler(DictionaryIterator *iter);
void styles_out_failed_handler(DictionaryIterator *failed, AppMessageResult reason);
