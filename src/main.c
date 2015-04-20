#include <pebble.h>
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_dateday_layer;
static GFont s_time_font;
static GFont s_date_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // get a tm struct
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  //create a long-lived buffer
  static char buffer[] = "00:00";
  
  // write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  // display time on textlayer
  text_layer_set_text(s_time_layer, buffer);
}

static void update_date() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  static char buffer[] = "Wednesday, 12/31/9999";
  strftime(buffer, sizeof("Wednesday, 12/31/9999"), "%A, %m/%d/%G", tick_time);
  
  text_layer_set_text(s_dateday_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  update_date();
}

static void main_window_load(Window *window) {
  //choose font
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CONSOLAS_44));
  s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  
  //creat GBitmap - set to created BitmaPlayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BLUE_FACE);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // time textlayer
  s_time_layer = text_layer_create(GRect(3, 46, 139, 50));
  s_dateday_layer = text_layer_create(GRect(0, 96, 144, 30));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_background_color(s_dateday_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorCeleste);
  text_layer_set_text_color(s_dateday_layer, GColorPastelYellow);
  
  // improve layout to be more like watchface
  text_layer_set_font(s_time_layer, s_time_font);   //applies chosen font to textlayer
  text_layer_set_font(s_dateday_layer, s_date_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_dateday_layer, GTextAlignmentCenter);

  //add as child layer to window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_dateday_layer));  
}

static void main_window_unload(Window *window){
  //destroy textlayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_dateday_layer);
  fonts_unload_custom_font(s_time_font);
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
}

static void init() {
  // create main window element, assign to pointer
  s_main_window = window_create();
  
  //set handlers to manage elements in window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  //Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // show window on watch - animated=true
  window_stack_push(s_main_window, true);
}

static void deinit() {
  // destroy window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
