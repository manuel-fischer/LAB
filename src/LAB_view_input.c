#include "LAB_view.h"
#include "LAB_world.h"

#include "LAB_view_input.h"
#include "LAB_window.h"
#include "LAB_block.h"
#include "LAB_chunk.h"
#include "LAB_gui_menu.h"
#include "LAB_gui_inventory.h"
#include "LAB_inventory.h"

#include "LAB_math.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL_fix.h"

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <limits.h> // INT_MAX

#include "LAB_memory.h"
#include "LAB_gl.h"
#include "LAB_sdl.h"

#include "LAB_ext.h"
#include "LAB_util.h"

LAB_STATIC int LAB_ViewInputInteract(LAB_ViewInput* view_input, int right);
LAB_STATIC void LAB_ShowGuiMenu(LAB_View* view);
LAB_STATIC void LAB_ShowGuiInventory(LAB_View* view, LAB_Block** block);
LAB_STATIC void LAB_GrabMouse(LAB_ViewInput* view_input, LAB_Window* window, bool grab);

//static int selected_block = 1;


bool LAB_ConstructViewInput(LAB_ViewInput* view_input, LAB_View* view)
{
    memset(view_input, 0, sizeof *view_input);
    view_input->speed = 3.0f;
    view_input->view = view;
    view_input->brushsize = 3;
    view_input->selected_block = &LAB_BLOCK_STONE_COBBLE;
    return 1;
}

void LAB_DestructViewInput(LAB_ViewInput* view_input)
{

}

LAB_STATIC void LAB_ShowGuiMenu(LAB_View* view)
{
    LAB_GuiMenu* gui = LAB_Malloc(sizeof *gui);
    if(!gui) return;
    LAB_GuiMenu_Create(gui, view->world);
    LAB_GuiManager_ShowDialog(&view->gui_mgr, (LAB_GuiComponent*)gui);
}

LAB_STATIC void LAB_ShowGuiInventory(LAB_View* view, LAB_Block** block)
{
    LAB_GuiInventory* gui = LAB_Malloc(sizeof *gui);
    if(!gui) return;
    LAB_GuiInventory_Create(gui, &LAB_cheat_inventory, block);
    LAB_GuiManager_ShowDialog(&view->gui_mgr, (LAB_GuiComponent*)gui);
}


LAB_STATIC void LAB_GrabMouse(LAB_ViewInput* view_input, LAB_Window* window, bool grab)
{
    view_input->mouse_grabbed = grab;
    SDL_SetRelativeMouseMode(grab);

    if(!grab)
    {
        int w, h;
        SDL_GetWindowSize(window->window, &w, &h);
        SDL_WarpMouseInWindow(window->window, w/2, h/2);
    }
}

LAB_STATIC bool LAB_ViewInput_IsMaskedEvent(LAB_View* view, SDL_Event* event)
{
    switch(event->type)
    {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            SDL_Keycode key = event->key.keysym.sym;
            if(key >= SDLK_F1 && key <= SDLK_F24)
            {
                return 1;
            }
            return 0;
        } break;
        default:
            return 0;
    }
}


