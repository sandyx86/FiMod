#include <stdio.h>
//#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <raygui.h>

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
    Window *connection; //wow you can connect it to another window
};

Window *winarray = {0}; //actual windows

//create a window
Window CreateWindow(Rectangle rect, char *title) {
    return (Window){
        .id = winid++, //might not need this
        .exists = true,
        .params = (Parameters) {rect, title},
        .label = NULL,
        .connection = NULL,
    };
}

//connect w1 to w2
bool ConnectWindow(Window *w1, Window *w2) {
    if (w1->exists && w2->exists) {
        w1->connection = w2;
        return true;
    }
    return false;
}

bool DisconnectWindow(Window *w) {
    w->connection = NULL;
}

//pass an array of windows
void DeleteWindow(Window *w, int id) {
    
    //potentially dangerous

    if (w[id].id + 1 == winid) {
        printf("%d %d\n", w[id].id, winid);
        memset(&w[id], 0, sizeof(Window)); //last window in array
    } else {
        memmove(&w[id], &w[id+1], sizeof(Window)); //not last window
    }
    
    if (w[id].connection != NULL) 
        w[id].connection->connection = NULL;
    
    //w->exists = false;
    
    //re id all the windows
}

enum winop {
    NONE = 0,
    DRAG,
    CONNECT,
};

//Window *winarray[5] = {0};
static bool mousePressed;
static int mode;
static Window *conSrc;
static float xdistance;
static float ydistance;
static Window *wp;

int main(void) {
    InitWindow(WINDOW_W, WINDOW_H, TITLE);
    SetTargetFPS(60);

    winarray = malloc(sizeof(Window) * 100); //space for 100 windows
    winarray[0] = CreateWindow((Rectangle){30, 30, 100, 100}, "The Window");
    winarray[1] = CreateWindow((Rectangle){30, 30, 100, 100}, "The Window 2");
    winarray[0].connection = &winarray[1];
    
    //main loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        for (int i = 0; i < winid; i++) {
            Window *p = &winarray[i];

            switch (mode) {
            case DRAG:
                DrawText("Mode: Drag", WINDOW_H - 20, 0, 20, BLACK);
                wp->params.bounds.x = GetMousePosition().x - xdistance;
                wp->params.bounds.y = GetMousePosition().y - ydistance;
                goto EXIT;
            case CONNECT:
                DrawText("Mode: Connect", WINDOW_H - 20, 0, 20, BLACK);
                Vector2 v = {
                    .x = wp->params.bounds.x + wp->params.bounds.width,
                    .y = wp->params.bounds.y + wp->params.bounds.height / 2,

                };
                DrawLineBezier(v, GetMousePosition(), 1.0, GREEN);

                //check if clicked on window
                if ( (CheckCollisionPointRec(GetMousePosition(), p->params.bounds)) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    wp->connection = p;
                    mode = NONE;
                }

                goto EXIT;
            case NONE:
                DrawText("Mode: None", WINDOW_H - 20, 0, 20, BLACK);

            }
    
EXIT:
            //window drag
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                
                //check if mouse is inside window and pressed
                if (CheckCollisionPointRec(GetMousePosition(), p->params.bounds)) {
                    xdistance = GetMousePosition().x - p->params.bounds.x;
                    ydistance = GetMousePosition().y - p->params.bounds.y;
                    //windowID = p;
                    wp = p;
                    mode = DRAG;
                }
            }

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                mode = NONE;
            }

            if (!p->exists) {
                continue;
            }

            if ( GuiWindowBox(p->params.bounds, p->params.title)) {
                DeleteWindow(winarray, p->id);
            }
                
            if (p->connection == NULL) {
                Rectangle b_rec = (Rectangle){
                    .x = p->params.bounds.x + p->params.bounds.width - 35 - 1,
                    .y = p->params.bounds.y + p->params.bounds.height - 25 - 1,
                    .width = 35,
                    .height = 25,
                };

                if (GuiButton(b_rec, "connect")) {
                    mode = CONNECT;
                    wp = p;
                }
                continue;
            } else {
                Rectangle b_rec = (Rectangle){
                    .x = p->params.bounds.x + p->params.bounds.width - 35 - 1,
                    .y = p->params.bounds.y + p->params.bounds.height - 25 - 1,
                    .width = 35,
                    .height = 25,
                };

                if (GuiButton(b_rec, "disconnect")) {
                    p->connection->connection = NULL;
                }
            }
            
            static Vector2 start;
            start.x = p->params.bounds.x + p->params.bounds.width;
            start.y = p->params.bounds.y + p->params.bounds.height / 2;

            static Vector2 end;
            end.x = p->connection->params.bounds.x;
            end.y = p->connection->params.bounds.y + p->connection->params.bounds.height / 2;
            //printf("%f\n", end.x);

            DrawLineBezier(start, end, 1.0, BLUE);
            GuiLabel(p->params.bounds, p->connection->params.title);
            
        }

        if ( GuiButton((Rectangle){1, 1, 30, 20}, "New") ) {
            Window new = CreateWindow((Rectangle){50, 50, 100, 100}, "yah");
            new.exists = true;
            new.connection = &winarray[0];
            memcpy(&winarray[winid - 1], &new, sizeof(Window));
        }

        EndDrawing();
    }
}