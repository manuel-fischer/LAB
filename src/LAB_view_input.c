#include "LAB_world_view.h"
#include "LAB_world.h"

#include "LAB_view_input.h"
#include "LAB_window.h"
#include "LAB_block.h"
#include "LAB_chunk.h"

#include "LAB_math.h"

#include <SDL2/SDL.h>
#include "SDL_fix.h"

#include <math.h>
#include <stdio.h>

#include "LAB_memory.h"
#include "LAB_gl.h"

#include "LAB_ext.h"

static int LAB_ViewInputInteract(LAB_ViewInput* view_input, int right);
static LAB_Block* blocks[9] =
{
    &LAB_BLOCK_STONE,
    &LAB_BLOCK_COBBLESTONE,
    &LAB_BLOCK_MARBLE,
    &LAB_BLOCK_MARBLECOBBLE,
    &LAB_BLOCK_WOOD,
    &LAB_BLOCK_WOOD_PLANKS,
    &LAB_BLOCK_WOOD_PLANKS_DARK,
    &LAB_BLOCK_GRASS,
    &LAB_BLOCK_LIGHT
};
static int selected_block = 1;


int  LAB_ConstructViewInput(LAB_ViewInput* view_input, LAB_View* view)
{
    memset(view_input, 0, sizeof *view_input);
    view_input->speed = 0.1f;
    view_input->view = view;
    view_input->brushsize = 3;
    return 1;
}

void LAB_DestructViewInput(LAB_ViewInput* view_input)
{

}