int LAB_ViewInputOnEventProc(void* user, LAB_Window* window, SDL_Event* event)
{
    LAB_ViewInput* view_input = (LAB_ViewInput*)user;
    LAB_View* view = view_input->view;

    if(!LAB_ViewInput_IsMaskedEvent(view, event) && LAB_GuiManager_HandleEvent(&view->gui_mgr, event))
    {
        LAB_GuiManager_Tick(&view->gui_mgr);
        if(!view->gui_mgr.component)
            LAB_GrabMouse(view_input, window, 1);
    }
    else switch(event->type)
    {
        case SDL_KEYDOWN:
        {
            SDL_Keycode key = ((SDL_KeyboardEvent*)event)->keysym.sym;

            switch(key)
            {
                /*case SDLK_w: view_input->dir_set |= 1; break;
                case SDLK_a: view_input->dir_set |= 2; break;
                case SDLK_s: view_input->dir_set |= 4; break;
                case SDLK_d: view_input->dir_set |= 8; break;

                case SDLK_SPACE:  view_input->updown |= 1; break;
                case SDLK_LSHIFT: view_input->updown |= 2; break;*/


                case SDLK_u:
                case SDLK_i:
                {
                    view->az += key == SDLK_u ? -1 : 1;
                    int siz;
                    const uint8_t* state = SDL_GetKeyboardState(&siz);
                    int other_scancode = (key+(SDL_SCANCODE_U-SDLK_u))^(SDL_SCANCODE_U^SDL_SCANCODE_I);
                    if(state[other_scancode]) view->az = 0;
                } break;

                case SDLK_SPACE:
                {
                    if(!view_input->space_pressed)
                    {
                        uint32_t time = SDL_GetTicks();
                        if(time-view_input->prev_space_down < 300/*ms*/)
                        {
                            view_input->flags ^= LAB_VIEWINPUT_NOCLIP;
                            view->vx = view->vy = view->vz = 0;
                        }

                        view_input->space_pressed = true;
                        view_input->prev_space_down = time;
                    }
                } break;
            }
        } break;
        case SDL_KEYUP:
        {
            SDL_Keycode key = ((SDL_KeyboardEvent*)event)->keysym.sym;

            switch(key)
            {
                case SDLK_SPACE:
                {
                    view_input->space_pressed = false;
                } break;

                /*case SDLK_w: view_input->dir_set &= ~1; break;
                case SDLK_a: view_input->dir_set &= ~2; break;
                case SDLK_s: view_input->dir_set &= ~4; break;
                case SDLK_d: view_input->dir_set &= ~8; break;

                case SDLK_SPACE:  view_input->updown &= ~1; break;
                case SDLK_LSHIFT: view_input->updown &= ~2; break;*/

                /*case SDLK_o:
                {
                    view->x = LAB_FastFloorF2I(view->x);
                    view->y = LAB_FastFloorF2I(view->y);
                    view->z = LAB_FastFloorF2I(view->z);
                } break;*/
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
                case SDLK_f:
                {
                    view_input->flags ^= LAB_VIEWINPUT_NOCLIP;
                    view->vx = view->vy = view->vz = 0;
                } break;

                case SDLK_MINUS:
                case SDLK_PLUS:
                {
                    if(key == SDLK_MINUS) view->render_dist--;
                    if(key == SDLK_PLUS) view->render_dist++;
                    if(view->render_dist == 0) view->render_dist = 1;
                    if(view->render_dist > 64) view->render_dist = 64;
                    view->preload_dist = LAB_PRELOAD_CHUNK(view->render_dist);
                    view->keep_dist = LAB_KEEP_CHUNK(view->render_dist);
                } break;

                case SDLK_n:
                case SDLK_m:
                {
                    /*if(key == SDLK_n) view_input->speed-=0.02;
                    if(key == SDLK_m) view_input->speed+=0.02;
                    if(view_input->speed < 0.02) view_input->speed = 0.02;
                    if(view_input->speed > 16)  view_input->speed = 16;*/
                    if(key == SDLK_n) view_input->speed-=1;
                    if(key == SDLK_m) view_input->speed+=1;
                    if(view_input->speed < 1) view_input->speed = 1;
                    if(view_input->speed > 100)  view_input->speed = 100;
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
                    LAB_ShowGuiMenu(view);
                    LAB_GrabMouse(view_input, window, 0);
                } break;

                case SDLK_e:
                {
                    LAB_ShowGuiInventory(view, &view_input->selected_block);
                    LAB_GrabMouse(view_input, window, 0);
                } break;

                case SDLK_F1:
                {
                    view->flags ^= LAB_VIEW_SHOW_HUD;
                } break;

                case SDLK_F9:
                {
                    view->flags ^= LAB_VIEW_SHOW_CHUNK_GRID;
                } break;

                case SDLK_F2:
                {
                    // TODO detection if multiple screenshots are saved in the same second
                    SDL_Surface* surf_screen;
                    LAB_SDL_ALLOC(SDL_CreateRGBSurfaceWithFormat, &surf_screen, 0, view->w, view->h, 32, SDL_PIXELFORMAT_RGBA32);
                    if(!surf_screen) break;
                    glReadPixels(0, 0, view->w, view->h, GL_RGBA, GL_UNSIGNED_BYTE, surf_screen->pixels);
                    LAB_GL_FixScreenImg(surf_screen->pixels, view->w, view->h);

                    time_t t;
                    struct tm* tinf;

                    time(&t);
                    tinf = localtime(&t); // TODO: localtime_r C2x

                    char fname[256];
                    strftime(fname, sizeof fname, "screenshots/scr-%Y%m%d-%H%M%S.png", tinf);
                    printf("Saving screenshot to %s\n", fname);
                    IMG_SavePNG(surf_screen, fname);
                    LAB_SDL_FREE(SDL_FreeSurface, &surf_screen);
                } break;

                case SDLK_F3:
                {
                    view->flags ^= LAB_VIEW_SHOW_FPS_GRAPH;

                } break;

                case SDLK_F4:
                {
                    view->flags ^= LAB_VIEW_USE_VBO;
                    printf("VBO turned %s\n", "off\0on"+4*!!(view->flags & LAB_VIEW_USE_VBO));
                    LAB_ViewInvalidateEverything(view, /*free_buffers*/1);

                } break;

                case SDLK_F6:
                {
                    view->flags ^= LAB_VIEW_BRIGHTER;
                    LAB_ViewInvalidateEverything(view, /*free_buffers*/0);

                } break;
                case SDLK_F7:
                {
                    view->flags ^= LAB_VIEW_FLAT_SHADE;
                    LAB_ViewInvalidateEverything(view, /*free_buffers*/0);

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
                        glDeleteBuffers(1, &view->chunks[i].vbo); LAB_GL_DEBUG_FREE(1);
                        LAB_Free(view->chunks[i].mesh);
                    }
                    memset(view->chunks, 0, view->chunk_count);
                    view->chunk_count = 0;
                } break;
                #endif

                LAB_CASE_RANGE('1', 9):
                {
                    int id = key-'1';
                    view_input->selected_block = LAB_blocks[id];
                } break;

                default: break;
            }
        } break;


        case SDL_MOUSEMOTION:
        {
            if(view_input->mouse_grabbed)
            {
                SDL_MouseMotionEvent* mmevent = (SDL_MouseMotionEvent*)event;
                const float speed = 1.f/16.f;
                float mx, my;
                mx = (float)(mmevent->xrel) * speed;
                my = (float)(mmevent->yrel) * speed;
                #if LAB_CLIP_AX
                view->ay+=mx;
                view->ax+=my;

                if(view->ax < -90) view->ax = -90;
                if(view->ax >  90) view->ax =  90;
                #else

                float ax, ay, az;
                ax = view->ax*(LAB_PI/180.f);
                ay = view->ay*(LAB_PI/180.f);
                az = view->az*(LAB_PI/180.f);

                float cx = cos(ax), sx = sin(ax);
                float cy = cos(ay), sy = sin(ay);
                float cz = cos(az), sz = sin(az);
                view->ax+=        cz*my +         sz*mx;
                view->ay+=    cx*-sz*my +      cx*cz*mx;
                view->az+=                    -sx*   mx;

                view->ax = LAB_AbsModF(view->ax, 360.f);
                view->ay = LAB_AbsModF(view->ay, 360.f);
                view->az = LAB_AbsModF(view->az, 360.f);
                #endif
            }

        } break;

        case SDL_MOUSEBUTTONDOWN:
        {
            SDL_MouseButtonEvent* mbevent = (SDL_MouseButtonEvent*)event;

            if(view_input->mouse_grabbed)
            {
                LAB_ViewInputInteract(view_input, mbevent->button);
            }
            else
            {
                LAB_GrabMouse(view_input, window, 1);
            }
        } break;

    }
    return 1;
}

