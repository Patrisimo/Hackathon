#include "brickbreaker.h"

double max(double a, double b) {
  return a > b ? a : b;
}
double min(double a, double b) {
  return a < b ? a : b;
}
double min_nonneg(double a, double b) {
  if (b < 0)
    return a;
  else
    return a < b ? a : b;
}

typedef struct BrickListNode {
  Ball *data;
  struct BrickListNode *next;
} BrickListNode;


int has_struck(Ball target, Ball mover) {
  // Want nonzero intersection
  // (x,y) \in target iff (t.x - t.dimx/2) < x < (t.x + dimx/2) and (t.y - dimy/2) < y < (t.y + dimy/2)
  // (x,y) \in mover  iff (m.x - dimx/2) < x < (m.x + dimx.2) and (m.y - dimy/2) < y < (m.y + dimy/2)
  // Need max()
  double target_minx, target_miny, target_maxx, target_maxy;
  double mover_minx, mover_miny, mover_maxx, mover_maxy;
  target_minx = target.x;
  target_maxx = target.x + target.dimx;
  target_miny = target.y;
  target_maxy = target.y + target.dimy;
  
  mover_minx = mover.x;
  mover_maxx = mover.x + mover.dimx;
  mover_miny = mover.y;
  mover_maxy = mover.y + mover.dimy;
  
  // Return a positive integer for the side we're hitting on
  /*      2
   *      _
   *   1 |_| 3
   *      4
   */
  
  if ( (mover_miny <= target_maxy) && (mover_maxy >= target_miny) && (mover_maxx >= target_minx) && (mover_minx <= target_maxx)) {
    if (mover.y + mover.dimy/2.0 >= target.y + target.dimy && mover.dy < 0)
      return 2;
    else if (mover.y + mover.dimy/2.0 <= target.y && mover.dy > 0)
      return 4;
    else if (mover.x + mover.dimx/2.0 >= target.x + target.dimx && mover.dx < 0)
      return 3;
    else if (mover.x + mover.dimx/2.0 <= target.x && mover.dx > 0)
      return 1;
    else
      return 0;
  } else 
    return 0;  
}


BrickList* make_bricklist() {
  BrickList *list = malloc(sizeof(BrickList));
  list->head = NULL;
  list->length = 0;
  return list;
}

BrickList* bricklist_add(BrickList *list, Ball *ball) {
   
  BrickListNode* new_el = malloc(sizeof(BrickList));
  new_el->data = ball;
  new_el->next = list->head;
  list->head = new_el;
  list->length++;
  
  return list;
}

BrickList *bricklist_remove(BrickList *list, Ball *remove) {
  BrickListNode *curr = list->head, *temp;
  if (curr == NULL)
    return list;
  if (curr->data == remove) {
    temp = curr;
    list->head = curr->next;
    list->length--;
    free(curr->data);
    free(curr);
    return list;
  }
  while ( curr->next != NULL ) {
    if (curr->next->data == remove) {
      temp = curr->next;
      curr->next = curr->next->next;
      free(temp->data);
      free(temp);
      return list;
    }
    curr = curr->next;
  }
  return list;
}

Ball *make_brick(int dimx, int dimy, double x, double y) {
  Ball *b = malloc(sizeof(Ball));
  b->dimx = dimx;
  b->dimy = dimy;
  b->x = x;
  b->y = y;
  b->dx = 0;
  b->dy = 0;
  return b;
}

int bounce(Ball *ball, BrickList *bricklist, Ball *brick) {
  int coll_res = has_struck(*brick, *ball);
  if (coll_res) {
    if (coll_res % 2)
      ball->dx = -1 * ball->dx;
    else
      ball->dy = -1 * ball->dy;
    bricklist_remove(bricklist, brick);
    return 1;
  }
  else
    return 0;
}

double check_bricks(BrickList *list, Ball *ball) {
  BrickListNode *curr, *temp;
  double min_time, time;
  curr = list->head;
  min_time = -1;
  while (curr != NULL) {
    temp = curr->next;
    time = time2impact(*ball, *(curr->data));
    if ( (min_time <0) || ( time > 0 && time < min_time ) ) {
      min_time = time;
    }
    bounce(ball, list, curr->data);
    curr = temp;
  }
  return min_time;
}

GRect draw_rectangle(Ball *brick) {
  return GRect(brick->x, brick->y, 
                                  brick->dimx, brick->dimy);
}
void draw_bricks(BrickList *list, GContext *ctx) {
  BrickListNode *curr = list->head;
  while (curr != NULL) {
    graphics_fill_rect(ctx, draw_rectangle(curr->data), 0, GCornerNone);
    curr = curr->next;
  }
}
  
double time2impact(Ball ball, Ball brick) {
  // Can reduce case to two sides
//  int vert_size, horz_size;
  double v_time, h_time, time, next;
  Point   b_vert, /* base point of the vertical face of the ball */ 
          b_horz, /* base point of the horizontal face of the ball */
          vert_face, /* base point of the vertical face of the brick */
          horz_face; /* base point of the horizontal face of the brick */
  
  time = -1; // Initialize to false
  v_time = -1;
  h_time = -1;
  
  if (ball.dx > 0) { // Heading right
    vert_face.x = brick.x;
    b_vert.x = ball.x + ball.dimx;
  } else {
    vert_face.x = brick.x + brick.dimx;
    b_vert.x = ball.x;
  }
  vert_face.y = brick.y;
  b_vert.y = ball.y;
  
  if (ball.dy > 0) { // Heading down
    horz_face.y = brick.y;
    b_horz.y = ball.y + ball.dimy;
  } else {
    horz_face.y = brick.y + brick.dimy;
    b_horz.y = ball.y;
  }
  horz_face.x = brick.x;
  b_horz.x = ball.x;
  
  // Find the time to intersect the vertical face
  // i.e., when the ball's x coordinate is equal to the face's x coordinate, and
  // when the faces overlap
  if ( ball.dx != 0 ) {
    v_time = (vert_face.x - b_vert.x) / ball.dx;
  } 
  // Check for actual hit if intersection happens in the future
  if (v_time > 0) { // Only want future hits
    next = b_vert.y + (v_time * ball.dy);
    if ( (next <= vert_face.y + brick.dimy) && (next + ball.dimy >= vert_face.y) ) {
      time = v_time; // Set time
    }
  }
  
  // Find the time to intersect the horizontal face
  // i.e., when the ball's y coordinate is equal to the face's y coordinate, and
  // when the faces overlap
  if ( ball.dy != 0 ) {
    h_time = (horz_face.y - b_horz.y) / ball.dy;
  } 
  // Check for actual hit if intersection happens in the future
  if (h_time > 0 && h_time < time) { // Only want future, and sooner, hits
    next = b_horz.x + (h_time * ball.dx);
    if ( (next <= horz_face.x + brick.dimx) && (next + ball.dimx >= horz_face.x) ) {
      time = h_time; // Set time
    }
  }
  
  
  return time;
}