int LAB_ViewInputOnEventProc(void* user, LAB_Window* window, SDL_Event* event)
{
    LAB_ViewInput* view_input = (LAB_ViewInput*)user;
    LAB_View* view = view_input->view;

    switch(event->type)
    {
        case SDL_KEYDOWN:
        {
            SDL_Keycode key = ((SDL_KeyboardEvent*)event)->keysym.sym;

            switch(key)
            {
                case SDLK_w: view_input->dir_set |= 1; break;
                case SDLK_a: view_input->dir_set |= 2; break;
                case SDLK_s: view_input->dir_set |= 4; break;
                case SDLK_d: view_input->dir_set |= 8; break;

                case SDLK_SPACE:  view_input->updown |= 1; break;
                case SDLK_LSHIFT: view_input->updown |= 2; break;

                LAB_CASE_RANGE('1', 9):
                {
                    int id = key-'1';
                    if(blocks[id] != NULL)
                    {
                        selected_block = id;
                    }
                } break;
            }
        } break;
        case SDL_KEYUP:
        {
            SDL_Keycode key = ((SDL_KeyboardEvent*)event)->keysym.sym;

            switch(key)
            {
                case SDLK_w: view_input->dir_set &= ~1; break;
                case SDLK_a: view_input->dir_set &= ~2; break;
                case SDLK_s: view_input->dir_set &= ~4; break;
                case SDLK_d: view_input->dir_set &= ~8; break;

                case SDLK_SPACE:  view_input->updown &= ~1; break;
                case SDLK_LSHIFT: view_input->updown &= ~2; break;

                case SDLK_x:
                {
                    view_input->flags ^= LAB_VIEWINPUT_DESTROY;
                    view_input->flags &= ~LAB_VIEWINPUT_CREATE;
                } break;
                case SDLK_c:
                {
                    view_input->flags ^= LAB_VIEWINPUT_CREATE;
                    view_input->flags &= ~LAB_VIEWINPUT_DESTROY;

                } break;

                case SDLK_MINUS:
                case SDLK_PLUS:
                {
                    if(key == SDLK_MINUS) view->render_dist--;
                    if(key == SDLK_PLUS) view->render_dist++;
                    if(view->render_dist == 0) view->render_dist = 1;
                    if(view->render_dist > 16) view->render_dist = 16;
                    view->preload_dist = view->render_dist+0;
                    view->keep_dist = view->render_dist+2;
                } break;

                case SDLK_n:
                case SDLK_m:
                {
                    if(key == SDLK_n) view_input->speed-=0.1;
                    if(key == SDLK_m) view_input->speed+=0.1;
                    if(view_input->speed < 0.1) view_input->speed = 0.1;
                    if(view_input->speed > 16)  view_input->speed = 16;
                } break;


                case SDLK_v:
                case SDLK_b:
                {
                    if(key == SDLK_v) view_input->brushsize--;
                    if(key == SDLK_b) view_input->brushsize++;
                    if(view_input->brushsize <  1) view_input->brushsize = 1;
                    if(view_input->brushsize > 16) view_input->brushsize = 16;
                } break;


                case SDLK_y:
                {
                    view_input->flags ^= LAB_VIEWINPUT_FORWARD;
                } break;



                case SDLK_ESCAPE:
                {
                    int grab;
                    grab = !!(view_input->flags&LAB_VIEWINPUT_GRAB); //SDL_GetWindowGrab(window->window);
                    SDL_SetWindowGrab_Fix(window->window, !grab);
                    printf("GRAB %u %u %p\n", !grab, SDL_GetWindowGrab(window->window), SDL_GetGrabbedWindow());
                    SDL_ShowCursor(grab);
                    view_input->flags ^= LAB_VIEWINPUT_GRAB;
                    if(!grab)
                    {
                        int w, h;
                        SDL_GetWindowSize(window->window, &w, &h);
                        SDL_WarpMouseInWindow(window->window, w/2, h/2);
                    }
                } break;

                case SDLK_F1:
                {
                    view->flags ^= LAB_VIEW_SHOW_GUI;
                } break;

                case SDLK_F3:
                {
                    view->flags ^= LAB_VIEW_USE_VBO;
                    printf("VBO turned %s\n", "off\0on"+4*!!(view->flags & LAB_VIEW_USE_VBO));
                    LAB_ViewInvalidateEverything(view);

                } break;

                case SDLK_F11:
                {
                    uint32_t fs_flags;
                    fs_flags = (SDL_GetWindowFlags(window->window) & SDL_WINDOW_FULLSCREEN)
                             ? 0
                             : SDL_WINDOW_FULLSCREEN_DESKTOP;
                    SDL_SetWindowFullscreen(window->window, fs_flags);
                } break;

                #if 0
                case SDLK_F4:
                {
                    //LAB_ChunkMap_Destruct(&view->world->chunks);
                    //LAB_ChunkMap_Construct(&view->world->chunks);

                    for(int i = 0; i < view->chunk_count; ++i)
                    {
                        LAB_Free(view->chunks[i].mesh);
                        glDeleteBuffers(1, &view->chunks[i].vbo);
                    }
                    memset(view->chunks, 0, view->chunk_count);
                    view->chunk_count = 0;
                } break;
                #endif

                default: break;
            }
        } break;


        case SDL_MOUSEMOTION:
        {
            if(view_input->flags&LAB_VIEWINPUT_GRAB)
            {
                int w, h;
                SDL_GetWindowSize(window->window, &w, &h);

                SDL_MouseMotionEvent* mmevent = (SDL_MouseMotionEvent*)event;
                view->ay+=(float)(mmevent->x-w/2) / 4.f;
                view->ax+=(float)(mmevent->y-h/2) / 4.f;

                if(view->ax < -90) view->ax = -90;
                if(view->ax >  90) view->ax =  90;

                SDL_WarpMouseInWindow(window->window, w/2, h/2);
            }

        } break;

        case SDL_MOUSEBUTTONDOWN:
        {
            SDL_MouseButtonEvent* mbevent = (SDL_MouseButtonEvent*)event;

            if(mbevent->button == SDL_BUTTON_LEFT || mbevent->button == SDL_BUTTON_RIGHT)
            {
                if(view_input->flags&LAB_VIEWINPUT_GRAB)
                {
                    LAB_ViewInputInteract(view_input, mbevent->button == SDL_BUTTON_RIGHT);
                }
                else
                {
                    SDL_SetWindowGrab_Fix(window->window, 1);
                    view_input->flags |= LAB_VIEWINPUT_GRAB;
                    SDL_ShowCursor(0);

                    int w, h;
                    SDL_GetWindowSize(window->window, &w, &h);
                    SDL_WarpMouseInWindow(window->window, w/2, h/2);
                }
            }

        }

    }
    return 1;
}

