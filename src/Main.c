/*
 * main.c
 * Creates a Window, InverterLayer and animates it around the screen 
 * from corner to corner. It uses the `.stopped` callback to schedule the 
 * next stage of the animation.
 */

#include <pebble.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "brickbreaker.h"
  
#define BOX_SIZE 5
#define TITLE_BAR_SIZE 16

#define ANIM_DURATION 1
#define ANIM_DELAY 0

#define UP 0
#define DOWN 1

#define SCREEN_WIDTH
#define SCREEN_HEIGHT

static Window *s_main_window;
static InverterLayer *s_box_layer;
static Layer *brick_layer;
static PropertyAnimation *s_box_animation;



// Function prototype 
static void next_animation();

static void anim_stopped_handler(Animation *animation, bool finished, void *context) {
  // Free the animation
  property_animation_destroy(s_box_animation);

  // Schedule the next one, unless the app is exiting
  if (finished) {
    next_animation();
  }
}

double speed = 5;

Ball gBall;
Ball paddle;
Ball top, left, right, bottom;
BrickList *bricklist;
int __errno;




static void next_animation() {
  // Determine start and finish positions
  GRect start, finish;
  int coll_res;
  double time;
  double endX,endY;
  int dx, dy;
  
  start = GRect(gBall.x, gBall.y, gBall.dimx, gBall.dimy);
  dx = gBall.dx;
  dy = gBall.dy;
  
  
  // Check for collision with paddle
  coll_res = has_struck(paddle, gBall);
  if (coll_res) {
    if (coll_res % 2)
      gBall.dx = -1 * gBall.dx;
    else
      gBall.dy = -1 * gBall.dy;
  }
  
  // Check for collision with bricks, find next collision time
  time = check_bricks(bricklist, &gBall);
  time = min_nonneg(time, time2impact(top, gBall));
  time = min_nonneg(time, time2impact(left, gBall));
  time = min_nonneg(time, time2impact(right, gBall));
  time = min_nonneg(time, time2impact(bottom, gBall));
  
  app_log(APP_LOG_LEVEL_INFO, "Main.c", 87, "Time to next bounce: %f\n", time);
  time = time + 2;
  
  endX =  (gBall.x + time * dx / sqrt( dx * dx + (dy * dy)) );
  endY =  (gBall.y + time * dy / sqrt( dx * dx + (dy * dy)) );
  
  if (endX < 0 && gBall.dx < 0) {
    gBall.dx = -1 * gBall.dx;
  }
  if (endX > 144 - gBall.dimx && gBall.dx > 0) {
    gBall.dx = -1 * gBall.dx;
  }
  if (endY < 0 && gBall.dy < 0) {
    gBall.dy = -1 * gBall.dy;
  }
  if (endY > 168 - TITLE_BAR_SIZE - gBall.dimy && gBall.dy > 0) {
    gBall.dy = -1 * gBall.dy;
  }
  
  finish  = GRect((int) endX, (int) endY, gBall.dimx, gBall.dimy);
  gBall.x = endX;
  gBall.y = endY;
  
  
  
  // Schedule the next animation
  s_box_animation = property_animation_create_layer_frame(inverter_layer_get_layer(s_box_layer), &start, &finish);
  animation_set_duration((Animation*)s_box_animation, (int) time + 1);
  animation_set_delay((Animation*)s_box_animation, ANIM_DELAY);
  animation_set_curve((Animation*)s_box_animation, AnimationCurveEaseInOut);
  animation_set_handlers((Animation*)s_box_animation, (AnimationHandlers) {
    .stopped = anim_stopped_handler
  }, NULL);
  animation_schedule((Animation*)s_box_animation);

  // Increment stage and wrap
  
}




static void brick_update_proc(Layer *this_layer, GContext *ctx) {
  
  
  graphics_fill_rect(ctx, GRect(paddle.x, paddle.y, paddle.dimx, paddle.dimy), 0, GCornerNone);
  
  draw_bricks(bricklist, ctx);
  
  
  
}


void setup_game() {
  // Maybe 6 rows, 12 columns
  int i,j;
  if (bricklist == NULL)
    bricklist = make_bricklist();
  
  for (i=0;i<6;i++) {
    for (j=0;j<9;j++) {
      bricklist = bricklist_add(bricklist, make_brick(12, 4, 10 + 14 * j, 5 + 6 * i));
    }
  }
  
  gBall = (Ball) {5, 5, 40.0, 43.0, 2, 5};
  paddle = (Ball) {40, 5,  60, 140, 0, 0};
  
  top = (Ball) {144, 0, 0, 0, 0, 0};
  bottom = (Ball) {144, 0, 0, 168 - TITLE_BAR_SIZE, 0, 0};
  left = (Ball) {0, 168 - TITLE_BAR_SIZE, 0, 0, 0, 0};
  right = (Ball) {0, 168 - TITLE_BAR_SIZE, 144, 0, 0, 0};

  
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Create InverterLayer
  s_box_layer = inverter_layer_create(GRect(0, 0, BOX_SIZE, BOX_SIZE));
  layer_add_child(window_layer, inverter_layer_get_layer(s_box_layer));
  
  // Create BrickLayer
  brick_layer = layer_create(GRect(0,0,window_bounds.size.w, window_bounds.size.h));
  layer_add_child(window_layer, brick_layer);
  layer_set_update_proc(brick_layer, brick_update_proc);
  
  setup_game();
  
}

static void main_window_unload(Window *window) {
  // Destroy InverterLayer
  inverter_layer_destroy(s_box_layer);
}


static void init(void) {
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  
  

  // Start animation loop
  next_animation();
}

static void deinit(void) {
  // Stop any animation in progress
  animation_unschedule_all();

  // Destroy main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}