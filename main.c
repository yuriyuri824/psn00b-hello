/*
    Edited from "PSn00bSDK basic graphics example"
    https://github.com/Lameguy64/PSn00bSDK/blob/master/examples/beginner/hello/main.c
    
    * (C) 2025 yuriyuri824 - MPL licensed
    * (C) 2020-2023 Lameguy64, spicyjpeg - MPL licensed

*/

#include <assert.h>
#include <stdio.h>
#include <psxgpu.h>

#define XSCREENRES 	(320)
#define YSCREENRES	(240)
#define XCENTRE		(XSCREENRES >> 1)
#define YCENTRE		(YSCREENRES >> 1)

#define OT_LEN		(16)
#define BUFFER_LEN	(4096)

typedef struct {
    DISPENV disp;
    DRAWENV draw;
    
    uint32_t OT[OT_LEN];
    uint8_t prim_buffer[BUFFER_LEN];
    
} db_t;

typedef struct {
    db_t db[2];
    short active;
    uint8_t *ptr_buffer;
    
} state_t;

extern const uint32_t tim_texture[];

static void flip_buffer(state_t *state) {
    DrawSync(0);
    VSync(0);
    
    short active = state->active;
    
    db_t *buffer0 = &state->db[active];
    db_t *buffer1 = &state->db[active ^ 1];
    
    PutDispEnv(&buffer0->disp);
    PutDrawEnv(&buffer1->draw);
    
    DrawOTag(buffer0->OT + (OT_LEN - 1));

    state->active ^= 1;
    state->ptr_buffer = buffer1->prim_buffer;
    ClearOTagR(buffer1->OT, OT_LEN);
}

static void add_primitive(void *prim, state_t *state, int z, size_t size) {
    short active = state->active;
    db_t *buffer0 = &state->db[active];
    addPrim(&(buffer0->OT[z]), prim);
    
    state->ptr_buffer += size;
    
    assert(state->ptr_buffer <= &(buffer0->prim_buffer[BUFFER_LEN - 1]));
    
    printf("[INFO]: Added prim at index %d\n", z);
}

int main(void) {
    ResetGraph(0);
    
    state_t state = {0};
    TIM_IMAGE tim = {0};
    
    state.ptr_buffer = state.db[0].prim_buffer;

    SetDefDispEnv(&state.db[0].disp, 0, 0, XSCREENRES, YSCREENRES);
    SetDefDrawEnv(&state.db[0].draw, 0, 0, XSCREENRES, YSCREENRES);
    
    SetDefDispEnv(&state.db[1].disp, XSCREENRES, 0, XSCREENRES, YSCREENRES);
    SetDefDrawEnv(&state.db[1].draw, XSCREENRES, 0, XSCREENRES, YSCREENRES);
    
    setRGB0(&state.db[0].draw, 64, 0, 127);
    setRGB0(&state.db[1].draw, 64, 0, 127);
    
    state.db[0].draw.isbg = 1;
    state.db[1].draw.isbg = 1;
    
    SetDispMask(1);
    
    if (GetTimInfo(tim_texture, &tim) != 0) {
        printf("Failed to get TIM info!\n");
        return -1;
    }
    
    printf("[INFO]: TIM mode: 0x%04x\n", tim.mode);
    if (tim.mode & 0x8) {
        LoadImage(tim.crect, tim.caddr);
        DrawSync(0);
    }
    LoadImage(tim.prect, tim.paddr);
    DrawSync(0);

    printf("tim.prect->x = %d tim.prect->y = %d\n", tim.prect->x, tim.prect->y);
    printf("tim.crect->x = %d tim.crect->y = %d\n", tim.crect->x, tim.crect->y);

    ClearOTagR(state.db[0].OT, OT_LEN);
    ClearOTagR(state.db[1].OT, OT_LEN);    
    
    int rectx = 0; int recty = 0;
    int rectdx = 1; int rectdy = 1;
    
    for (;;) {
        short active = state.active;        
        db_t *buffer0 = &state.db[active];
        
        SPRT *sprt = (SPRT*)state.ptr_buffer;
        setSprt(sprt);
        setXY0(sprt, XCENTRE - 64, YCENTRE - 64);
        setWH(sprt, 128, 128);
        setRGB0(sprt, 127, 127, 127);
        setClut(sprt, tim.crect->x, tim.crect->y);
        add_primitive(sprt, &state, 2, sizeof(SPRT));
        
        DR_TPAGE *tp = (DR_TPAGE*)state.ptr_buffer;
        setDrawTPage(tp, 1, 0, getTPage(1, 0, tim.prect->x, tim.prect->y));
        add_primitive(tp, &state, 2, sizeof(DR_TPAGE));
        
        if ((rectx + 64 > XSCREENRES) || (rectx < 0)) rectdx = -rectdx;
        if ((recty + 64 > YSCREENRES) || (recty < 0)) rectdy = -rectdy;
        
        rectx += rectdx;
        recty += rectdy;

        TILE *tile1 = (TILE*)state.ptr_buffer;
        setTile(tile1);
        setXY0(tile1, 0, 0);
        setWH(tile1, 64, 64);
        setRGB0(tile1, 0, 200, 0);
        add_primitive(tile1, &state, 3, sizeof(TILE));

        TILE *tile = (TILE*)state.ptr_buffer;
        setTile(tile);
        setXY0(tile, rectx, recty);
        setWH(tile, 64, 64);
        setRGB0(tile, 200, 200, 0);
        add_primitive(tile, &state, 1, sizeof(TILE));
      
        flip_buffer(&state);    
    }

    return 0;
}