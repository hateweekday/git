/**
 * ============================================================
 *  控制台贪吃蛇 (Console Snake Game)
 *  平台: Windows 控制台
 *  编译: gcc main.c -o snake.exe -Wall
 *  操作: W/A/S/D 或 方向键 控制移动
 *        P 暂停/继续
 *        Q 退出游戏
 * ============================================================
 */

#include "snake.h"

/* ========== 控制台操作实现 ========== */

/* 移动控制台光标到指定位置 */
void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = (SHORT)x;
    coord.Y = (SHORT)y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

/* 隐藏光标闪烁 */
void hide_cursor(void)
{
    CONSOLE_CURSOR_INFO cursor_info;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(handle, &cursor_info);
    cursor_info.bVisible = FALSE;
    SetConsoleCursorInfo(handle, &cursor_info);
}

/* 设置控制台文字颜色 */
void set_console_color(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
}

/* ========== 游戏核心实现 ========== */

/* 初始化游戏状态 */
void init_game(GameState *gs)
{
    int i;

    /* 初始化蛇: 水平方向居中靠下 */
    gs->snake.length = INIT_LENGTH;
    gs->snake.dir = DIR_RIGHT;

    for (i = 0; i < INIT_LENGTH; i++) {
        gs->snake.body[i].x = MAP_WIDTH / 2 - i;
        gs->snake.body[i].y = MAP_HEIGHT / 2;
    }

    gs->score = 0;
    gs->speed = 150;        /* 初始延迟150ms */
    gs->game_over = false;
    gs->paused = false;

    /* 生成第一个食物 */
    generate_food(gs);
}

/* 随机生成食物 (确保不在蛇身上) */
void generate_food(GameState *gs)
{
    int i, attempts = 0;
    bool valid;

    do {
        valid = true;
        gs->food.x = rand() % MAP_WIDTH;
        gs->food.y = rand() % MAP_HEIGHT;

        for (i = 0; i < gs->snake.length; i++) {
            if (gs->snake.body[i].x == gs->food.x &&
                gs->snake.body[i].y == gs->food.y) {
                valid = false;
                break;
            }
        }
        attempts++;
        /* 防止死循环: 蛇占满地图时退出 */
        if (attempts > MAX_SNAKE * 10) break;
    } while (!valid);
}

/* 更新游戏逻辑 (每帧调用一次) */
void update_game(GameState *gs)
{
    int i;
    Point new_head;

    if (gs->game_over || gs->paused) return;

    /* 计算新头部位置 */
    new_head = gs->snake.body[0];
    switch (gs->snake.dir) {
        case DIR_UP:    new_head.y--; break;
        case DIR_DOWN:  new_head.y++; break;
        case DIR_LEFT:  new_head.x--; break;
        case DIR_RIGHT: new_head.x++; break;
    }

    /* 碰撞检测: 墙壁 */
    if (new_head.x < 0 || new_head.x >= MAP_WIDTH ||
        new_head.y < 0 || new_head.y >= MAP_HEIGHT) {
        gs->game_over = true;
        return;
    }

    /* 碰撞检测: 自身 (不检查尾部，因为尾部即将移走) */
    for (i = 0; i < gs->snake.length - 1; i++) {
        if (gs->snake.body[i].x == new_head.x &&
            gs->snake.body[i].y == new_head.y) {
            gs->game_over = true;
            return;
        }
    }

    /* 移动蛇: 身体后移一位 */
    for (i = gs->snake.length - 1; i > 0; i--) {
        gs->snake.body[i] = gs->snake.body[i - 1];
    }
    gs->snake.body[0] = new_head;

    /* 吃食物判定 */
    if (new_head.x == gs->food.x && new_head.y == gs->food.y) {
        gs->score += 10;
        /* 长度+1 (尾部多加一格) */
        if (gs->snake.length < MAX_SNAKE) {
            gs->snake.body[gs->snake.length] = gs->snake.body[gs->snake.length - 1];
            gs->snake.length++;
        }
        /* 加速: 每吃5个食物加速一次 */
        if (gs->score % 50 == 0 && gs->speed > 40) {
            gs->speed -= 15;
        }
        generate_food(gs);
    }
}

/* 绘制游戏画面 */
void draw_game(const GameState *gs)
{
    int x, y, i;
    bool is_snake;

    gotoxy(0, 0);

    /* 顶边框 */
    set_console_color(8);           /* 灰色边框 */
    printf("╔");
    for (x = 0; x < MAP_WIDTH; x++) printf("═");
    printf("╗\n");

    /* 地图区域 */
    for (y = 0; y < MAP_HEIGHT; y++) {
        set_console_color(8);
        printf("║");               /* 左边框 */

        for (x = 0; x < MAP_WIDTH; x++) {
            is_snake = false;
            /* 检查蛇身 */
            for (i = 0; i < gs->snake.length; i++) {
                if (gs->snake.body[i].x == x && gs->snake.body[i].y == y) {
                    is_snake = true;
                    if (i == 0) {
                        /* 蛇头 */
                        set_console_color(10);      /* 亮绿色 */
                        printf("●");
                    } else {
                        /* 蛇身: 越靠头越亮 */
                        if (i <= gs->snake.length / 3) {
                            set_console_color(10);  /* 亮绿 */
                            printf("○");
                        } else if (i <= gs->snake.length * 2 / 3) {
                            set_console_color(2);   /* 深绿 */
                            printf("○");
                        } else {
                            set_console_color(2);
                            printf("·");
                        }
                    }
                    break;
                }
            }

            if (!is_snake) {
                /* 食物 */
                if (gs->food.x == x && gs->food.y == y) {
                    set_console_color(12);          /* 红色食物 */
                    printf("♥");
                } else {
                    /* 空地 */
                    printf(" ");
                }
            }
        }

        set_console_color(8);
        printf("║\n");            /* 右边框 */
    }

    /* 底边框 */
    set_console_color(8);
    printf("╚");
    for (x = 0; x < MAP_WIDTH; x++) printf("═");
    printf("╝\n");

    set_console_color(7);           /* 恢复默认白色 */
}

