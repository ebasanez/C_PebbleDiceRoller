#include <pebble.h>
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

static Window *window;


int selected = 0;
int suma = 0;
int ultimo = 0;
int repeticiones[] = {0,0,0,0,0,0};
static int dices[] = {4,6,8,10,12,20};
static TextLayer *total_layer;
static TextLayer *dice_layers[sizeof(dices)];
static char dice_chars[sizeof(dices)][20];
int num = sizeof(dices)/sizeof(dices[0]);

static void draw_results(){
  static char total_text[20];
  snprintf(total_text, 20,"Total = %d (%d)", suma, ultimo);
  text_layer_set_text(total_layer, total_text);
  for(int i = 0; i < num; i++){
    snprintf(dice_chars[i],20,"%d x d %d",repeticiones[i], dices[i]);
    text_layer_set_text(dice_layers[i],dice_chars[i]);
    if(selected == i){
      text_layer_set_background_color(dice_layers[i],GColorBlack);
      text_layer_set_text_color(dice_layers[i],GColorWhite);
    }else{
      text_layer_set_background_color(dice_layers[i],GColorWhite);
      text_layer_set_text_color(dice_layers[i],GColorBlack);
    }
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  repeticiones[selected]++;
  ultimo = (rand() % dices[selected]) + 1;
  suma = suma + ultimo;
  draw_results();
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  suma = 0;
  ultimo = 0;
  for(int i = 0; i < num; i++){
    repeticiones[i]=0;
  }
  draw_results();
  vibes_short_pulse();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  selected =  MAX(0,selected - 1);
   draw_results();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  selected =  MIN(5,selected + 1);
   draw_results();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 1000, select_long_click_handler,select_long_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Generamos los elementos de la pantalla inicial:
  total_layer = text_layer_create(GRect(0,0,bounds.size.w, 20));
  text_layer_set_text_alignment(total_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(total_layer));
    
  for(int i = 0; i < num; i++){
    TextLayer *dado_layer = text_layer_create(GRect(0,20*(i+1),bounds.size.w, 20));
    dice_layers[i] = dado_layer;
    text_layer_set_text_alignment(dice_layers[i], GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(dice_layers[i]));
  }  
  draw_results();
}


static void window_unload(Window *window) {
  text_layer_destroy(total_layer);
  int i;
  for(i = 0; i < num; i++){
    text_layer_destroy(dice_layers[i]);
  }
}

static void init(void) {
  srand(time(NULL));
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
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
  app_event_loop();
  deinit();
}