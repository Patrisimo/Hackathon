#include <pebble.h>
#include "brickbreaker.h"

double max(double a, double b) {
  return a > b ? a : b;
}
double min(double a, double b) {
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
    free(curr->data);
    free(curr);
    list->head = NULL;
    list->length = 0;
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

void bounce(Ball ball, BrickList *bricklist, Ball *brick) {
  int coll_res = has_struck(*brick, ball);
  if (coll_res) {
    if (coll_res % 2)
      ball.dx = -1 * ball.dx;
    else
      ball.dy = -1 * ball.dy;
    bricklist_remove(bricklist, brick);
  }
}

void check_bricks(BrickList *list, Ball ball) {
  BrickListNode *curr, *temp;
  curr = list->head;
  while (curr != NULL) {
    temp = curr->next;
    bounce(ball, list, curr->data);    
    curr = temp;
  }
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
  
  