LAB_STATIC int LAB_ViewInputInteract(LAB_ViewInput* view_input, int button)
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

    if(LAB_TraceBlock(view->world, 10, vpos, dir, LAB_CHUNK_GENERATE, LAB_BLOCK_INTERACTABLE, target, prev, hit))
    {
        switch(button)
        {
            case SDL_BUTTON_LEFT:
            {
                LAB_SetBlock(view->world, target[0], target[1], target[2], LAB_CHUNK_GENERATE, &LAB_BLOCK_AIR);
            } break;

            case SDL_BUTTON_RIGHT:
            {
                if(!(view_input->flags&LAB_VIEWINPUT_NOCLIP) && (view_input->selected_block->flags&LAB_BLOCK_MASSIVE)
                   &&  prev[0]==LAB_FastFloorF2I(view->x)
                   && (prev[1]==LAB_FastFloorF2I(view->y) || prev[1]==LAB_FastFloorF2I(view->y)-1)
                   &&  prev[2]==LAB_FastFloorF2I(view->z)) return 0;
                LAB_SetBlock(view->world, prev[0], prev[1], prev[2], LAB_CHUNK_GENERATE, view_input->selected_block);
            } break;

            default: // SDL_BUTTON_MIDDLE
            {
                LAB_Block* b = LAB_GetBlock(view->world, target[0], target[1], target[2], LAB_CHUNK_EXISTING);
                if(b != &LAB_BLOCK_OUTSIDE)
                {
                    view_input->selected_block = b;
                }
            } break;
        }
        return 1;
    }
    return 0;
}


