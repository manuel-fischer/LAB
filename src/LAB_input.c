#include "LAB_view.h"
#include "LAB_world.h"

#include "LAB_input.h"
#include "LAB_window.h"
#include "LAB_block.h"
#include "LAB_chunk.h"

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
#include "LAB_debug.h"

#include "LAB_game_server.h"

#include "LAB_player.h"
#include "LAB_blocks.h" // for inventory

LAB_STATIC int  LAB_Input_Interact(LAB_Input* input, int button);
LAB_STATIC void LAB_Input_GrabMouse(LAB_Input* input, LAB_Window* window, bool grab);
LAB_STATIC bool LAB_Input_IsMaskedEvent(LAB_View* view, SDL_Event* event);

LAB_STATIC void LAB_DBG_Input_OnError(void* vinput);

//static int selected_block = 1;


bool LAB_Input_Create(LAB_Input* input, LAB_View* view)
{
    memset(input, 0, sizeof *input);
    input->speed = 3.0f;
    input->view = view;
    input->brushsize = 3;
    input->selected_block = LAB_BID_AIR; //LAB_BLOCK_STONE.cobble;

    LAB_DbgAtHalt(&LAB_DBG_Input_OnError, input);

    return 1;
}

void LAB_Input_Destroy(LAB_Input* input)
{
    LAB_DbgRemoveHalt(&LAB_DBG_Input_OnError, input);
}

LAB_STATIC void LAB_DBG_Input_OnError(void* vinput)
{
    LAB_Input* input = vinput;
    input->mouse_grabbed = false;
    SDL_SetRelativeMouseMode(false);
}


LAB_STATIC void LAB_Input_GrabMouse(LAB_Input* input, LAB_Window* window, bool grab)
{
    input->mouse_grabbed = grab;
    SDL_SetRelativeMouseMode(grab);

    if(!grab)
    {
        int w, h;
        SDL_GetWindowSize(window->window, &w, &h);
        SDL_WarpMouseInWindow(window->window, w/2, h/2);
    }
}

LAB_STATIC bool LAB_Input_IsMaskedEvent(LAB_View* view, SDL_Event* event)
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


