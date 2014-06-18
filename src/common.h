#pragma once

#define MENU_HEADER_HEIGHT 15
#define MENU_CELL_HEIGHT 44

#define PEBBLE_WIDTH 144
#define PEBBLE_HEIGHT 152

typedef struct {
	int beerId;
	char name[32];
	char abv[32];
	char ibu[32];
} Beer;

typedef struct {
	int breweryId;
	char name[32];
} Brewery;

typedef struct {
	int styleId;
	char name[32];
} Style;

enum {
	KEY_TYPE,
	KEY_INDEX,
	KEY_NAME,
	KEY_LOADED
};

enum {
	KEY_TYPE_BEER,
	KEY_TYPE_BREWERY,
	KEY_TYPE_STYLE
};

