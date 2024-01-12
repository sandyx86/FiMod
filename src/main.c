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

Window *wowawholebunchofmemory = {0}; //actual windows

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

//Window *winarray[5] = {0};
static bool mousePressed;
static bool connectMode = 0;
static Window *conSrc;
static float xdistance;
static float ydistance;
static Window *wp;

int main(void) {
    InitWindow(WINDOW_W, WINDOW_H, TITLE);
    SetTargetFPS(60);

    wowawholebunchofmemory = malloc(sizeof(Window) * 100);
    wowawholebunchofmemory[0] = CreateWindow((Rectangle){30, 30, 100, 100}, "The Window");
    wowawholebunchofmemory[1] = CreateWindow((Rectangle){30, 30, 100, 100}, "The Window 2");
    //winarray[0] = &wowawholebunchofmemory[0];
    wowawholebunchofmemory[0].connection = &wowawholebunchofmemory[1];
    //winarray[1] = &wowawholebunchofmemory[1];
    
    //main loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        for (int i = 0; wowawholebunchofmemory[i].exists; i++) {
            Window *p = &wowawholebunchofmemory[i];
            
            //window drag
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                
                //check if mouse is inside window
                if (CheckCollisionPointRec(GetMousePosition(), p->params.bounds)) {
                    xdistance = GetMousePosition().x - p->params.bounds.x;
                    ydistance = GetMousePosition().y - p->params.bounds.y;
                    //windowID = p;
                    wp = p;
                    mousePressed = true;
                    
                    if (connectMode) {
                        Vector2 v = {
                            .x = conSrc->params.bounds.x,
                            .y = conSrc->params.bounds.y,
                        };
                        //DrawLineBezier(v, GetMousePosition(), 1.0, GREEN);
                        conSrc->connection = p;
                        connectMode = false;
                    }
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

            if (!p->exists) {
                continue;
            }

            if ( GuiWindowBox(p->params.bounds, p->params.title)) {
                DeleteWindow(wowawholebunchofmemory, p->id);
            }
            
            if (connectMode) {
                Vector2 v = {
                            .x = conSrc->params.bounds.x,
                            .y = conSrc->params.bounds.y,
                        };
                DrawLineBezier(v, GetMousePosition(), 1.0, GREEN);

            }
            
            if (p->connection == NULL) {
                Rectangle b_rec = (Rectangle){
                    .x = p->params.bounds.x + p->params.bounds.width - 35 - 1,
                    .y = p->params.bounds.y + p->params.bounds.height - 25 - 1,
                    .width = 35,
                    .height = 25,
                };

                if (GuiButton(b_rec, "connect")) {
                    connectMode = true;
                    conSrc = p;
                }
                continue;
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
            new.connection = &wowawholebunchofmemory[0];
            memcpy(&wowawholebunchofmemory[winid - 1], &new, sizeof(Window));
        }

        EndDrawing();
    }
}