int LAB_Input_OnEvent_Proc(void* user, LAB_Window* window, SDL_Event* event)
{
    LAB_Input* input = (LAB_Input*)user;
    LAB_View* view = input->view;

    if(!LAB_Input_IsMaskedEvent(view, event) && LAB_GuiManager_HandleEvent(&view->gui_mgr, event))
    {
        LAB_GuiManager_Tick(&view->gui_mgr);
        if(!view->gui_mgr.component)
            LAB_Input_GrabMouse(input, window, 1);
    }
    else switch(event->type)
    {
        case SDL_KEYDOWN:
        {
            SDL_Keycode key = ((SDL_KeyboardEvent*)event)->keysym.sym;
            const Uint8* kbstate = SDL_GetKeyboardState(NULL);

            if(SDLK_0 <= key && key <= SDLK_9)
            {
                if(kbstate[SDL_SCANCODE_F3])
                {
                    input->graph_selected = true;
                }
            }

            switch(key)
            {
                /*case SDLK_w: input->dir_set |= 1; break;
                case SDLK_a: input->dir_set |= 2; break;
                case SDLK_s: input->dir_set |= 4; break;
                case SDLK_d: input->dir_set |= 8; break;

                case SDLK_SPACE:  input->updown |= 1; break;
                case SDLK_LSHIFT: input->updown |= 2; break;*/


                case SDLK_u:
                case SDLK_i:
                {
                    view->angle.z += key == SDLK_u ? -1 : 1;
                    int other_scancode = (key+(SDL_SCANCODE_U-SDLK_u))^(SDL_SCANCODE_U^SDL_SCANCODE_I);
                    if(kbstate[other_scancode]) view->angle.z = 0;
                } break;

                #if 1
                case SDLK_SPACE:
                {
                    if(!input->space_pressed)
                    {
                        uint32_t time = SDL_GetTicks();
                        if(time-input->prev_space_down < 300/*ms*/)
                        {
                            input->flags ^= LAB_VIEWINPUT_NOCLIP;
                            view->velocity = (LAB_Vec3D) {0};
                        }

                        input->space_pressed = true;
                        input->prev_space_down = time;
                    }
                } break;
                #endif
            }
        } break;
        case SDL_KEYUP:
        {
            SDL_Keycode key = ((SDL_KeyboardEvent*)event)->keysym.sym;
            //const Uint8* kbstate = SDL_GetKeyboardState(NULL);

            if(SDLK_0 <= key && key <= SDLK_9)
            {
                if(input->graph_selected)
                {
                    LAB_PerfInfo_Toggle(view->perf_info, key-SDLK_0);
                }
            }

            switch(key)
            {
                case SDLK_SPACE:
                {
                    input->space_pressed = false;
                } break;

                /*case SDLK_w: input->dir_set &= ~1; break;
                case SDLK_a: input->dir_set &= ~2; break;
                case SDLK_s: input->dir_set &= ~4; break;
                case SDLK_d: input->dir_set &= ~8; break;

                case SDLK_SPACE:  input->updown &= ~1; break;
                case SDLK_LSHIFT: input->updown &= ~2; break;*/

                /*case SDLK_o:
                {
                    view->x = LAB_FastFloorF2I(view->x);
                    view->y = LAB_FastFloorF2I(view->y);
                    view->z = LAB_FastFloorF2I(view->z);
                } break;*/
                #if 0
                case SDLK_x:
                {
                    input->flags ^= LAB_VIEWINPUT_DESTROY;
                    input->flags &= ~LAB_VIEWINPUT_CREATE;
                } break;
                case SDLK_c:
                {
                    input->flags ^= LAB_VIEWINPUT_CREATE;
                    input->flags &= ~LAB_VIEWINPUT_DESTROY;
                } break;
                case SDLK_f:
                {
                    /*input->flags ^= LAB_VIEWINPUT_NOCLIP;
                    view->vx = view->vy = view->vz = 0;*/
                } break;
                #endif

                case SDLK_MINUS:
                case SDLK_PLUS:
                {
                    if(key == SDLK_MINUS) view->cfg.render_dist--;
                    if(key == SDLK_PLUS) view->cfg.render_dist++;
                    if(view->cfg.render_dist == 0) view->cfg.render_dist = 1;
                    if(view->cfg.render_dist > 64) view->cfg.render_dist = 64;
                    view->cfg.preload_dist = LAB_PRELOAD_CHUNK(view->cfg.render_dist);
                    view->cfg.keep_dist = LAB_KEEP_CHUNK(view->cfg.render_dist);
                } break;

                case SDLK_n:
                case SDLK_m:
                {
                    /*if(key == SDLK_n) input->speed-=0.02;
                    if(key == SDLK_m) input->speed+=0.02;
                    if(input->speed < 0.02) input->speed = 0.02;
                    if(input->speed > 16)  input->speed = 16;*/
                    if(key == SDLK_n) input->speed *= 0.5;
                    if(key == SDLK_m) input->speed *= 2.0;
                    if(input->speed < 1) input->speed = 1;
                    if(input->speed > 100)  input->speed = 100;
                    SDL_Keymod mods = SDL_GetModState();
                    if(mods & KMOD_CTRL)
                        input->speed = 3.0f;
                } break;


                case SDLK_v:
                case SDLK_b:
                {
                    if(key == SDLK_v) input->brushsize--;
                    if(key == SDLK_b) input->brushsize++;
                    if(input->brushsize <  1) input->brushsize = 1;
                    if(input->brushsize > 16) input->brushsize = 16;
                } break;


                case SDLK_y:
                {
                    input->flags ^= LAB_VIEWINPUT_FORWARD;
                } break;



                case SDLK_ESCAPE:
                {
                    LAB_View_ShowGuiMenu(view);
                    LAB_Input_GrabMouse(input, window, 0);
                } break;

                case SDLK_e:
                {
                    LAB_View_ShowGuiInventory(view, &input->selected_block);
                    LAB_Input_GrabMouse(input, window, 0);
                } break;

                case SDLK_F1:
                {
                    view->cfg.flags ^= LAB_VIEW_SHOW_HUD;
                } break;

                case SDLK_F9:
                {
                    view->cfg.flags ^= LAB_VIEW_SHOW_CHUNK_GRID;
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
                    LAB_DbgPrintf("Saving screenshot to %s\n", fname);
                    IMG_SavePNG(surf_screen, fname);
                    LAB_SDL_FREE(SDL_FreeSurface, &surf_screen);
                } break;

                case SDLK_F3:
                {
                    if(input->graph_selected)
                        input->graph_selected = false;
                    else
                        view->cfg.flags ^= LAB_VIEW_SHOW_FPS_GRAPH;

                } break;

                case SDLK_F4:
                {
                    //view->cfg.flags ^= LAB_VIEW_USE_VBO;
                    //LAB_DbgPrintf("VBO turned %s\n", "off\0on"+4*!!(view->cfg.flags & LAB_VIEW_USE_VBO));
                    //LAB_ViewInvalidateEverything(view, /*free_buffers*/1);

                } break;


                case SDLK_F5:
                {
                    const float delta = 0.125f;
                    SDL_Keymod mods = SDL_GetModState();
                    if(mods & KMOD_CTRL)
                        view->cfg.saturation = 1.f;
                    else if(mods & KMOD_SHIFT)
                        view->cfg.saturation -= delta;
                    else
                        view->cfg.saturation += delta;
                } break;

                case SDLK_F6:
                {
                    const float factor = 1.25f;
                    SDL_Keymod mods = SDL_GetModState();
                    if(mods & KMOD_CTRL)
                        view->cfg.exposure = 1;
                    else if(mods & KMOD_SHIFT)
                        view->cfg.exposure *= 1.0f/factor;
                    else
                        view->cfg.exposure *= factor;
                } break;
                case SDLK_F7:
                {
                    LAB_GameServer_Lock(view->server);
                    view->cfg.flags ^= LAB_VIEW_FLAT_SHADE;
                    LAB_ViewInvalidateEverything(view, /*free_buffers*/0);
                    LAB_GameServer_Unlock(view->server);

                } break;

                case SDLK_F10:
                {
                    LAB_GameServer_Lock(view->server);
                    LAB_View_Clear(view);
                    LAB_GameServer_Unlock(view->server);
                } break;

                case SDLK_F11:
                {
                    #ifdef NDEBUG
                    uint32_t fs_flags;
                    fs_flags = (SDL_GetWindowFlags(window->window) & SDL_WINDOW_FULLSCREEN)
                             ? 0
                             : SDL_WINDOW_FULLSCREEN_DESKTOP;
                    SDL_SetWindowFullscreen(window->window, fs_flags);
                    #else
                    if(SDL_GetWindowFlags(window->window) & SDL_WINDOW_MAXIMIZED)
                        SDL_RestoreWindow(window->window);
                    else
                        SDL_MaximizeWindow(window->window);
                    #endif
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
                    input->selected_block = LAB_item_blocks[id];
                } break;

                default: break;
            }
        } break;


        case SDL_MOUSEMOTION:
        {
            if(input->mouse_grabbed)
            {
                SDL_MouseMotionEvent* mmevent = (SDL_MouseMotionEvent*)event;
                const float speed = 1.f/16.f;
                float mx, my;
                mx = (float)(mmevent->xrel) * speed;
                my = (float)(mmevent->yrel) * speed;
                #if LAB_CLIP_AX
                view->angle.y+=mx;
                view->angle.x+=my;
                LAB_CLAMP_EQ(view->angle.x, -90, 90);
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

            if(input->mouse_grabbed)
            {
                LAB_Input_Interact(input, mbevent->button);
            }
            else
            {
                LAB_Input_GrabMouse(input, window, 1);
            }
        } break;

    }
    return 1;
}

LAB_STATIC int LAB_Input_Interact(LAB_Input* input, int button)
{
    LAB_View* view = input->view;

    LAB_Vec3F vpos = LAB_Vec3D2F_Cast(view->pos);
    LAB_Vec3F dir = LAB_View_GetDirection(view); // view-dir

    LAB_TraceBlock_Result trace;
    if((trace = LAB_TraceBlock(view->world, 10, vpos, dir, LAB_BLOCK_INTERACTABLE)).has_hit)
    {
        switch(button)
        {
            case SDL_BUTTON_LEFT:
            {
                //LAB_WorldServer_Lock(view->server);
                LAB_SetBlock(view->world, trace.hit_block.x, trace.hit_block.y, trace.hit_block.z, LAB_BID_AIR);
                //LAB_WorldServer_Unlock(view->server);
            } break;

            case SDL_BUTTON_RIGHT:
            {
                LAB_Vec3I block_pos = trace.prev_block;

                // check if player is inside and the block should not be set
                if(!(input->flags&LAB_VIEWINPUT_NOCLIP) && (LAB_BlockP(input->selected_block)->flags&LAB_BLOCK_MASSIVE))
                {
                    LAB_Box3F player_box = LAB_Box3F_Add(LAB_PLAYER_BOUNDS, vpos);
                    LAB_Box3F block_box  = { {0, 0, 0}, {1, 1, 1} };
                    block_box = LAB_Box3F_Add(block_box, LAB_Vec3I2F(block_pos));

                    if(LAB_Box3F_Intersects(player_box, block_box)) return 0;
                }

                LAB_SetBlock(view->world, block_pos.x, block_pos.y, block_pos.z, input->selected_block);
            } break;

            default: // SDL_BUTTON_MIDDLE
            {
                LAB_BlockID b = LAB_GetBlock(view->world, trace.hit_block.x, trace.hit_block.y, trace.hit_block.z);
                if(b != LAB_BID_OUTSIDE)
                {
                    input->selected_block = b;
                }
            } break;
        }
        return 1;
    }
    return 0;
}


void LAB_Input_Tick(LAB_Input* input, uint32_t delta_ms)
{

    LAB_View* view = input->view;
    LAB_Vec3D delta = {0};

    //if(view->az > 180) view->az -= 360;
    //view->az *= 0.95;

    if(view->gui_mgr.component == NULL)
    {
        const Uint8* kbstate = SDL_GetKeyboardState(NULL); // simple load
        int mx, my;
        Uint32 mbstate = SDL_GetMouseState(&mx, &my); // simple load

        if(kbstate[SDL_SCANCODE_C])
            view->fov_factor = view->fov_factor*0.95 + 0.3*0.05;
        else
            view->fov_factor = view->fov_factor*0.95 + 1.0*0.05;



        float speed = input->speed * (kbstate[SDL_SCANCODE_LCTRL] ? 2.5 : 1);
        speed *= (float)delta_ms*(1.f/1000.f);


        //unsigned dir_set = input->dir_set;
        unsigned dir_set = (kbstate[SDL_SCANCODE_W]?1:0)
                         | (kbstate[SDL_SCANCODE_A]?2:0)
                         | (kbstate[SDL_SCANCODE_S]?4:0)
                         | (kbstate[SDL_SCANCODE_D]?8:0);
        if(input->flags & LAB_VIEWINPUT_FORWARD)
        {
            if(dir_set&(1|4))
            {
                int bw = (dir_set&1) - !!(dir_set&4);
                LAB_Vec3F dir = LAB_View_GetDirection(view);
                delta = LAB_Vec3D_Add(delta, LAB_Vec3D_RMul(LAB_Vec3F2D(dir), speed*bw));
                if(!(input->flags & LAB_VIEWINPUT_NOCLIP)) view->velocity.y += pow((1+dir.y*bw)*0.5, 0.7)*speed;
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
                float ang_rad = view->angle.y*LAB_PI/180.f - (float)ang8th*LAB_PI*2/8.f;

                float s = sin(ang_rad);
                float c = cos(ang_rad);

                delta.x += s*speed;
                delta.z -= c*speed;
            }


            //if(kbstate[SDL_SCANCODE_SPACE]) dy+=speed;
            if(kbstate[SDL_SCANCODE_SPACE]) {
                if(!(input->flags&LAB_VIEWINPUT_NOCLIP))
                {
                    if(view->on_ground)
                        view->velocity.y = 4.8*(1.4);
                }
                else
                    delta.y+=speed;
                //if(view->vy < 0) view->vy = 0;
            }
            if(kbstate[SDL_SCANCODE_LSHIFT]) delta.y-=speed;
        }

        if(kbstate[SDL_SCANCODE_LALT])
        {
            //for(int i=0; i < 10; ++i)
            //    if(LAB_ViewInputInteract(input, !!(mbstate & SDL_BUTTON(SDL_BUTTON_RIGHT)))) break;
            int button = 0;
            if(mbstate & SDL_BUTTON(SDL_BUTTON_MIDDLE)) button = SDL_BUTTON_MIDDLE;
            if(mbstate & SDL_BUTTON(SDL_BUTTON_RIGHT))  button = SDL_BUTTON_RIGHT;
            if(mbstate & SDL_BUTTON(SDL_BUTTON_LEFT))   button = SDL_BUTTON_LEFT;
            if(button) LAB_Input_Interact(input, button);
        }
    }


    if(input->flags & (LAB_VIEWINPUT_DESTROY|LAB_VIEWINPUT_CREATE))
    {
        LAB_BlockID block = input->flags & LAB_VIEWINPUT_CREATE ? input->selected_block : LAB_BID_AIR;

        LAB_Vec3I bpos = LAB_Pos3D2Block(view->pos);
        int bx, by, bz;
        LAB_Vec3I_Unpack(&bx, &by, &bz, bpos);

        int dist = input->brushsize;
        for(int zz = -dist; zz <= dist; ++zz)
        for(int yy = -dist; yy <= dist; ++yy)
        for(int xx = -dist; xx <= dist; ++xx)
        {
            if((input->flags & LAB_VIEWINPUT_CREATE) && LAB_GetBlock(view->world, bx+xx, by+yy, bz+zz) != LAB_BID_AIR)
                continue;
            if(xx*xx+yy*yy+zz*zz <= dist*dist+dist)
                LAB_SetBlock(view->world, bx+xx, by+yy, bz+zz, block);
        }
    }

    if(input->flags & LAB_VIEWINPUT_NOCLIP)
    {
        const double c = pow(0.95, 0.9*60.f);

        float dt = (float)delta_ms*(1.f/1000.f);
        double m = pow(c, dt);
        view->velocity = LAB_Vec3D_RMul(view->velocity, m);
        view->velocity = LAB_Vec3D_Add(view->velocity,
                             LAB_Vec3D_RMul(delta, (0.5*60)*dt));
        
        view->pos = LAB_Vec3D_Add(view->pos, view->velocity);
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

        delta.y += accel*dt*dt*0.5 + view->velocity.y*dt;
        view->velocity.y += accel*dt;
        //if(dy > 0) view->vy = dy;

        // TODO move this collision physics to the world itself
        // clipping
        const float maxstep = 0.0625f;
        float fsteps = ceilf(LAB_Vec3D_GetMax(LAB_Vec3D_Abs(delta))/maxstep + 1);
        LAB_ASSUME(fsteps<INT_MAX);
        int steps = (int)fsteps;

        LAB_Vec3D ddelta = LAB_Vec3D_Div(delta, (float)steps);
        LAB_ASSUME(ddelta.x <= maxstep);
        LAB_ASSUME(ddelta.y <= maxstep);
        LAB_ASSUME(ddelta.z <= maxstep);

        //LAB_DbgPrintf("steps=%i, %f|%f  %f|%f  %f|%f\n", steps, dx, ddx, dy, ddy, dz, ddz);

        for(int step = 0; step < steps; ++step)
        {
            view->pos = LAB_Vec3D_Add(view->pos, ddelta);
            //LAB_DbgPrintf("    %04i: %f %f %f\n", step, view->x, view->y, view->z);

            LAB_Vec3I block_pos = LAB_Pos3D2Block(view->pos);
            LAB_Vec3D frac_pos = LAB_Vec3D_Sub(view->pos, LAB_Vec3I2D(block_pos));

            int bx, by, bz;
            LAB_Vec3I_Unpack(&bx, &by, &bz, block_pos);

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

            view->on_ground = false;
            for(int i = 0; i < 2; ++i)
            //for(int yy = -2; yy <= 1; ++yy)
            {
                int yy = i ? 1 : -2;
                for(int xx = -1; xx <= 1; ++xx)
                for(int zz = -1; zz <= 1; ++zz)
                {
                    LAB_Block* block  = LAB_GetBlockP_FromMainThread(view->world, bx+xx, by+yy, bz+zz);

                    if(xx!=0||zz!=0)
                    {
                        // Blocks on the same axis
                        LAB_Block* b1 = LAB_GetBlockP_FromMainThread(view->world, bx+xx, by+0,  bz   );
                        LAB_Block* b2 = LAB_GetBlockP_FromMainThread(view->world, bx+xx, by-1,  bz   );
                        LAB_Block* b3 = LAB_GetBlockP_FromMainThread(view->world, bx,    by+0,  bz+zz);
                        LAB_Block* b4 = LAB_GetBlockP_FromMainThread(view->world, bx,    by-1,  bz+zz);
                        if(b1->flags&LAB_BLOCK_MASSIVE || b2->flags&LAB_BLOCK_MASSIVE) continue;
                        if(b3->flags&LAB_BLOCK_MASSIVE || b4->flags&LAB_BLOCK_MASSIVE) continue;

                        // Blocks possibly in the corners
                        LAB_Block* b5 = LAB_GetBlockP_FromMainThread(view->world, bx+xx, by+0,  bz+zz);
                        LAB_Block* b6 = LAB_GetBlockP_FromMainThread(view->world, bx+xx, by-1,  bz+zz);
                        if(b5->flags&LAB_BLOCK_MASSIVE || b6->flags&LAB_BLOCK_MASSIVE) continue;
                    }

                    if(block->flags&LAB_BLOCK_MASSIVE)
                    {
                        int collides = 1;
                        collides &= (xx < 0 && frac_pos.x < 0.2) || (xx > 0 && frac_pos.x > 0.8) || xx == 0;
                        collides &= (zz < 0 && frac_pos.z < 0.2) || (zz > 0 && frac_pos.z > 0.8) || zz == 0;
                        collides &= (yy <-1 && frac_pos.y < 0.4 && frac_pos.y > 0.3) || (yy > 0 && frac_pos.y > 0.8 && frac_pos.y < 0.9) || yy ==-1 || yy == 0;

                        if(collides)
                        {
                            /**/ if(yy < 0) { view->pos.y = by+0.4+d; view->velocity.y = LAB_MAX(view->velocity.y, 0); view->on_ground = true; }
                            else if(yy > 0) { view->pos.y = by+0.8-d; view->velocity.y = LAB_MIN(view->velocity.y, 0); }

                            frac_pos = LAB_Vec3D_Sub(view->pos, LAB_Vec3I2D(block_pos));
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
                    LAB_Block* block = LAB_GetBlockP_FromMainThread(view->world, bx+xx, by+yy, bz+zz);
                    if(block->flags&LAB_BLOCK_MASSIVE)
                    {
                        int collides = 1;
                        collides &= (xx < 0 && frac_pos.x < 0.2) || (xx > 0 && frac_pos.x > 0.8) || xx == 0;
                        collides &= (zz < 0 && frac_pos.z < 0.2) || (zz > 0 && frac_pos.z > 0.8) || zz == 0;
                        collides &= (yy <-1 && frac_pos.y < 0.4) || (yy > 0 && frac_pos.y > 0.8) || yy ==-1 || yy == 0;


                        if(collides)
                        {
                            if(fabs(frac_pos.x-0.5f) < fabs(frac_pos.z-0.5f))
                            //if(abs(xx) > abs(zz))
                            //if(0)
                            {
                              /**/ if(xx < 0) { view->pos.x = bx+0.2+d; view->velocity.x = 0; }
                              else if(xx > 0) { view->pos.x = bx+0.8-d; view->velocity.x = 0; }
                              else if(zz < 0) { view->pos.z = bz+0.2+d; view->velocity.z = 0; }
                              else if(zz > 0) { view->pos.z = bz+0.8-d; view->velocity.z = 0; }
                            }
                            else
                            {
                                /**/ if(zz < 0) { view->pos.z = bz+0.2+d; view->velocity.z = 0; }
                                else if(zz > 0) { view->pos.z = bz+0.8-d; view->velocity.z = 0; }
                                else if(xx < 0) { view->pos.x = bx+0.2+d; view->velocity.x = 0; }
                                else if(xx > 0) { view->pos.x = bx+0.8-d; view->velocity.x = 0; }
                            }
                            //if(yy < 0) view->y = by+0.5;
                            //if(yy > 0) view->y = by+0.5;
                            frac_pos = LAB_Vec3D_Sub(view->pos, LAB_Vec3I2D(block_pos));
                            f = 1;
                        }
                    }

                }
                if(f && i == 3) break;
            }
        }
    }
}
