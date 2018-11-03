/* 
** NOME: Sérgio Nóbrega
** NÚMERO: 86806
** - Projeto Intermédio -
** O objetivo do projeto é elaborar o jogo 2048 em C usando SDL2.
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define STRING_SIZE 100       // max size for some strings
#define TABLE_SIZE 650        // main game space size
#define LEFT_BAR_SIZE 150     // left white bar size
#define WINDOW_POSX 200       // initial position of the window: x
#define WINDOW_POSY 200       // initial position of the window: y
#define SQUARE_SEPARATOR 8    // square separator in px
#define BOARD_SIZE_PER 0.7f   // board size in % wrt to table size 
#define MAX_BOARD_POS 11      // maximum size of the board
#define MIN_BOARD_POS 2
#define MAX_LEVELS 23
#define MIN_LEVELS 4
#define MAX_NAME 8
#define MARGIN 5


// declaration of the functions related to graphical issues
void InitEverything(int , int , TTF_Font **, SDL_Surface **, SDL_Window ** , SDL_Renderer **, TTF_Font **);
void InitSDL();
void InitFont();
SDL_Window* CreateWindow(int , int );
SDL_Renderer* CreateRenderer(int , int , SDL_Window *);
int RenderText(int, int, const char *, TTF_Font *, SDL_Color *, SDL_Renderer *);
int RenderLogo(int, int, SDL_Surface *, SDL_Renderer *);
void RenderTable(int, int *, int *, TTF_Font *, SDL_Surface **, SDL_Renderer *);
void RenderBoard(int [][MAX_BOARD_POS], SDL_Surface **, int, int, int, SDL_Renderer *);
void RenderStats( SDL_Renderer *, int , int , int, TTF_Font *_font);
//lose and win function for rectangles
void RenderVictory(SDL_Renderer *_renderer, TTF_Font *_font, int, int);
void LoadValues(SDL_Surface **);
void UnLoadValues(SDL_Surface **);

// mechanic functions 
void parametros(int *_dim, int *dif, char name[MAX_NAME], int board[MAX_BOARD_POS][MAX_BOARD_POS]);
void slideup(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *det);
void slidedown(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *det);
void slideleft(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *det);
void slideright(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *det);
void randompiece(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int detect, int detectmerge);
void clearmatrix(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS]);
void mergeup(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *_score, int *_detectmerge);
void mergedown(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *_score, int *_detectmerge);
void mergeleft(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *_score, int *_detectmerge);
void mergeright(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *_score, int *_detectmerge);
void losewin(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int dif, int *v, int *l, int *game);
void copymatrix (int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int boardcopy[MAX_BOARD_POS][MAX_BOARD_POS], int _score, int *_undoscore);
void undo (int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int boardcopy[MAX_BOARD_POS][MAX_BOARD_POS], int *_score, int _undoscore);
void statistics(int score, int time, int level, char name[MAX_NAME]);
void highestvalue(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *high);
void timecount(time_t beginoftime, int *time_variable, int, int, int, int timesave);

//advanced
void advancecopy(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int _score, int _time_variable, int _dif, char name[MAX_NAME]);
void advanceopen(int *board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *_score, int *_time_variable, int *_dif, char name[MAX_NAME], int *timeready);
void oldornew(int *board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], char name[MAX_NAME], int *dif, int *timeready, int *timesave, int *score);

// definition of some strings: they cannot be changed when the program is executed !
const char myName[] = "Sergio Nobrega";
const char myNumber[] = "IST186806";

/**
 * main function: entry point of the program
 * only to invoke other functions !
 */