/* 绘制UI信息 */
void draw_ui(const GameState *gs)
{
    int ui_y = MAP_HEIGHT + 2;

    gotoxy(0, ui_y);
    set_console_color(14);          /* 黄色 */
    printf("  ╔══════════════════════════════════════╗\n");
    gotoxy(0, ui_y + 1);
    printf("  ║  得分: %-5d                         ║\n", gs->score);
    gotoxy(0, ui_y + 2);
    printf("  ║  长度: %-5d    速度: %-3d ms         ║\n",
           gs->snake.length, gs->speed);
    gotoxy(0, ui_y + 3);
    printf("  ║  操作: W↑ S↓ A← D→  方向键均可     ║\n");
    gotoxy(0, ui_y + 4);
    printf("  ║  P = 暂停    Q = 退出              ║\n");
    gotoxy(0, ui_y + 5);
    printf("  ╚══════════════════════════════════════╝\n");

    if (gs->paused) {
        gotoxy(MAP_WIDTH / 2 - 3, MAP_HEIGHT / 2);
        set_console_color(14);
        printf("█ 暂停中 █");
        gotoxy(MAP_WIDTH / 2 - 4, MAP_HEIGHT / 2 + 1);
        printf("按 P 键继续");
    }

    if (gs->game_over) {
        gotoxy(MAP_WIDTH / 2 - 5, MAP_HEIGHT / 2 - 1);
        set_console_color(12);
        printf("╔══════════════╗");
        gotoxy(MAP_WIDTH / 2 - 5, MAP_HEIGHT / 2);
        printf("║  游 戏 结 束  ║");
        gotoxy(MAP_WIDTH / 2 - 5, MAP_HEIGHT / 2 + 1);
        printf("║  最终得分: %-3d║", gs->score);
        gotoxy(MAP_WIDTH / 2 - 5, MAP_HEIGHT / 2 + 2);
        printf("╚══════════════╝");
        gotoxy(MAP_WIDTH / 2 - 6, MAP_HEIGHT / 2 + 4);
        set_console_color(7);
        printf("按 R 重新开始  Q 退出");
    }

    set_console_color(7);
}

/* 处理键盘输入 */
void handle_input(GameState *gs)
{
    int ch;

    if (!_kbhit()) return;

    ch = _getch();

    /* 方向键处理 (双字节: 224 + 方向码) */
    if (ch == 224 || ch == 0) {
        ch = _getch();
        switch (ch) {
            case 72: /* 上 */  if (gs->snake.dir != DIR_DOWN)  gs->snake.dir = DIR_UP;    break;
            case 80: /* 下 */  if (gs->snake.dir != DIR_UP)    gs->snake.dir = DIR_DOWN;  break;
            case 75: /* 左 */  if (gs->snake.dir != DIR_RIGHT) gs->snake.dir = DIR_LEFT;  break;
            case 77: /* 右 */  if (gs->snake.dir != DIR_LEFT)  gs->snake.dir = DIR_RIGHT; break;
        }
        return;
    }

    /* 字母键处理 */
    switch (ch) {
        case 'w': case 'W': if (gs->snake.dir != DIR_DOWN)  gs->snake.dir = DIR_UP;    break;
        case 's': case 'S': if (gs->snake.dir != DIR_UP)    gs->snake.dir = DIR_DOWN;  break;
        case 'a': case 'A': if (gs->snake.dir != DIR_RIGHT) gs->snake.dir = DIR_LEFT;  break;
        case 'd': case 'D': if (gs->snake.dir != DIR_LEFT)  gs->snake.dir = DIR_RIGHT; break;
        case 'p': case 'P': gs->paused = !gs->paused; break;
        case 'r': case 'R': if (gs->game_over) init_game(gs); break;
        case 'q': case 'Q': exit(0);
    }
}

/* ========== 主函数 ========== */
int main(void)
{
    GameState gs;

    /* 设置控制台 */
    system("title 贪吃蛇 Snake Game");
    system("mode con cols=90 lines=30");
    hide_cursor();

    /* 设置随机种子 */
    srand((unsigned int)GetTickCount());

    /* 初始化游戏 */
    init_game(&gs);

    /* 游戏主循环 */
    while (1) {
        handle_input(&gs);
        update_game(&gs);
        draw_game(&gs);
        draw_ui(&gs);

        Sleep((DWORD)gs.speed);
    }

    return 0;
}
