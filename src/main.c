#include <stdio.h>
//#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "raygui.h"

#define WINDOW_W 640
#define WINDOW_H 480
#define TITLE "FiMod"

typedef struct _entry {
    double debit;
    double credit;
    char *date;
    char *desc;
} Entry;

typedef struct _entrylist {
    int count;
    Entry *list;
} EntryList;


int winid = 0; //keep track of how many windows

//parameters for GuiWindowBox
typedef struct _params {
    Rectangle bounds;
    char *title;
} Parameters;

//label for window
typedef struct _label {
    Vector2 pos;
    Color color;
    char text[20]; //20 char max
} Label;


typedef struct _window Window;
typedef struct _window {
    int id;
    bool exists;
    Parameters params;
    Label *label;
    Window *connections;
};

Window CreateWindow(Rectangle rect, const char *title) {
    return (Window){
        .id = winid++, //might not need this
        .exists = true,
        .params = (Parameters) {rect, title},
        .label = NULL,
        .connections = NULL,
    };
}

Window *winarray[5] = {0};
static bool mousePressed;
static float xdistance;
static float ydistance;
static Window *wp;

int main(void) {
    InitWindow(WINDOW_W, WINDOW_H, TITLE);
    SetTargetFPS(60);

    Window win1 = CreateWindow((Rectangle){30, 30, 100, 100}, "The Window");
    Window win2 = CreateWindow((Rectangle){30, 30, 100, 100}, "The Window 2");
    win1.connections = &win2;
    win2.connections = &win1;
    winarray[0] = &win1;
    winarray[1] = &win2;
    
    //main loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        //DrawBar(&bar);

        for (int i = 0; winarray[i] != NULL; i++) {
            Window *p = winarray[i];
            
            //window drag
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                
                //check if mouse is inside window
                if (CheckCollisionPointRec(GetMousePosition(), p->params.bounds)) {
                    xdistance = GetMousePosition().x - p->params.bounds.x;
                    ydistance = GetMousePosition().y - p->params.bounds.y;
                    //windowID = p;
                    wp = p;
                    mousePressed = true;
                }
            }

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                mousePressed = false;
            }

            if (mousePressed) {
                wp->params.bounds.x = GetMousePosition().x - xdistance;
                wp->params.bounds.y = GetMousePosition().y - ydistance;
                //printf("%p\n", windowID);
            }

            GuiWindowBox(p->params.bounds, p->params.title);
            
            if (p->connections == NULL)
                continue; 
            
            static Vector2 start;
            start.x = p->params.bounds.x + p->params.bounds.width;
            start.y = p->params.bounds.y + p->params.bounds.height / 2;

            static Vector2 end;
            end.x = p->connections->params.bounds.x;
            end.y = p->connections->params.bounds.y + p->connections->params.bounds.height / 2;
            printf("%f\n", end.x);

            DrawLineBezier(start, end, 1.0, BLUE);
            GuiLabel(p->params.bounds, p->connections->params.title);
        }

        EndDrawing();
    }
}