int main( int argc, char* args[] )
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *serif = NULL;
    TTF_Font *opensans = NULL;
    SDL_Surface *array_of_numbers[MAX_LEVELS], *imgs[2];
    SDL_Event event;
    int delay = 300;
    int quit = 0;
    int width = (TABLE_SIZE + LEFT_BAR_SIZE);
    int height = TABLE_SIZE;
    int square_size_px, board_size_px, board_pos = 5;
    int board[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}};
    int boardcopy[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}};
    int dif = 10;
    char name[MAX_NAME];
    int detect = 0, out = 0, detectmerge = 0;
    int score = 0, timea = 0, level = 0;
    int v = 0, l = 0;
    int game = 1, timeready = 0, timesave = 0;
    time_t begin = time(&begin);
    int undoscore;
    
    oldornew(&board_pos, board, name, &dif, &timeready, &timesave, &score);

    // initialize graphics
    InitEverything(width, height, &serif, imgs, &window, &renderer, &opensans);
    // loads numbers as images
    LoadValues(array_of_numbers);

    while( quit == 0 )
    {
        // while there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            if( event.type == SDL_QUIT )
            {
                // quit !
                quit = 1;
            }

            else if ( event.type == SDL_KEYDOWN )
            {  
                 
                switch ( event.key.keysym.sym )
                {
                    case SDLK_UP:
                        if(game==1)
                        {
                            //Copiar a matriz
                            copymatrix(board_pos, board, boardcopy, score, &undoscore);
                            //First slide (eg [2, 2, 2, 2])
                            slideup(board_pos, board, &detect);
                            //The pieces that can merge, do so (eg [0, 4, 0, 4])
                            mergeup(board_pos, board, &score, &detectmerge);
                            //Then the other pieces slide again (eg [ 0, 0, 4, 4])
                            //&out is a variable that is not being used
                            slideup(board_pos, board, &out);
                            //After the movements generate a new piece
                            randompiece(board_pos, board, detect, detectmerge);
                            //Copiar para ficheiro (func. avançada)

                        }

                        break;
                    case SDLK_DOWN:
                    if(game==1)
                        {
                            copymatrix(board_pos, board, boardcopy, score, &undoscore);
                            slidedown(board_pos, board, &detect);
                            mergedown(board_pos, board, &score, &detectmerge);
                            slidedown(board_pos, board, &out);
                            randompiece(board_pos, board, detect, detectmerge);

                        }

                        break;
                    case SDLK_LEFT:
                        if(game==1)
                        {
                            copymatrix(board_pos, board, boardcopy, score, &undoscore);
                            slideleft(board_pos, board, &detect);
                            mergeleft(board_pos, board, &score, &detectmerge);
                            slideleft(board_pos, board, &out);
                            randompiece(board_pos, board, detect, detectmerge);

                        }
                        break;
                    case SDLK_RIGHT:
                    if(game==1)
                        {
                            copymatrix(board_pos, board, boardcopy, score, &undoscore);
                            slideright(board_pos, board, &detect);
                            mergeright(board_pos, board, &score, &detectmerge);
                            slideright(board_pos, board, &out);
                            randompiece(board_pos, board, detect, detectmerge);

                        }

                        break;
                    case SDLK_q:
                        //quit !
                        quit = 1;
                        break;
                    case SDLK_n:
                        //new game
                        detect = 1;
                        detectmerge = 1;
                        clearmatrix(board_pos, board);
                        randompiece(board_pos, board, detect, detectmerge);
                        randompiece(board_pos, board, detect, detectmerge);
                        v = 0;
                        l = 0;
                        score = 0;
                        game = 1;
                        timeready = 1;
                        begin = time(&begin);
                        break;
                    case SDLK_u:    
                        //undo
                        if(game==1)
                        {
                            undo(board_pos, board, boardcopy, &score, undoscore);
                        }
                        break;

                    case SDLK_s:


                        break;
                    default:
                        break;
                }
            }
        }

        // lose or win
        losewin(board_pos, board, dif, &v, &l, &game);

        // funcionalidade avançada
                                    advancecopy(board_pos, board, score, timea, dif, name);
                                    
        //time
        timecount(begin, &timea, timeready, v, l, timesave);
        
        //highest value calculation
        highestvalue(board_pos, board, &level);
       
        // render game table
        RenderTable(board_pos, &board_size_px, &square_size_px, serif, imgs, renderer);
        // render board 
        RenderBoard(board, array_of_numbers, board_pos, board_size_px, square_size_px, renderer);
        // render stats
        RenderStats(renderer, dif, score, timea, opensans);
       
        //render victory or loss
        RenderVictory(renderer, opensans, v, l);

        // render in the screen all changes above
        SDL_RenderPresent(renderer);
        // add a delay
        SDL_Delay( delay );
    }

    //statistics
    statistics(score, timea, level, name);

    // free memory allocated for images and textures and closes everything including fonts
    UnLoadValues(array_of_numbers);
    TTF_CloseFont(serif);
    TTF_CloseFont(opensans);
    SDL_FreeSurface(imgs[0]);
    SDL_FreeSurface(imgs[1]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}


