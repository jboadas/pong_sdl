#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

using namespace std;
#define PI 3.14159265358979323846
#define MAX_KEYBOARD_KEYS 350

int WIDTH = 800;
int HEIGHT = 600;
int FONT_SIZE = 32;
int BALL_SIZE = 16;
int BALL_SPEED = 10;
int SPEED = 9;
int R_SPEED = 4;
int keyboard[MAX_KEYBOARD_KEYS];

SDL_Renderer* renderer;
SDL_Window* window;
TTF_Font* font;
SDL_Color color;
SDL_Rect l_paddle, r_paddle, ball, score_board;
float vel_x, vel_y;
std::string score = "SCORE";
int l_s, r_s;
bool turn;
int rendererFlags, windowFlags;

void serve(){
    l_paddle.y = r_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
    if (turn){
        ball.x = l_paddle.x + (l_paddle.w * 4);
        vel_x = BALL_SPEED;
    }else{
        ball.x = r_paddle.x - (r_paddle.w * 4);
        vel_x = -BALL_SPEED;
    }
    ball.y = HEIGHT / 2 - (BALL_SIZE /2);
    turn = !turn;

}

void write(std::string text, int x, int y){
    SDL_Surface* surface;
    SDL_Texture* texture;
    const char* t = text.c_str();
    surface = TTF_RenderText_Solid(font, t, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    score_board.w = surface->w;
    score_board.h = surface->h;
    score_board.x = x - score_board.w;
    score_board.y = y - score_board.h;
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &score_board);
    SDL_DestroyTexture(texture);
}

static void movePaddle(void){
    int dy = 0;
    if (keyboard[SDL_SCANCODE_W]){
        dy = -SPEED;
    }
    if (keyboard[SDL_SCANCODE_S]){
        dy = SPEED;
    }
    if (keyboard[SDL_SCANCODE_ESCAPE]){
        SDL_Quit();
        exit(0);
    }
    l_paddle.y += dy;
}

void update(){
    movePaddle();
    score = std::to_string(l_s) + "    " + std::to_string(r_s);
    if(l_paddle.y <= 0){
        l_paddle.y = 0;
    }
    if(l_paddle.y + l_paddle.h >= HEIGHT){
        l_paddle.y = HEIGHT - l_paddle.h;
    }
    if(ball.y < r_paddle.y + (r_paddle.h/2)){
        r_paddle.y -= R_SPEED;
        if(r_paddle.y <= 0){
            r_paddle.y = 0;
        }
    }
    if(ball.y > r_paddle.y + (r_paddle.h/2)){
        r_paddle.y += R_SPEED;
        if(r_paddle.y + r_paddle.h >= HEIGHT){
            r_paddle.y = HEIGHT - r_paddle.h;
        }
    }
    if(ball.x <= 0){
        vel_x = - vel_x;
        r_s += 1;
        serve();
    }
    if(ball.x + ball.w >= WIDTH){
        vel_x = - vel_x;
        l_s +=1;
        serve();
    }
    if((ball.y <= 0) || (ball.y + ball.h > HEIGHT)){
        vel_y = - vel_y;
    }
    if(SDL_HasIntersection(&ball, &l_paddle)){
        double rel = (l_paddle.y + (l_paddle.h/2)) - (ball.y + (BALL_SIZE/2));
        double norm = rel/(l_paddle.h/2);
        double bounce = norm * (5*PI/12);
        vel_x = BALL_SPEED * cos(bounce);
        vel_y = BALL_SPEED * sin(bounce);
    }
    if(SDL_HasIntersection(&ball, &r_paddle)){
        double rel = (r_paddle.y + (r_paddle.h/2)) - (ball.y + (BALL_SIZE/2));
        double norm = rel/(r_paddle.h/2);
        double bounce = norm * (5*PI/12);
        vel_x = -BALL_SPEED * cos(bounce);
        vel_y = -BALL_SPEED * sin(bounce);
    }
    ball.x += vel_x;
    ball.y += vel_y;
}

void render(){
    SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xFF);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 0xFF);
    SDL_RenderFillRect(renderer, &l_paddle);
    SDL_RenderFillRect(renderer, &r_paddle);
    SDL_RenderFillRect(renderer, &ball);
    write(score, WIDTH/2 + FONT_SIZE, FONT_SIZE * 2);
    SDL_RenderPresent(renderer);
}

void doKeyUp(SDL_KeyboardEvent *event){
    if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS){
        keyboard[event->keysym.scancode] = 0;
    }
}

void doKeyDown(SDL_KeyboardEvent *event){
    if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS){
        keyboard[event->keysym.scancode] = 1;
    }
}

void input(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch (event.type){
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_KEYDOWN:
                doKeyDown(&event.key);
                break;
            case SDL_KEYUP:
                doKeyUp(&event.key);
                break;
            default:
                break;
        }
    }
}

static void capFrameRate(Uint32 *lastTime, float *remainder)
{
    long wait, frameTime;
    wait = 16 + *remainder;
    *remainder -= (int)*remainder;
    frameTime = SDL_GetTicks() - *lastTime;
    wait -= frameTime;
    if (wait < 1){
        wait = 1;
    }
    SDL_Delay(wait);
    *remainder += 0.667;
    *lastTime = SDL_GetTicks();
}


int main()
{
    if(SDL_Init(SDL_INIT_EVERYTHING < 0)){
        cout << "Failed SDL_Init" << endl;
        cout << SDL_GetError() << endl;
        SDL_Quit();
    }

    windowFlags = 0;
    window = SDL_CreateWindow(
                "Shooter 01",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                WIDTH,
                HEIGHT,
                windowFlags);

    if (!window)
    {
        cout << "Failed to Create Window" << endl;
        cout << SDL_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    rendererFlags = SDL_RENDERER_ACCELERATED;
    renderer = SDL_CreateRenderer(window, -1, rendererFlags);
    if (!renderer)
    {
        cout << "Failed to Create renderer" << endl;
        cout << SDL_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    TTF_Init();
    if(TTF_Init() == -1) {
        cout << "Error TTF init" << endl;
        cout << TTF_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    font = TTF_OpenFont("fonts/Peepo.ttf", FONT_SIZE);
    if( font == NULL )
    {
        cout << "Failed to load Font" << endl;
        cout << TTF_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    SDL_RendererInfo drinfo;
    SDL_GetRenderDriverInfo (0, &drinfo);
    cout << drinfo.flags << endl;
    cout << drinfo.max_texture_height << endl;
    cout << drinfo.max_texture_width << endl;
    cout << drinfo.num_texture_formats << endl;
    cout << drinfo.texture_formats << endl;
    cout << drinfo.name << endl;


    color.r = color.g = color.b = 255;
    l_s = r_s = 0;
    l_paddle.x = 32;
    l_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
    l_paddle.h = HEIGHT / 4;
    l_paddle.w = 12;
    r_paddle = l_paddle;
    r_paddle.x = WIDTH - r_paddle.w - 32;
    ball.w = ball.h = BALL_SIZE;
    serve();

    Uint32 lastTicks = SDL_GetTicks();
    float remainder = 0;
    while(!SDL_QuitRequested()){
        input();
        update();
        render();
        capFrameRate(&lastTicks, &remainder);
    }
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(0);
    return 0;
}
