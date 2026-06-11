#ifndef SNAKE_H
#define SNAKE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>
#include <windows.h>

/* ========== 游戏常量 ========== */
#define MAP_WIDTH   40      // 地图宽度
#define MAP_HEIGHT  20      // 地图高度
#define INIT_LENGTH 3       // 蛇初始长度
#define MAX_SNAKE   800     // 蛇最大长度 (MAP_WIDTH * MAP_HEIGHT)

/* 方向枚举 */
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

/* 坐标结构 */
typedef struct {
    int x;
    int y;
} Point;

/* 蛇结构 */
typedef struct {
    Point body[MAX_SNAKE];  // 蛇身坐标数组
    int length;             // 当前长度
    Direction dir;          // 当前移动方向
} Snake;

/* 游戏状态 */
typedef struct {
    Snake snake;
    Point food;
    int score;
    int speed;              // 延迟毫秒数，越小越快
    bool game_over;
    bool paused;
} GameState;

/* ========== 函数声明 ========== */

/* 控制台操作 */
void gotoxy(int x, int y);
void hide_cursor(void);
void set_console_color(int color);

/* 游戏核心 */
void init_game(GameState *gs);
void generate_food(GameState *gs);
void update_game(GameState *gs);
void draw_game(const GameState *gs);
void draw_ui(const GameState *gs);
void handle_input(GameState *gs);

#endif /* SNAKE_H */