/**
 * LoadCards: Loads all images with the values
 * \param _array_of_numbers vector with all loaded numbers
 */
void LoadValues(SDL_Surface **_array_of_numbers)
{
    int i;
    char filename[STRING_SIZE];

    // loads all images to an array
    for ( i = 0 ; i < MAX_LEVELS; i++ )
    {
        // create the filename !
        sprintf(filename, ".//numbers//value_%02d.jpg", i+1);
        // loads the image !
        _array_of_numbers[i] = IMG_Load(filename);
        // check for errors: deleted files ?
        if (_array_of_numbers[i] == NULL)
        {
            printf("Unable to load image: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }
}


/**
 * UnLoadCards: unloads all values images of the memory
 * \param _array_of_numbers vector with all loaded image numbers
 */
void UnLoadValues(SDL_Surface **_array_of_numbers)
{
    // unload all cards of the memory: +1 for the card back
    for ( int i = 0 ; i < MAX_LEVELS; i++ )
    {
        SDL_FreeSurface(_array_of_numbers[i]);
    }
}

/**
 * RenderBoard: renders the board
 * \param _board multidimensional array with the board numbers
 * \param _array_of_numbers vector with all loaded images
 * \param _board_pos number of positions in the board
 * \param _board_size_px size of the board in pixels
 * \param _square_size_px size of each square
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderBoard(int _board[][MAX_BOARD_POS], SDL_Surface **_array_of_numbers, int _board_pos,
        int _board_size_px, int _square_size_px, SDL_Renderer *_renderer )
{
    int x_corner, y_corner;
    SDL_Rect boardPos;
    SDL_Texture *number_text;

    // corner of the board
    x_corner = (TABLE_SIZE - _board_size_px) >> 1;
    y_corner = (TABLE_SIZE - _board_size_px - 15);

    // renders the squares where the numbers will appear
    for ( int i = 0; i < _board_pos; i++ )
    {
        for ( int j = 0; j < _board_pos; j++ )
        {
            // basic check
            if ( _board[i][j] > 23 || _board[i][j] < 0 ) 
            {
                printf("Invalid board value: RenderBoard\n");
                exit(EXIT_FAILURE);
            }
            // only draws a number if it corresponds to a non-empty space
            if ( _board[i][j] != 0 )
            {
                // define the size and copy the image to display
                boardPos.x = x_corner + (i+1)*SQUARE_SEPARATOR + i*_square_size_px;  
                boardPos.y = y_corner + (j+1)*SQUARE_SEPARATOR + j*_square_size_px;
                boardPos.w = _square_size_px;
                boardPos.h = _square_size_px;
                number_text = SDL_CreateTextureFromSurface( _renderer, _array_of_numbers[ _board[i][j] - 1] );
                SDL_RenderCopy( _renderer, number_text, NULL, &boardPos );
                // destroy texture 
                SDL_DestroyTexture(number_text);
            }
        }
    }
}

/*
 * Shows some information about the game:
 * - Level to achieve (difficulty)
 * - Score of the game
 * - Time spent in seconds
 * \param _renderer renderer to handle all rendering in a window
 * \param _level current level
 * \param _score actual score
 * \param _time time to display in seconds
 */
void RenderStats( SDL_Renderer *_renderer, int dif, int _score, int _time, TTF_Font *_statsfont)
{

    /* TO DO*/
    SDL_Color white = { 255, 255, 255 }; // white
    //Rect #1
    SDL_Rect recpos; 

    recpos.x = 50;
    recpos.y = 100;
    recpos.w = 150;
    recpos.h = 50;

    //Rect #2
    SDL_Rect recpos2; 

    recpos2.x = 250;
    recpos2.y = 100;
    recpos2.w = 150;
    recpos2.h = 50;

    //Rect #3
    SDL_Rect recpos3; 

    recpos3.x = 450;
    recpos3.y = 100;
    recpos3.w = 150;
    recpos3.h = 50;

    //setting the color of the renderer
    SDL_SetRenderDrawColor(_renderer, 139, 69, 19, 255);

    SDL_RenderFillRect(_renderer, &recpos);
    SDL_RenderFillRect(_renderer, &recpos2);
    SDL_RenderFillRect(_renderer, &recpos3);

    char scorec[STRING_SIZE];
    sprintf(scorec, "%d", _score);

    char timec[STRING_SIZE];
    sprintf(timec, "%d", _time);

    dif = pow(2, dif);
    char difc[STRING_SIZE];
    sprintf(difc, "%d", dif);

    RenderText(recpos.x, recpos.y, scorec, _statsfont, &white, _renderer);
    RenderText(recpos2.x, recpos2.y, timec, _statsfont, &white, _renderer);
    RenderText(recpos3.x, recpos3.y, difc, _statsfont, &white, _renderer);

    RenderText(recpos.x, 50, "Score", _statsfont, &white, _renderer);
    RenderText(recpos2.x, 50, "Time", _statsfont, &white, _renderer);
    RenderText(recpos3.x, 50, "Nivel", _statsfont, &white, _renderer);

}

/*
 * RenderTable: Draws the table where the game will be played, namely:
 * -  some texture for the background
 * -  the right part with the IST logo and the student name and number
 * -  the grid for game board with squares and seperator lines
 * \param _board_pos number of squares in the board 
 * \param _font font used to render the text
 * \param _img surfaces with the table background and IST logo (already loaded)
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderTable( int _board_pos, int *_board_size_px, int *_square_size_px, TTF_Font *_font, SDL_Surface *_img[], SDL_Renderer* _renderer )
{
    SDL_Color black = { 0, 0, 0 }; // black
    SDL_Color light = { 205, 193, 181 };
    SDL_Color dark = { 120, 110, 102 };
    SDL_Texture *table_texture;
    SDL_Rect tableSrc, tableDest, board, board_square;
    int height, board_size_px, square_size_px;

    // set color of renderer to some color
    SDL_SetRenderDrawColor( _renderer, 255, 255, 255, 255 );

    // clear the window
    SDL_RenderClear( _renderer );

    tableDest.x = tableSrc.x = 0;
    tableDest.y = tableSrc.y = 0;
    tableSrc.w = _img[0]->w;
    tableSrc.h = _img[0]->h;
    tableDest.w = TABLE_SIZE;
    tableDest.h = TABLE_SIZE;

    // draws the table texture
    table_texture = SDL_CreateTextureFromSurface(_renderer, _img[0]);
    SDL_RenderCopy(_renderer, table_texture, &tableSrc, &tableDest);

    // render the IST Logo
    height = RenderLogo(TABLE_SIZE, 0, _img[1], _renderer);

    // render the student name
    height += RenderText(TABLE_SIZE+3*MARGIN, height, myName, _font, &black, _renderer);

    // this renders the student number
    RenderText(TABLE_SIZE+3*MARGIN, height, myNumber, _font, &black, _renderer);

    // compute and adjust the size of the table and squares
    board_size_px = (int)(BOARD_SIZE_PER*TABLE_SIZE);
    square_size_px = (board_size_px - (_board_pos+1)*SQUARE_SEPARATOR) / _board_pos; 
    board_size_px -= board_size_px % (_board_pos*(square_size_px+SQUARE_SEPARATOR));
    board_size_px += SQUARE_SEPARATOR;

    // renders the entire board background 
    SDL_SetRenderDrawColor(_renderer, dark.r, dark.g, dark.b, dark.a );
    board.x = (TABLE_SIZE - board_size_px) >> 1;
    board.y = (TABLE_SIZE - board_size_px - 15);
    board.w = board_size_px;
    board.h = board_size_px;
    SDL_RenderFillRect(_renderer, &board);

    // renders the squares where the numbers will appear
    SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a );

    // iterate over all squares
    for ( int i = 0; i < _board_pos; i++ )
    {
        for ( int j = 0; j < _board_pos; j++ )
        {
            board_square.x = board.x + (i+1)*SQUARE_SEPARATOR + i*square_size_px;  
            board_square.y = board.y + (j+1)*SQUARE_SEPARATOR + j*square_size_px;
            board_square.w = square_size_px;
            board_square.h = square_size_px;
            SDL_RenderFillRect(_renderer, &board_square);
        }
    }

    // destroy everything
    SDL_DestroyTexture(table_texture);
    // store some variables for later use
    *_board_size_px = board_size_px;
    *_square_size_px = square_size_px;
}

/**
 * RenderLogo function: Renders the IST logo on the app window 
 * \param x X coordinate of the Logo
 * \param y Y coordinate of the Logo
 * \param _logoIST surface with the IST logo image to render
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderLogo(int x, int y, SDL_Surface *_logoIST, SDL_Renderer* _renderer)
{
    SDL_Texture *text_IST;
    SDL_Rect boardPos;

    // space occupied by the logo
    boardPos.x = x;
    boardPos.y = y;
    boardPos.w = _logoIST->w;
    boardPos.h = _logoIST->h;

    // render it
    text_IST = SDL_CreateTextureFromSurface(_renderer, _logoIST);
    SDL_RenderCopy(_renderer, text_IST, NULL, &boardPos);

    // destroy associated texture !
    SDL_DestroyTexture(text_IST);
    return _logoIST->h;
}

/**
 * RenderText function: Renders some text on a position inside the app window
 * \param x X coordinate of the text
 * \param y Y coordinate of the text
 * \param text string with the text to be written
 * \param _font TTF font used to render the text
 * \param _color color of the text
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderText(int x, int y, const char *text, TTF_Font *_font, SDL_Color *_color, SDL_Renderer* _renderer)
{
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect solidRect;

    solidRect.x = x;
    solidRect.y = y;
    // create a surface from the string text with a predefined font
    text_surface = TTF_RenderText_Blended(_font,text,*_color);
    if(!text_surface)
    {
        printf("TTF_RenderText_Blended: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    // create texture
    text_texture = SDL_CreateTextureFromSurface(_renderer, text_surface);
    // obtain size
    SDL_QueryTexture( text_texture, NULL, NULL, &solidRect.w, &solidRect.h );
    // render it !
    SDL_RenderCopy(_renderer, text_texture, NULL, &solidRect);
    // clear memory
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
    return solidRect.h;
}

/**
 * InitEverything: Initializes the SDL2 library and all graphical components: font, window, renderer
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _font font that will be used to render the text
 * \param _img surface to be created with the table background and IST logo
 * \param _window represents the window of the application
 * \param _renderer renderer to handle all rendering in a window
 */
void InitEverything(int width, int height, TTF_Font **_font, SDL_Surface *_img[], SDL_Window** _window, SDL_Renderer** _renderer, TTF_Font **_statsfont)
{
    InitSDL();
    InitFont();
    *_window = CreateWindow(width, height);
    *_renderer = CreateRenderer(width, height, *_window);

    // load the table texture
    _img[0] = IMG_Load("table_texture.png");
    if (_img[0] == NULL)
    {
        printf("Unable to load image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // load IST logo
    _img[1] = SDL_LoadBMP("ist_logo.bmp");
    if (_img[1] == NULL)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // this opens (loads) a font file and sets a size
    *_font = TTF_OpenFont("FreeSerif.ttf", 16);
    if(!*_font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    *_statsfont = TTF_OpenFont("OpenSans.ttf", 32);
    if(!*_statsfont)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitSDL: Initializes the SDL2 graphic library
 */
void InitSDL()
{
    // init SDL library
    if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        printf(" Failed to initialize SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitFont: Initializes the SDL2_ttf font library
 */
void InitFont()
{
    // Init font library
    if(TTF_Init()==-1)
    {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * CreateWindow: Creates a window for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \return pointer to the window created
 */
SDL_Window* CreateWindow(int width, int height)
{
    SDL_Window *window;
    // init window
    window = SDL_CreateWindow( "1024", WINDOW_POSX, WINDOW_POSY, width, height, 0 );
    // check for error !
    if ( window == NULL )
    {
        printf("Failed to create window : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return window;
}

/**
 * CreateRenderer: Creates a renderer for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _window represents the window for which the renderer is associated
 * \return pointer to the renderer created
 */
SDL_Renderer* CreateRenderer(int width, int height, SDL_Window *_window)
{
    SDL_Renderer *renderer;
    // init renderer
    renderer = SDL_CreateRenderer( _window, -1, 0 );

    if ( renderer == NULL )
    {
        printf("Failed to create renderer : %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // set size of renderer to the same as window
    SDL_RenderSetLogicalSize( renderer, width, height );

    return renderer;
}


//Function that asks for the parameters of the game (dimension of the board, difficulty and name of the player)
void parametros(int *_dim, int *dif, char name[MAX_NAME], int board[MAX_BOARD_POS][MAX_BOARD_POS])
{
    *_dim = 0;
    *dif = 0;

    //Does it once and if the parameters don't meet what is needed then repeat
    do
    {
        printf("Bem-vindo jogador! Qual é o seu nome? ");
        scanf("%s", name);

        if(strlen(name)>MAX_NAME)
            printf("O nome inserido excede o número permitido!\n");
    }while(strlen(name)>MAX_NAME);

    do
    {
        printf("Qual é a dimensao do seu tabuleiro? ");
        scanf("%d", _dim);

        if(*_dim>MAX_BOARD_POS||*_dim<MIN_BOARD_POS)
            printf("A dimensao introduzida está fora do alcance possível!\n");
    } while(*_dim>MAX_BOARD_POS||*_dim<MIN_BOARD_POS);

    do
    {
        printf("E a dificuldade do jogo? DICA: Vai até 23\n");
        scanf("%d", dif);

        if(*dif>MAX_LEVELS||(*dif<MIN_LEVELS))
            printf("A dificuldade introduzida está fora do alcance possível!\n");
    }while(*dif>MAX_LEVELS||(*dif<MIN_LEVELS));


}

//Creating 3*2 random numbers (value of the piece, column and row)
void aleatorio(int board_pos, int *valuec, int *valuer, int *value, int *valuec_init, int *valuer_init, int *value_init)
{
    srand(time(NULL));

    int range[2]={1,2};
    int rangei = rand()%2;
    
    *valuec = rand()% board_pos;
    *valuer = rand()% board_pos;

    *value = range[rangei];

    do
    {int rangei_init = rand()%2;
    
    *valuec_init = rand()% board_pos;
    *valuer_init = rand()% board_pos;

    *value_init = range[rangei_init];}while(valuec==valuec_init&&valuer==valuer);
}


//Movements
void slideup(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *det)
{
    int x, y, z = 0;
    *det = 0;

    //Scan board
    for (x=0; x<board_pos; x++)
    {
        for(y=0; y<board_pos; y++)
        {  
            //Detect piece with value different of 0 and that can be moved (the next is 0)
            if(board[x][y]!=0 && board[x][y-1]==0 && y-1>=0)
            {
                //Number of times the piece has to be moved by scanning the entire column upwards (z)
                while(board[x][y-z-1]==0 && (y-z)>0)
                {
                    z++;
                    *det = 1;
                }

                //Does changes
                board[x][y-z]=board[x][y];
                board[x][y]=0;

                //z goes back to 0
                z = 0;
            }
        }
    }
}

void slidedown(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *det)
{
    int x, y, z = 0;
    *det = 0;

    for (x=board_pos-1; x>=0; x--)
    {
        for(y=board_pos-1; y>=0; y--)
        {  
            if(board[x][y]!=0 && board[x][y+1]==0 && y+1<board_pos)
            {
                while(board[x][y+z+1]==0 && (y+z)<board_pos-1)
                {
                    z++;
                    *det = 1;
                }
                    
                    board[x][y+z]=board[x][y];
                    board[x][y]=0;
    
                z = 0;
            }
        }
    }
}

void slideleft(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *det)
{
    int x, y, z = 0;
    *det = 0;

    for (x=0; x<board_pos; x++)
    {
        for(y=0; y<board_pos; y++)
        {  
            if(board[x][y]!=0 && board[x-1][y]==0 && x-1>=0)
            {
                while(board[x-z-1][y]==0 && (x-z)>0)
                {
                    z++;
                    *det = 1;
                }
                    
                    board[x-z][y]=board[x][y];
                    board[x][y]=0;
    
                z = 0;
            }
        }
    }
}

void slideright(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *det)
{
    int x, y, z = 0;
    *det = 0;

    for (x=board_pos-1; x>=0; x--)
    {
        for(y=board_pos-1; y>=0; y--)
        {  
            if(board[x][y]!=0 && board[x+1][y]==0 && x+1<board_pos)
            {
                while(board[x+z+1][y]==0 && (x+z)<board_pos-1)
                {
                    z++;
                    *det = 1;
                }
                    
                    board[x+z][y]=board[x][y];
                    board[x][y]=0;

                z = 0;
            }
        }
    }
}

//Gerar peça aleatória
void randompiece(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int detect, int detectmerge)
{
    srand(time(NULL));
    int valuec, valuer, value;

    //Did any of the pieces move or if any merged together? If yes procceed
    if(detect==1 || detectmerge==1)
    {
        //Generating 3 random numbers: one for the column, other for the row and other for the value (1 or 2).
        //Do and, if the position is occupied, do it again.
        do
        {
        valuec = rand()% board_pos;
        valuer = rand()% board_pos;

        value = rand()%2 + 1;
        }while(board[valuec][valuer]!=0);


        board[valuec][valuer]=value;  
    }

}

//Fusao das peças
void mergeup(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *_score, int *_detectmerge)
{
    int x, y;
    *_detectmerge=0;
    for (x=0; x<board_pos; x++)
    {
        for(y=0; y<board_pos; y++)
        {  
            if (board[x][y]!=0 && board[x][y]==board[x][y-1])
            {
                board[x][y-1] = board[x][y-1] + 1;
                *_score += pow(2, board[x][y-1]);
                board[x][y]=0;
                *_detectmerge=1;
            }
        }       
    }
}

void mergedown(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *_score, int *_detectmerge)
{
    int x, y;
    *_detectmerge=0;

    for (x=board_pos-1; x>=0; x--)
    {
        for(y=board_pos-1; y>=0; y--)
        {  
            if (board[x][y]!=0 && board[x][y]==board[x][y+1])
            {
                board[x][y+1] = board[x][y+1] + 1;
                *_score += pow(2, board[x][y+1]);
                board[x][y]=0;
                *_detectmerge=1;
            }
        }       
    }
}

void mergeleft(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *_score, int *_detectmerge)
{
    int x, y;
    *_detectmerge=0;

    for (x=0; x<board_pos; x++)
    {
        for(y=0; y<board_pos; y++)
        {  
            if (board[x][y]!=0 && board[x][y]==board[x-1][y])
            {
                board[x-1][y] = board[x-1][y] + 1;
                *_score += pow(2, board[x-1][y]);
                board[x][y]=0;
                *_detectmerge=1;
            }
        }       
    }
}

void mergeright(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *_score, int *_detectmerge)
{
    int x, y;
    *_detectmerge=0;

    for (x=board_pos-1; x>=0; x--)
    {
        for(y=board_pos-1; y>=0; y--)
        {  
            if (board[x][y]!=0 && board[x][y]==board[x+1][y])
            {
                board[x+1][y] = board[x+1][y] + 1;
                *_score += pow(2, board[x+1][y]);
                board[x][y]=0;
                *_detectmerge=1;
            }
        }       
    }
}

//Limpar o tabuleiro
void clearmatrix(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS])
{
    int x, y;

    //Colocar todas as posiçoes a 0
    for(x=0; x<board_pos; x++)
    {
        for(y=0;y<board_pos;y++)
        {
            board[x][y] = 0;
        }
    }
}


//Função que deteta lose ou win, com duas flags que são usadas para o RenderVictory
void losewin(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int dif, int *v, int *l, int *game)
{
    int x, y;    
    *l = 1;

    for(x=0; x<board_pos; x++)
    {
        for(y=0;y<board_pos;y++)
        {
            //Atingir peça com a dificuldade máxima
            if(board[x][y]==dif)
            {
                *v = 1;
                *game = 0;
            }

            //Detetar se é possível haver merge de peças ou se há uma peça a 0
            if((board[x][y]==0)||(board[x][y]!=0 && (board[x][y]==board[x][y+1]||board[x][y]==board[x+1][y]||board[x][y]==board[x-1][y]||board[x][y]==board[x][y-1])))
            {
                *l = 0; 
            }
        }
    }
}

//Fazer uma função só para render para separar a mecanica do jogo do aspeto gráfico
void RenderVictory(SDL_Renderer *_renderer, TTF_Font *_statsfont, int v, int l)
{
    SDL_Color white = { 255, 255, 255 }; // white

    //Definição do rectângulo
    SDL_Rect rstat; 

    rstat.x = 250;
    rstat.y = 200;
    rstat.w = 150;
    rstat.h = 50;

    //Permite escrever menos linhas de código
    if(v==1)
    {
        //setting the color of the renderer
        SDL_SetRenderDrawColor(_renderer, 58, 75, 131, 255);

        SDL_RenderFillRect(_renderer, &rstat);

        RenderText(rstat.x, rstat.y, "Vencedor!", _statsfont, &white, _renderer);
    }

    if(l==1)
    {
        //setting the color of the renderer
        SDL_SetRenderDrawColor(_renderer, 131, 58, 58, 255);

        SDL_RenderFillRect(_renderer, &rstat)   ;

        RenderText(rstat.x, rstat.y, "Perdedor!", _statsfont, &white, _renderer);
    }
}

//Copiar a matriz e score a cada jogo
void copymatrix (int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int boardcopy[MAX_BOARD_POS][MAX_BOARD_POS], int _score, int *_undoscore)
{
    int x, y;

    for(x=0; x<board_pos; x++)
    {
        for(y=0;y<board_pos;y++)
        {
            boardcopy[x][y]=board[x][y];
        }
    }

    *_undoscore = _score;
}

//Fazer o undo efetivamente
void undo (int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int boardcopy[MAX_BOARD_POS][MAX_BOARD_POS], int *_score, int _undoscore)
{
    int x, y;
    for(x=0; x<board_pos; x++)
    {
        for(y=0;y<board_pos;y++)
        {
            board[x][y]=boardcopy[x][y];
        }
    }

    *_score = _undoscore;
}

//Ficheiro de estatísticas
void statistics(int score, int timea, int level, char name[MAX_NAME])
{
    FILE *f = fopen("stats.txt", "a");
    if (f==NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    fprintf(f, "Name: %s | Score: %d | Time: %d | Piece with highest value: %d\n", name, score, timea, level);

    fclose(f);
}

//Descobrir a peça de maior valor
void highestvalue(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *high)
{
    int x, y;
    *high = board[0][0];

    //Comparação em ciclo for que caso a peça a ser scaneada seja maior que a da posição (0,0) a (0,0) fica com esse valor
     for(x=0; x<board_pos; x++)
    {
        for(y=0;y<board_pos;y++)
        {
            if(board[x][y]>*high)
                *high = board[x][y];
        }
    }

    *high = pow(2, *high);
}

//Função que conta o tempo
void timecount(time_t beginoftime, int *time_variable, int timeready, int v, int l, int timesave)
{
    //Quando puder contar o tempo e não derrota (l) ou vitória (1)
    if(timeready==1 && v==0 &&  l==0)
    {
    time_t endoftime = time(&endoftime);
    *time_variable=difftime(endoftime, beginoftime) + timesave;
    }
}


void advancecopy(int board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int _score, int _time_variable, int _dif, char name[MAX_NAME])
{
    int x, y;

    FILE *fgame = fopen("saveboard.txt", "w");
    if (fgame==NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

     for(x=0; x<board_pos; x++)
    {
        for(y=0;y<board_pos;y++)
        {
            fprintf(fgame, "%d\n", board[x][y]);
        }
    }

    fclose(fgame);

    FILE *fs = fopen("savestats.txt", "w+");
    if (fs==NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    fprintf(fs, "%d %d %d %d %s", _score, _time_variable, _dif, board_pos, name);

    fclose(fs);
}

void advanceopen(int *board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], int *_score, int *_time_variable, int *_dif, char name[MAX_NAME], int *timeready)
{
    int x, y;


    FILE *fs = fopen("savestats.txt", "r");
    if (fs==NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    fscanf(fs, "%d %d %d %d %s", _score, _time_variable, _dif, board_pos, name);

    fclose(fs);

    FILE *fgame = fopen("saveboard.txt", "r");
    if (fgame==NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }


     for(x=0; x<*board_pos; x++)
    {
        for(y=0;y<*board_pos;y++)
        {
            fscanf(fgame, "%d\n", &board[x][y]);
        }
    }

    fclose(fgame);

    *timeready = 1;
}

void oldornew(int *board_pos, int board[MAX_BOARD_POS][MAX_BOARD_POS], char name[MAX_NAME], int *dif, int *timeready, int *timesave, int *score)
{
    char save[4];

    printf("Bem-vindo jogador! Pretende carregar o jogo anterior? Yes/No\n");
    scanf("%s", save);

    if(!strcmp(save, "yes") || !strcmp(save, "Yes") || !strcmp(save, "YES"))
    {
        advanceopen(board_pos, board, score, timesave, dif, name, timeready);
    }

    else if(!strcmp(save, "no") || !strcmp(save, "No") || !strcmp(save, "NO"))
    {
        parametros(board_pos, dif, name, board);
    }

    else
    {
        printf("Comando não reconhecido, tente novamente!\n");
        printf("Bem-vindo jogador! Pretende carregar o jogo anterior? Yes/No\n");
        scanf("%s", save);
    }
}