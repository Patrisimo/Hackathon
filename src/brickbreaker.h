#pragma once
  
typedef struct Ball {
  int dimx;
  int dimy;
  double x;
  double y;
  int dx;
  int dy;
} Ball;

struct BrickListNode;

typedef struct BrickList {
  int length;
  struct BrickListNode *head;
} BrickList;
  
double max(double a, double b);
double min(double a, double b);

int has_struck(Ball target, Ball mover);
BrickList *make_bricklist();
BrickList *bricklist_add(BrickList *el, Ball *ball); 
BrickList *bricklist_remove(BrickList *list, Ball *remove);
void bounce(Ball ball, BrickList *bricklist, Ball *brick);
Ball *make_brick(int dimx, int dimy, double x, double y);
void check_bricks(BrickList *bricklist, Ball ball);
void draw_bricks(BrickList *list, GContext *ctx);