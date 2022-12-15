#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>

Uint8 * RAM;
Uint8 * V;
Uint16 I;
Uint16 PC;
Uint16 SP;
Uint16 * stack;
Uint16 DT;
Uint16 ST;
bool * keys;


int main(int argc, char *argv[]){

    srand(time(0));

    RAM = (Uint8*)calloc( 4096 , sizeof(Uint8) );
    V = (Uint8*)calloc( 16 , sizeof(Uint8) );
    stack = (Uint16*)calloc( 16 , sizeof(Uint16) );
    keys = (bool*)calloc(16, sizeof(bool));

    PC = 512;
    I = 0;
    DT = 0;
    ST = 0;
    SP = 0;

    Uint8 s0[] = { 0xF0, 0x90, 0x90, 0x90, 0xF0 };
    Uint8 s1[] = { 0x20, 0x60, 0x20, 0x20, 0x70 };
    Uint8 s2[] = { 0xF0, 0x10, 0xF0, 0x80, 0xF0 };
    Uint8 s3[] = { 0xF0, 0x10, 0xF0, 0x10, 0xF0 };
    Uint8 s4[] = { 0x90, 0x90, 0xF0, 0x10, 0x10 };
    Uint8 s5[] = { 0xF0, 0x80, 0xF0, 0x10, 0xF0 };
    Uint8 s6[] = { 0xF0, 0x80, 0xF0, 0x90, 0xF0 };
    Uint8 s7[] = { 0xF0, 0x10, 0x20, 0x40, 0x40 };
    Uint8 s8[] = { 0xF0, 0x90, 0xF0, 0x90, 0xF0 };
    Uint8 s9[] = { 0xF0, 0x90, 0xF0, 0x10, 0xF0 };
    Uint8 sa[] = { 0xF0, 0x90, 0xF0, 0x90, 0x90 };
    Uint8 sb[] = { 0xE0, 0x90, 0xE0, 0x90, 0xE0 };
    Uint8 sc[] = { 0xF0, 0x80, 0x80, 0x80, 0xF0 };
    Uint8 sd[] = { 0xE0, 0x90, 0x90, 0x90, 0xE0 };
    Uint8 se[] = { 0xF0, 0x80, 0xF0, 0x80, 0xF0 };
    Uint8 sf[] = { 0xF0, 0x80, 0xF0, 0x80, 0x80 };
    memcpy(RAM, &s0, 5);
    memcpy(RAM + 5, &s1, 5);
    memcpy(RAM + 10, &s2, 5);
    memcpy(RAM + 15, &s3, 5);
    memcpy(RAM + 20, &s4, 5);
    memcpy(RAM + 25, &s5, 5);
    memcpy(RAM + 30, &s6, 5);
    memcpy(RAM + 35, &s7, 5);
    memcpy(RAM + 40, &s8, 5);
    memcpy(RAM + 45, &s9, 5);
    memcpy(RAM + 50, &sa, 5);
    memcpy(RAM + 55, &sb, 5);
    memcpy(RAM + 60, &sc, 5);
    memcpy(RAM + 65, &sd, 5);
    memcpy(RAM + 70, &se, 5);
    memcpy(RAM + 75, &sf, 5);

    FILE * file;
    if (argc == 2) file = fopen(argv[1], "rb");
    else return 0;

    fseek(file, 0, SEEK_END);
    int fsize = ftell(file);
    rewind(file);

    fread(RAM + 512, sizeof(Uint8), fsize, file);

    fclose(file);

    int i;
    /*for (i = 512; i < 512+fsize; i++){
         if ( i % 2 == 0) printf("%d ", i);
        printf("%x ", RAM[i]);
        if ( i % 2 == 1) printf("\n");
    }*/

	int scale = 10;
    int ww = 64 * scale;
    int wh = 32 * scale;

    SDL_Init(SDL_INIT_EVERYTHING);
    Mix_Init(MIX_INIT_MP3);
	SDL_Window *window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ww, wh, SDL_WINDOW_SHOWN);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Event event;

    Uint8 screen[32][64];
    int  j;
    for (i = 0; i < 32; i++){
        for (j = 0; j < 64; j++){
            screen[i][j] = 0;
        }
    }
    int fpsfac = 8;
    int timer = 1;
    int FPS=60 * fpsfac;
    int frameDelay=1000/FPS;
    Uint32 frameStart;
    int frameTime;

    SDL_Rect rekt;
    rekt.h = scale;
    rekt.w = scale;

    Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,4096);
    Mix_Music * MYMUSIC = Mix_LoadMUS("beep.mp3");
    Mix_VolumeMusic(64);
    Mix_PlayMusic(MYMUSIC,-1);
    Mix_PauseMusic();

    bool playing = false;
    bool loop = true;
    while (loop){
        frameStart = SDL_GetTicks();

        while( SDL_PollEvent(&event)!=0 ){
            if (event.type == SDL_QUIT) loop = false;
        }

        if (timer == fpsfac){
            if (ST > 0)ST--;
            else if (playing) {
                playing = false;
                Mix_PauseMusic();
            }
            if (DT > 0)DT--;
            timer = 0;
        }
        timer++;

        const Uint8 *keystate = SDL_GetKeyboardState(NULL);
        int i;
        for (i = 0; i< 16; i++)keys[i] = false;
        if (keystate[SDL_SCANCODE_X] ) keys[0] = true;
        if (keystate[SDL_SCANCODE_1] ) keys[1] = true;
        if (keystate[SDL_SCANCODE_2] ) keys[2] = true;
        if (keystate[SDL_SCANCODE_3] ) keys[3] = true;
        if (keystate[SDL_SCANCODE_Q] ) keys[4] = true;
        if (keystate[SDL_SCANCODE_W] ) keys[5] = true;
        if (keystate[SDL_SCANCODE_E] ) keys[6] = true;
        if (keystate[SDL_SCANCODE_A] ) keys[7] = true;
        if (keystate[SDL_SCANCODE_S] ) keys[8] = true;
        if (keystate[SDL_SCANCODE_D] ) keys[9] = true;
        if (keystate[SDL_SCANCODE_Z] ) keys[10] = true;
        if (keystate[SDL_SCANCODE_C] ) keys[11] = true;
        if (keystate[SDL_SCANCODE_4] ) keys[12] = true;
        if (keystate[SDL_SCANCODE_R] ) keys[13] = true;
        if (keystate[SDL_SCANCODE_F] ) keys[14] = true;
        if (keystate[SDL_SCANCODE_V] ) keys[15] = true;

        //printf("%d  %x %x\n", PC, RAM[PC], RAM[PC+1]);

        if ( RAM[PC] == 0x00 && RAM[PC+1] == 0xE0 ){
            for (i = 0; i < 32; i++){
                for (j = 0; j < 64; j++){
                    screen[i][j] = 0;
                }
            }
        }
        else if ( RAM[PC] == 0x00 && RAM[PC+1] == 0xEE ){
            PC = stack[SP-1] ;
            SP--;
        }
        else if ( RAM[PC] >> 4 == 0x01 ) PC = ((RAM[PC] & 0x0F) << 8 ) + RAM[PC+1] - 2;
        else if ( RAM[PC] >> 4 == 0x02 ) {
            stack[SP] = PC;
            SP++;
            PC = ((RAM[PC] & 0x0F) << 8 ) + RAM[PC+1] - 2;
        }
        else if ( RAM[PC] >> 4 == 0x03 ) { if ( V[RAM[PC] & 0x0F] == RAM[PC+1] ) PC += 2; }
        else if ( RAM[PC] >> 4 == 0x04 ) { if ( V[RAM[PC] & 0x0F] != RAM[PC+1] ) PC += 2; }
        else if ( RAM[PC] >> 4 == 0x05 ) { if ( V[RAM[PC] & 0x0F] == V[(RAM[PC+1] >> 4)] ) PC += 2; }
        else if ( RAM[PC] >> 4 == 0x06 ) V[RAM[PC] & 0x0F] = RAM[PC+1];
        else if ( RAM[PC] >> 4 == 0x07 ) V[RAM[PC] & 0x0F] += RAM[PC+1];
        else if ( RAM[PC] >> 4 == 0x08 && (RAM[PC+1] & 0x0F) == 0) V[RAM[PC] & 0x0F] = V[RAM[PC+1] >> 4];
        else if ( RAM[PC] >> 4 == 0x08 && (RAM[PC+1] & 0x0F) == 1) V[RAM[PC] & 0x0F] = V[RAM[PC+1] >> 4] | V[RAM[PC] & 0x0F];
        else if ( RAM[PC] >> 4 == 0x08 && (RAM[PC+1] & 0x0F) == 2) V[RAM[PC] & 0x0F] = V[RAM[PC+1] >> 4] & V[RAM[PC] & 0x0F];
        else if ( RAM[PC] >> 4 == 0x08 && (RAM[PC+1] & 0x0F) == 3) V[RAM[PC] & 0x0F] = V[RAM[PC+1] >> 4] ^ V[RAM[PC] & 0x0F];
        else if ( RAM[PC] >> 4 == 0x08 && (RAM[PC+1] & 0x0F) == 4) {
            if ( V[RAM[PC] & 0x0F] + V[RAM[PC+1] >> 4] > 255 ) V[0xF] = 1;
            else V[0xF] = 0;
            V[RAM[PC] & 0x0F] = V[RAM[PC] & 0x0F] + V[RAM[PC+1] >> 4];
        }
        else if ( RAM[PC] >> 4 == 0x08 &&  (RAM[PC+1] & 0x0F) == 5) {
            if ( V[RAM[PC] & 0x0F] > V[RAM[PC+1] >> 4] ) V[0xF] = 1;
            else V[0xF] = 0;
            V[RAM[PC] & 0x0F] -= V[RAM[PC+1] >> 4];
        }
        else if ( RAM[PC] >> 4 == 0x08 &&  (RAM[PC+1] & 0x0F) == 6) {
            if ( V[RAM[PC] & 0x0F] % 2 == 1) V[0xF] = 1;
            else V[0xF] = 0;
            V[RAM[PC] & 0x0F] = V[RAM[PC] & 0x0F] >> 1;
        }
        else if ( RAM[PC] >> 4 == 0x08 &&  (RAM[PC+1] & 0x0F) == 7) {
            if ( V[RAM[PC+1] >> 4] > V[RAM[PC] & 0x0F] ) V[0xF] = 1;
            else V[0xF] = 0;
            V[RAM[PC] & 0x0F] = V[RAM[PC+1] >> 4] - V[RAM[PC] & 0x0F];
        }
        else if ( RAM[PC] >> 4 == 0x08 &&  (RAM[PC+1] & 0x0F) == 0x0E) {
            if ( V[RAM[PC] & 0x0F] > 127) V[0xF] = 1;
            else V[0xF] = 0;
            V[RAM[PC] & 0x0F] = V[RAM[PC] & 0x0F] << 1;
        }
        else if ( RAM[PC] >> 4 == 0x09 ) { if ( V[RAM[PC] & 0x0F] != V[RAM[PC+1] >> 4] ) PC += 2; }
        else if ( RAM[PC] >> 4 == 0x0A ) I = ((RAM[PC] & 0x0F) << 8 ) + RAM[PC+1];
        else if ( RAM[PC] >> 4 == 0x0B ) PC = ((RAM[PC] & 0x0F) << 8 ) + RAM[PC+1] + V[0] - 2;
        else if ( RAM[PC] >> 4 == 0x0C ) V[RAM[PC] & 0x0F] = (rand()%256) & RAM[PC+1];
        else if ( RAM[PC] >> 4 == 0x0D ) {

            int bef, af;
            V[0x0F] = 0;
            for (i = 0; i < (RAM[PC+1] & 0x0F); i++ ){
                for (j = 0; j < 8; j++){
                    bef = screen[(V[RAM[PC+1] >> 4] + i)%32][(V[RAM[PC] & 0x0F] + j)%64];
                    screen[(V[RAM[PC+1] >> 4] + i)%32][(V[RAM[PC] & 0x0F] + j)%64] = screen[(V[RAM[PC+1] >> 4] + i)%32][(V[RAM[PC] & 0x0F] + j)%64] ^ ((RAM[I+i] >> 7-j)%2);
                    af = screen[(V[RAM[PC+1] >> 4] + i)%32][(V[RAM[PC] & 0x0F] + j)%64];
                    if (bef - af == 1) V[0x0F] = 1;
                }
            }
        }
        else if ( RAM[PC] >> 4 == 0x0E && RAM[PC+1] == 0x9E ) { if ( keys[ V[RAM[PC] & 0x0F] ]   ) PC += 2; }
        else if ( RAM[PC] >> 4 == 0x0E && RAM[PC+1] == 0xA1 ) { if ( !keys[ V[RAM[PC] & 0x0F] ]   ) PC += 2; }
        else if ( RAM[PC] >> 4 == 0x0F && RAM[PC+1] == 0x07 ) V[RAM[PC] & 0x0F] = DT;
        else if ( RAM[PC] >> 4 == 0x0F && RAM[PC+1] == 0x0A ) {
            bool brk = false;
            while (true){
                while( SDL_PollEvent(&event)!=0 ){
                    if (event.type == SDL_QUIT){
                        loop = false;
                        brk = true;
                        break;
                    }
                    if (event.type==SDL_KEYDOWN) {
                        switch (event.key.keysym.sym){
                            case SDLK_x : V[RAM[PC] & 0x0F] = 0; brk = true; break;
                            case SDLK_1 : V[RAM[PC] & 0x0F] = 1; brk = true; break;
                            case SDLK_2 : V[RAM[PC] & 0x0F] = 2; brk = true; break;
                            case SDLK_3 : V[RAM[PC] & 0x0F] = 3; brk = true; break;
                            case SDLK_q : V[RAM[PC] & 0x0F] = 4; brk = true; break;
                            case SDLK_w : V[RAM[PC] & 0x0F] = 5; brk = true; break;
                            case SDLK_e : V[RAM[PC] & 0x0F] = 6; brk = true; break;
                            case SDLK_a : V[RAM[PC] & 0x0F] = 7; brk = true; break;
                            case SDLK_s : V[RAM[PC] & 0x0F] = 8; brk = true; break;
                            case SDLK_d : V[RAM[PC] & 0x0F] = 9; brk = true; break;
                            case SDLK_z : V[RAM[PC] & 0x0F] = 10; brk = true; break;
                            case SDLK_c : V[RAM[PC] & 0x0F] = 11; brk = true; break;
                            case SDLK_4 : V[RAM[PC] & 0x0F] = 12; brk = true; break;
                            case SDLK_r : V[RAM[PC] & 0x0F] = 13; brk = true; break;
                            case SDLK_f : V[RAM[PC] & 0x0F] = 14; brk = true; break;
                            case SDLK_v : V[RAM[PC] & 0x0F] = 15; brk = true; break;
                        }
                    }
                    if (brk) break;
                }
                if (brk) break;
            }
        }
        else if ( RAM[PC] >> 4 == 0x0F && RAM[PC+1] == 0x15 ) DT = V[RAM[PC] & 0x0F];
        else if ( RAM[PC] >> 4 == 0x0F && RAM[PC+1] == 0x18 ) ST = V[RAM[PC] & 0x0F];
        else if ( RAM[PC] >> 4 == 0x0F && RAM[PC+1] == 0x1E ) I += V[RAM[PC] & 0x0F];
        else if ( RAM[PC] >> 4 == 0x0F && RAM[PC+1] == 0x29 ) I = V[RAM[PC] & 0x0F] * 5;
        else if ( RAM[PC] >> 4 == 0x0F && RAM[PC+1] == 0x33 ) {
            RAM[I] = V[RAM[PC] & 0x0F] / 100;
            RAM[I+1] = (V[RAM[PC] & 0x0F] - RAM[I] * 100 ) / 10;
            RAM[I+2] = (V[RAM[PC] & 0x0F] - RAM[I] * 100 - RAM[I+1] * 10);
        }
        else if ( RAM[PC] >> 4 == 0x0F && RAM[PC+1] == 0x55 ) {
            for (i = 0; i < (RAM[PC] & 0x0F) + 1; i++){
                RAM[I+i] = V[i];
            }
        }
        else if ( RAM[PC] >> 4 == 0x0F && RAM[PC+1] == 0x65 ) {
            for (i = 0; i < (RAM[PC] & 0x0F) + 1; i++){
                V[i] = RAM[I + i];
            }
        }
        /*else {
            printf("%x %x \n", RAM[PC], RAM[PC+1]);
        }*/

        PC += 2;

        if (ST > 0 && !playing) {
            playing = true;
            Mix_ResumeMusic();
        }

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0);

        for (i = 0; i < 32; i++){
            for (j = 0; j < 64; j++){
                if (screen[i][j] == 1) SDL_SetRenderDrawColor( renderer, 255, 255, 255, 0);
                else SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0);
                rekt.x = j * scale;
                rekt.y = i * scale;
                SDL_RenderFillRect(renderer, &rekt);
            }
        }
        SDL_RenderPresent(renderer);

        frameTime = SDL_GetTicks()-frameStart;
        if (frameDelay > frameTime){
            SDL_Delay(frameDelay-frameTime);
        }
    }
    Mix_Quit();
    SDL_Quit();

    return 0;
}