void LAB_ViewInputTick(LAB_ViewInput* view_input, uint32_t delta_ms)
{

    LAB_View* view = view_input->view;
    float dx = 0, dy = 0, dz = 0;

    //if(view->az > 180) view->az -= 360;
    //view->az *= 0.95;

    if(view->gui_mgr.component == NULL)
    {
        int kbstate_size;
        const Uint8* kbstate = SDL_GetKeyboardState(&kbstate_size);
        int mx, my;
        Uint32 mbstate = SDL_GetMouseState(&mx, &my);

        float speed = view_input->speed * (kbstate[SDL_SCANCODE_LCTRL] ? 2.5 : 1);
        speed *= (float)delta_ms*(1.f/1000.f);


        //unsigned dir_set = view_input->dir_set;
        unsigned dir_set = (kbstate[SDL_SCANCODE_W]?1:0)
                         | (kbstate[SDL_SCANCODE_A]?2:0)
                         | (kbstate[SDL_SCANCODE_S]?4:0)
                         | (kbstate[SDL_SCANCODE_D]?8:0);
        if(view_input->flags & LAB_VIEWINPUT_FORWARD)
        {
            if(dir_set&(1|4))
            {
                int bw = (dir_set&1) - !!(dir_set&4);
                float dir[3];
                LAB_ViewGetDirection(view, dir);
                dx += dir[0]*speed*bw;
                dy += dir[1]*speed*bw;
                dz += dir[2]*speed*bw;
                if(!(view_input->flags & LAB_VIEWINPUT_NOCLIP)) view->vy += pow((1+dir[1]*bw)*0.5, 0.7)*speed;
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

                dx += s*speed;
                dz -= c*speed;
            }


            //if(kbstate[SDL_SCANCODE_SPACE]) dy+=speed;
            if(kbstate[SDL_SCANCODE_SPACE]) {
                if(!(view_input->flags&LAB_VIEWINPUT_NOCLIP))
                {
                    if(view->on_ground)
                        view->vy = 4.8*(1.4);
                }
                else
                    dy+=speed;
                //if(view->vy < 0) view->vy = 0;
            }
            if(kbstate[SDL_SCANCODE_LSHIFT]) dy-=speed;
        }

        if(kbstate[SDL_SCANCODE_LALT])
        {
            //for(int i=0; i < 10; ++i)
            //    if(LAB_ViewInputInteract(view_input, !!(mbstate & SDL_BUTTON(SDL_BUTTON_RIGHT)))) break;
            int button = 0;
            if(mbstate & SDL_BUTTON(SDL_BUTTON_MIDDLE)) button = SDL_BUTTON_MIDDLE;
            if(mbstate & SDL_BUTTON(SDL_BUTTON_RIGHT))  button = SDL_BUTTON_RIGHT;
            if(mbstate & SDL_BUTTON(SDL_BUTTON_LEFT))   button = SDL_BUTTON_LEFT;
            if(button) LAB_ViewInputInteract(view_input, button);
        }
    }


    if(view_input->flags & (LAB_VIEWINPUT_DESTROY|LAB_VIEWINPUT_CREATE))
    {
        LAB_Block* block = view_input->flags & LAB_VIEWINPUT_CREATE ? view_input->selected_block : &LAB_BLOCK_AIR;

        int bx, by, bz;
        bx = LAB_FastFloorF2I(view->x);
        by = LAB_FastFloorF2I(view->y);
        bz = LAB_FastFloorF2I(view->z);

        int dist = view_input->brushsize;
        for(int zz = -dist; zz <= dist; ++zz)
        for(int yy = -dist; yy <= dist; ++yy)
        for(int xx = -dist; xx <= dist; ++xx)
        {
            if((view_input->flags & LAB_VIEWINPUT_CREATE) && LAB_GetBlock(view->world, bx+xx, by+yy, bz+zz, LAB_CHUNK_GENERATE) != &LAB_BLOCK_AIR)
                continue;
            if(xx*xx+yy*yy+zz*zz <= dist*dist+dist)
                LAB_SetBlock(view->world, bx+xx, by+yy, bz+zz, LAB_CHUNK_GENERATE, block);
        }
    }

    if(view_input->flags & LAB_VIEWINPUT_NOCLIP)
    {
        view->vx *= 0.95;
        view->vy *= 0.95;
        view->vz *= 0.95;

        view->vx += dx*0.5;
        view->vy += dy*0.5;
        view->vz += dz*0.5;

        view->x += view->vx;
        view->y += view->vy;
        view->z += view->vz;

        //view->vy = 0;
    }
    else
    {
        // TODO: collect/iterate local collision boxes, all inside the entity box
        //       and some blocks further out (MAX_COLLISION_SIZE blocks)
        //     - allows partial blocks like stairs or pipes
        //     - if the difference between the bottom of the entity and the top
        //       of the collision box is around 1/2 of a block, the entity gets moved
        //       up -> walking up stairs smoothly without jumping
        //

        const float d = 0.001;

        float dt = (float)delta_ms*(1.f/1000.f);
        const float accel = -9.81*2;

        dy += accel*dt*dt*0.5 + view->vy*dt;
        view->vy += accel*dt;
        //if(dy > 0) view->vy = dy;

        // TODO move this collision physics to the world itself
        // clipping
        const float maxstep = 0.0625f;
        float fsteps = ceilf(LAB_MAX3(fabs(dx), fabs(dy), fabs(dz))/maxstep + 1);
        LAB_ASSUME(fsteps<INT_MAX);
        int steps = (int)fsteps;

        float ddx = dx/(float)steps, ddy = dy/(float)steps, ddz = dz/(float)steps;
        LAB_ASSUME(ddx <= maxstep);
        LAB_ASSUME(ddy <= maxstep);
        LAB_ASSUME(ddz <= maxstep);

        //printf("steps=%i, %f|%f  %f|%f  %f|%f\n", steps, dx, ddx, dy, ddy, dz, ddz);

        for(int step = 0; step < steps; ++step)
        {
            view->x += ddx;
            view->y += ddy;
            view->z += ddz;
            //printf("    %04i: %f %f %f\n", step, view->x, view->y, view->z);

            int bx, by, bz;
            bx = LAB_FastFloorF2I(view->x);
            by = LAB_FastFloorF2I(view->y);
            bz = LAB_FastFloorF2I(view->z);

            float fx, fy, fz;
            fx = view->x-(float)bx;
            fy = view->y-(float)by;
            fz = view->z-(float)bz;

            static const int8_t xz[8][2] = {
                { -1,  0 },
                {  0,  1 },
                {  1,  0 },
                {  0, -1 },
                { -1, -1 },
                { -1,  1 },
                {  1, -1 },
                {  1,  1 },
            };

            view->on_ground = 0;
            for(int i = 0; i < 2; ++i)
            //for(int yy = -2; yy <= 1; ++yy)
            {
                int yy = i ? 1 : -2;
                for(int xx = -1; xx <= 1; ++xx)
                for(int zz = -1; zz <= 1; ++zz)
                {
                    LAB_Block* block  = LAB_GetBlock(view->world, bx+xx, by+yy, bz+zz, LAB_CHUNK_GENERATE);

                    if(xx!=0||zz!=0)
                    {
                        // Blocks on the same axis
                        LAB_Block* b1 = LAB_GetBlock(view->world, bx+xx, by+0,  bz,    LAB_CHUNK_GENERATE);
                        LAB_Block* b2 = LAB_GetBlock(view->world, bx+xx, by-1,  bz,    LAB_CHUNK_GENERATE);
                        LAB_Block* b3 = LAB_GetBlock(view->world, bx,    by+0,  bz+zz, LAB_CHUNK_GENERATE);
                        LAB_Block* b4 = LAB_GetBlock(view->world, bx,    by-1,  bz+zz, LAB_CHUNK_GENERATE);
                        if(b1->flags&LAB_BLOCK_MASSIVE || b2->flags&LAB_BLOCK_MASSIVE) continue;
                        if(b3->flags&LAB_BLOCK_MASSIVE || b4->flags&LAB_BLOCK_MASSIVE) continue;

                        // Blocks possibly in the corners
                        LAB_Block* b5 = LAB_GetBlock(view->world, bx+xx, by+0,  bz+zz, LAB_CHUNK_GENERATE);
                        LAB_Block* b6 = LAB_GetBlock(view->world, bx+xx, by-1,  bz+zz, LAB_CHUNK_GENERATE);
                        if(b5->flags&LAB_BLOCK_MASSIVE || b6->flags&LAB_BLOCK_MASSIVE) continue;
                    }

                    if(block->flags&LAB_BLOCK_MASSIVE)
                    {
                        int collides = 1;
                        collides &= (xx < 0 && fx < 0.2) || (xx > 0 && fx > 0.8) || xx == 0;
                        collides &= (zz < 0 && fz < 0.2) || (zz > 0 && fz > 0.8) || zz == 0;
                        collides &= (yy <-1 && fy < 0.4 && fy > 0.3) || (yy > 0 && fy > 0.8 && fy < 0.9) || yy ==-1 || yy == 0;

                        if(collides)
                        {
                            /**/ if(yy < 0) { view->y = by+0.4+d; view->vy = LAB_MAX(view->vy, 0); view->on_ground = 1; }
                            else if(yy > 0) { view->y = by+0.8-d; view->vy = LAB_MIN(view->vy, 0); }


                            fx = view->x-bx;
                            fy = view->y-by;
                            fz = view->z-bz;
                            goto exit_for_yy;
                        }
                    }
                }
            }
            exit_for_yy:;

            int f = 0;
            for(int i = 0; i < 8; ++i)
            {
                int xx = xz[i][0];
                int zz = xz[i][1];
                for(int yy = -2; yy <= 1; ++yy)
                {
                    LAB_Block* block = LAB_GetBlock(view->world, bx+xx, by+yy, bz+zz, LAB_CHUNK_GENERATE);
                    if(block->flags&LAB_BLOCK_MASSIVE)
                    {
                        int collides = 1;
                        collides &= (xx < 0 && fx < 0.2) || (xx > 0 && fx > 0.8) || xx == 0;
                        collides &= (zz < 0 && fz < 0.2) || (zz > 0 && fz > 0.8) || zz == 0;
                        collides &= (yy <-1 && fy < 0.4) || (yy > 0 && fy > 0.8) || yy ==-1 || yy == 0;


                        if(collides)
                        {
                            if(fabs(fx-0.5f) < fabs(fz-0.5f))
                            //if(abs(xx) > abs(zz))
                            //if(0)
                            {
                              /**/ if(xx < 0) { view->x = bx+0.2+d; view->vx = 0; }
                              else if(xx > 0) { view->x = bx+0.8-d; view->vx = 0; }
                              else if(zz < 0) { view->z = bz+0.2+d; view->vz = 0; }
                              else if(zz > 0) { view->z = bz+0.8-d; view->vz = 0; }
                            }
                            else
                            {
                                /**/ if(zz < 0) { view->z = bz+0.2+d; view->vz = 0; }
                                else if(zz > 0) { view->z = bz+0.8-d; view->vz = 0; }
                                else if(xx < 0) { view->x = bx+0.2+d; view->vx = 0; }
                                else if(xx > 0) { view->x = bx+0.8-d; view->vx = 0; }
                            }
                            //if(yy < 0) view->y = by+0.5;
                            //if(yy > 0) view->y = by+0.5;
                            fx = view->x-bx;
                            fy = view->y-by;
                            fz = view->z-bz;
                            f = 1;
                        }
                    }

                }
                if(f && i == 3) break;
            }
        }
    }
}