static int LAB_ViewInputInteract(LAB_ViewInput* view_input, int right)
{
    LAB_View* view = view_input->view;

    int target[3]; // targeted block
    int prev[3]; // previous block
    float hit[3]; // hit pos

    // view-pos
    float vpos[3];
    // view-dir
    float dir[3];

    vpos[0] = view->x;
    vpos[1] = view->y;
    vpos[2] = view->z;
    LAB_ViewGetDirection(view, dir);

    if(LAB_TraceBlock(view->world, 10, vpos, dir, LAB_CHUNK_GENERATE, LAB_BLOCK_SOLID, target, prev, hit))
    {
        if(!right)
        {
            LAB_SetBlock(view->world, target[0], target[1], target[2], LAB_CHUNK_GENERATE, &LAB_BLOCK_AIR);
        }
        else
        {
            LAB_SetBlock(view->world, prev[0], prev[1], prev[2], LAB_CHUNK_GENERATE, blocks[selected_block]);
        }
        return 1;
    }
    return 0;
}


void LAB_ViewInputTick(LAB_ViewInput* view_input)
{

    LAB_View* view = view_input->view;

    int kbstate_size;
    const Uint8* kbstate = SDL_GetKeyboardState(&kbstate_size);
    int mx, my;
    Uint32 mbstate = SDL_GetMouseState(&mx, &my);

    float speed = view_input->speed * (kbstate[SDL_SCANCODE_LCTRL] ? 7.5 : 1);

    unsigned dir_set = view_input->dir_set;
    if(view_input->flags & LAB_VIEWINPUT_FORWARD)
    {
        if(dir_set&(1|4))
        {
            int bw = (dir_set&1) - !!(dir_set&4);
            float dir[3];
            LAB_ViewGetDirection(view, dir);
            view->x += dir[0]*speed*bw;
            view->y += dir[1]*speed*bw;
            view->z += dir[2]*speed*bw;
        }
        dir_set&=(2|8);
    }

    {


        //              w.w.w.w.w.w.w.w.
        //              aa..aa..aa..aa..
        //              ssss....ssss....
        //              dddddddd........
        int ang8th = (0xf4650f7623f4120full >> (dir_set*4)) & 0xf;
        if(ang8th != 0xf)
        {
            float ang_rad = view->ay*LAB_PI/180.f - (float)ang8th*LAB_PI*2/8.f;

            float s = sin(ang_rad);
            float c = cos(ang_rad);

            view->x += s*speed;
            view->z -= c*speed;
        }

        if(view_input->updown&1) view->y+=speed;
        if(view_input->updown&2) view->y-=speed;
    }

    if(kbstate[SDL_SCANCODE_LALT])
    {
        if(mbstate == SDL_BUTTON(SDL_BUTTON_LEFT) || mbstate == SDL_BUTTON(SDL_BUTTON_RIGHT))
        {
            //for(int i=0; i < 10; ++i)
            //    if(LAB_ViewInputInteract(view_input, !!(mbstate & SDL_BUTTON(SDL_BUTTON_RIGHT)))) break;
            LAB_ViewInputInteract(view_input, !!(mbstate & SDL_BUTTON(SDL_BUTTON_RIGHT)));
        }
    }



    if(view_input->flags & (LAB_VIEWINPUT_DESTROY|LAB_VIEWINPUT_CREATE))
    {
        LAB_Block* block = view_input->flags & LAB_VIEWINPUT_CREATE ? blocks[selected_block] : &LAB_BLOCK_AIR;

        int bx, by, bz;
        bx = (int)floorf(view->x);
        by = (int)floorf(view->y);
        bz = (int)floorf(view->z);

        int dist = view_input->brushsize;
        for(int zz = -dist; zz <= dist; ++zz)
        for(int yy = -dist; yy <= dist; ++yy)
        for(int xx = -dist; xx <= dist; ++xx)
        {
            if(xx*xx+yy*yy+zz*zz <= dist*dist+dist)
                LAB_SetBlock(view->world, bx+xx, by+yy, bz+zz, LAB_CHUNK_GENERATE, block);
        }
    }


}
