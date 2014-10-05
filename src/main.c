#include <pebble.h>
	
static Window *s_main_window;
static BitmapLayer *s_image_layer;
static GBitmap *s_play_bitmap;
static GBitmap *s_ciseaux_bitmap;
static GBitmap *s_feuille_bitmap;
static GBitmap *s_pierre_bitmap;

static int s_choix = 0;
static int s_showing = 0;

static void main_window_load(Window *window) {
	s_play_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PLAY);
	s_ciseaux_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CISEAUX);
	s_feuille_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FEUILLE);
	s_pierre_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PIERRE);
	s_image_layer = bitmap_layer_create(GRect(0, 0, 144, 148));
	bitmap_layer_set_bitmap(s_image_layer, s_play_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_image_layer));
}

void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	s_choix = 1;
}

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	s_choix = 2;
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	s_choix = 3;
}

void config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	s_showing++;
	if(s_showing>3){
		s_choix = 0;
		s_showing = 0;
		bitmap_layer_set_bitmap(s_image_layer, s_play_bitmap);
		tick_timer_service_unsubscribe();
	}
}

void accel_tap_handler(AccelAxisType axis, int32_t direction) {
	if(s_showing==0 && s_choix!=0){
		switch(s_choix){
			case 1:
				bitmap_layer_set_bitmap(s_image_layer, s_ciseaux_bitmap);
			break;
			case 2:
				bitmap_layer_set_bitmap(s_image_layer, s_feuille_bitmap);
			break;
			case 3:
				bitmap_layer_set_bitmap(s_image_layer, s_pierre_bitmap);
			break;
		}
		s_showing++;
		tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
	}
}

static void main_window_unload(Window *window) {
	bitmap_layer_destroy(s_image_layer);
	gbitmap_destroy(s_play_bitmap);
	gbitmap_destroy(s_ciseaux_bitmap);
	gbitmap_destroy(s_feuille_bitmap);
	gbitmap_destroy(s_pierre_bitmap);
}

static void init() {
	s_main_window = window_create();
	
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});
	
	window_stack_push(s_main_window, true);
	
	// Associate Click Config Provider 
	window_set_click_config_provider(s_main_window, config_provider);
	accel_tap_service_subscribe(accel_tap_handler);
}

static void deinit() {
	window_destroy(s_main_window);
	accel_tap_service_unsubscribe();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}