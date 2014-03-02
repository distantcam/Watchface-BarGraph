#include <pebble.h>

Window *window;

GBitmap *faceImage;
BitmapLayer *faceLayer;

Layer *hourLayer;
Layer *minLayer;

TextLayer *dateLayer;

void update_hour(Layer *me, GContext* ctx) {
	time_t now = time(NULL);
  	struct tm *t = localtime(&now);

  	int height = t->tm_hour;

  	if (height > 12)
    	height -= 12;

  	if (height == 0)
    	height = 12;

  	graphics_context_set_fill_color(ctx, GColorWhite);
  	graphics_context_set_stroke_color(ctx, GColorBlack);

  	graphics_fill_rect(ctx, GRect(38, 132 - height * 10, 30, height * 10), 0, GCornerNone);

  	for (int i = 0; i < height-1; i++)
	{
    	graphics_draw_line(ctx, GPoint(38, 122 - i*10), GPoint(67, 122 - i*10));
	}
}

void update_minute(Layer *me, GContext* ctx) {
	time_t now = time(NULL);
  	struct tm *t = localtime(&now);

    int height = t->tm_min * 2;

    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_context_set_stroke_color(ctx, GColorBlack);

    graphics_fill_rect(ctx, GRect(76, 132 - height, 30, height), 0, GCornerNone);

    for (int i = 10; i < height; i+=10)
    {
      graphics_draw_line(ctx, GPoint(76, 132 - i), GPoint(105, 132 - i));
    }
}

void update_date()
{
	time_t now = time(NULL);
  	struct tm *t = localtime(&now);
	
	static char dateText[] = "xxx, xxx 00";
	
	strftime(dateText, sizeof(dateText), "%a, %b %e", t);
	text_layer_set_text(dateLayer, dateText);
}

void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	if ((units_changed & HOUR_UNIT) != 0)
		layer_mark_dirty(hourLayer);
	if ((units_changed & MINUTE_UNIT) != 0)
		layer_mark_dirty(minLayer);
	if ((units_changed & DAY_UNIT) != 0)
		update_date();
}

void init(void) {
	// Set up window
	window = window_create();
	window_set_background_color(window, GColorBlack);
  	window_stack_push(window, true); // Animated
	
	Layer *windowLayer = window_get_root_layer(window);
	GRect windowFrame = layer_get_frame(windowLayer);
	
	// Set up face
	faceImage = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FACE);
	faceLayer = bitmap_layer_create(windowFrame);
	
	bitmap_layer_set_bitmap(faceLayer, faceImage);
	layer_add_child(windowLayer, bitmap_layer_get_layer(faceLayer));
	
	// Set up hour
	hourLayer = layer_create(windowFrame);
	layer_set_update_proc(hourLayer, update_hour);
  	layer_add_child(windowLayer, hourLayer);
	
	// Set up minute
	minLayer = layer_create(windowFrame);
	layer_set_update_proc(minLayer, update_minute);
  	layer_add_child(windowLayer, minLayer);
	
	// Set up date text
	dateLayer = text_layer_create(GRect(0,138,144,30));
	text_layer_set_text_color(dateLayer, GColorWhite);
  	text_layer_set_background_color(dateLayer, GColorClear);
  	text_layer_set_text_alignment(dateLayer, GTextAlignmentCenter);
  	text_layer_set_font(dateLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARIAL_22)));
  	layer_add_child(windowLayer, text_layer_get_layer(dateLayer));
	
	update_date();
	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

void deinit(void) {
	layer_remove_from_parent(bitmap_layer_get_layer(faceLayer));
  	bitmap_layer_destroy(faceLayer);
  	gbitmap_destroy(faceImage);
	
	layer_destroy(hourLayer);
	layer_destroy(minLayer);
	
	text_layer_destroy(dateLayer);
	
	window_destroy(window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
