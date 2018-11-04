// gcc ISTDots.c -g -I/usr/local/include -Wall -pedantic -std=c99 -L/usr/local/lib -lm -lSDL2 -lSDL2_ttf -lSDL2_image -o ISTDots

/*
*	Autor do trabalho: Rodrigo Figueiredo || nº 90185
*
*  Projeto intermédio para a cadeira de Programação
*  Ano letivo 2017/2018
*  
*   O projeto tem como objetivo a representação do jogo Two Dots. Jogo este que tem tido grande adesão pelos adolescentes.
*   
*   O jogo consiste na ligação de diferentes pontos da mesma cor que no final de cada jogada rebentam
*   Caso o utilizador desenhe um quadrado, todos os pontos da mesma cor desaparecem, bem como todos os pontos que estejam presentes dentro do quadrado
*   
*   Caso o jogador cumpra com os objetivos definidos por si no inicio do jogo num número de jogadas igual ou inferior ao estipulado por si, terá uma vitória.
*   Caso contrário uma derrota.
*   Após cada jogo terá a opção de sair(tecla Q) ou começar outro jogo com os mesmos parâmetros (N)
*   
*   Quando o utilizador sair do jogo, será criado um ficheiro no qual vai ser escrito o número de jogos realizados e, para cada jogo, 
*   se obteve uma vitória ou derrota e o número de jogadas utilizadas.
*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <locale.h>

#define MAX(a,b)    (((a)>(b))?(a):(b))
#define M_PI 3.1415926535897932384626433832
#define STRING_SIZE 100       // max size for some strings
#define TABLE_SIZE 850        // main game space size
#define LEFT_BAR_SIZE 150     // left white bar size
#define WINDOW_POSX 200       // initial position of the window: x
#define WINDOW_POSY 200       // initial position of the window: y
#define SQUARE_SEPARATOR 8    // square separator in px
#define BOARD_SIZE_PER 0.7f   // board size in % wrt to table size
#define MAX_BOARD_POS 15      // maximum size of the board
#define MAX_COLORS 7
#define MARGIN 5

#define MAX_NAME_SIZE 9         // tamanho maximo para o nome
#define MIN_TABLE_SIZE 5        // tamanho minimo para o tabuleiro
#define MAX_TABLE_SIZE 15       // tamanho maxmo para o tabuleiro
#define MAX_SIZE_DOTS 99        // tamanho máximo para o vetor do número de pontos objetivo para cada cor
#define MAX_MOVE 225            // número máximo de ligações +1
#define MAX_COLOURS_USER 5      // número máximo de cores para o utilizador
#define MAX_NUM_GAMES 1000      // número máximo de componentes para o vetor que contém os dados para escrever nos ficheiros
#define corBranco 5             // número da cor branca
#define corPreto 6              // número da cor preta


// declaration of the functions related to graphical issues
void InitEverything(int , int , TTF_Font **, SDL_Surface **, SDL_Window ** , SDL_Renderer ** , TTF_Font **);
void InitSDL();
void InitFont();
SDL_Window* CreateWindow(int , int );
SDL_Renderer* CreateRenderer(int , int , SDL_Window *);
int RenderText(int, int, const char *, TTF_Font *, SDL_Color *, SDL_Renderer *);
int RenderLogo(int, int, SDL_Surface *, SDL_Renderer *);
int RenderTable(int, int, int [], TTF_Font *, SDL_Surface **, SDL_Renderer *, int, int, int, int [2][MAX_MOVE], int, int);
void ProcessMouseEvent(int , int , int [], int , int *, int * );
void RenderPoints(int [][MAX_BOARD_POS], int, int, int [], int, SDL_Renderer *);
void RenderStats( SDL_Renderer *, TTF_Font *, int [], int , int );
void filledCircleRGBA(SDL_Renderer * , int , int , int , int , int , int );

void obterParametros1(int*, int*); // altura, largura
void obterParametros2(char [MAX_NAME_SIZE], int*); // nome, nJogadas
void obterParametros3(int*, int[MAX_COLOURS_USER]); // nCores, nPontos objetivo para cada cor
void limparTab(int, int, int[MAX_TABLE_SIZE][MAX_TABLE_SIZE]); // comprimento, largura, matriz do tabuleiro
void gerarCores(int, int, int, int[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int, int); // altura, largura, nrcores, matriz do tabuleiro, cor da jogada, variável quadrado
int validaMov(int [MAX_TABLE_SIZE][MAX_TABLE_SIZE], int, int, int); // matriz board, pt_x, pt_y, corIni
void eliminaPontos(int [MAX_TABLE_SIZE][MAX_TABLE_SIZE], int[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int[MAX_COLOURS_USER]); // matriz do tabuleiro, matrizBranca, vetor nPontos
void descerPontos(int [MAX_TABLE_SIZE][MAX_TABLE_SIZE], int, int); // matriz do tabuleiro, altura, largura
int retrocesso(int [2][MAX_MOVE], int* ); // matriz dos vetores, linhacoord
int verificaJogadas(int [MAX_TABLE_SIZE][MAX_TABLE_SIZE], int, int); // matriz board, altura, largura
void reorganiza(int[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int, int, int); // matriz board, numero de cores, altura, largura
int verificaQuadrado(int[2][MAX_MOVE], int, int, int); // matriz das coordenadas, pt_x, pt_y, linhaCoord
void eliminaQuadrado(int[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int, int, int); // matriz board, matrizBranca, altura, largura, corIni
void converteCoordBranca(int[2][MAX_MOVE], int[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int); // matriz coordenadas, matrizBranca, linhaCoord
void eliminaPontosDentroQuadrado(int[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int, int, int [MAX_TABLE_SIZE][MAX_TABLE_SIZE]); // matrizBranca, altura, largura, board
int pontoRodeado(int, int, int[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int, int); // cordx, cordy, matrizbranca, altura, largura
void gerarCong(int, int, int[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int*, int*); // altura, largura, board, pos_x_cong, pos_y_cong
void decrementaJogadas(int*, int); //nJogadas, linhaCoord
void backup (int, int[MAX_COLOURS_USER], int[MAX_COLOURS_USER], int*); // nJogadas, nPontos, nPontosBCKUP, nJogadasBCKUP
int fimJogo(int, int[MAX_COLOURS_USER]); // nJogadas, nPontos
void ativaBACKUP(int*, int, int[MAX_COLOURS_USER], int[MAX_COLOURS_USER]); // nJogadas, nJogadasBCKUP, nPontos, nPontosBCKUP
void limpaJogadas(int [2][MAX_MOVE]); // matriz com as coordenadas da jogada
void renderVitoria(SDL_Renderer * , TTF_Font *); // função para mostrar vitória no ecrã
void renderDerrota(SDL_Renderer * , TTF_Font *); // função para mostrar derrota no ecrã
void renderBaralha(SDL_Renderer * , TTF_Font *); // função para mostrar shuffling no ecrã
void escreverFicheiros(char [MAX_NAME_SIZE], int [MAX_NUM_GAMES], int ); // nome, vetor com resultados do jogo, variável pos que indica a posição no vetor
void renderInstrucoes(SDL_Renderer * , TTF_Font *); // função para mostar as instruções de jogo



// definition of some strings: they cannot be changed when the program is executed !
const char myName[] = "Rodrigo Figueiredo";
const char myNumber[] = "IST 90185";
const int colors[3][MAX_COLORS] = {{255, 0, 0, 255, 119, 255, 0},{165, 0, 255, 0, 136, 255, 0},{0, 255, 0, 0, 153, 255, 0}};

/**
 * main function: entry point of the program
 * only to invoke other functions !
 */
int main( void )
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *serif = NULL;
    TTF_Font *capture = NULL;
    SDL_Surface *imgs[2];
    SDL_Event event;
    int delay = 100;
    int quit = 0;
    int width = (TABLE_SIZE + LEFT_BAR_SIZE);
    int height = TABLE_SIZE;
    int square_size_px = 0, board_size_px[2] = {0};
    int board_pos_x = 0, board_pos_y = 0;
    int board[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}};
    int pt_x = 0, pt_y = 0;
    int altura = 0, largura = 0, nCores = 0, nJogadas = 0;
    char name[MAX_NAME_SIZE];

    int nPontos[MAX_COLOURS_USER] = {0}; // vetor com o número de cores objetivo definido pelo utilizador
    int corIni = 0; // cor da bola onde se carrega no botão
    int matrizBranca [MAX_TABLE_SIZE][MAX_TABLE_SIZE] = {{0}}; // matriz que contém 1 onde o ponto vai ter a cor branca e 0 onde não vai ter a cor branca
    int checkDown = 0; // Variável para verificar que o botão do rato está premido
    int xmov = 0, ymov = 0; // Variável para guardar as posições do rato anteriores às atuais
    int matrizCoord [2][MAX_MOVE] = {{0}}; // matriz que contém os pontos da jogada que está a ser efetuada
    int linhaCoord = 0; // determina a posição da matizCoord em que vai escrever
    int quadrado = 0; // vetor que tem na posição 0 se ocorre quadrado ou não e nas restantes as coordenadas do ponto de interceção
    int nPontosBCKUP[MAX_COLOURS_USER] = {0}; // vetor que contém os objetivos de cores definidos pelo utilizador no inicio do jogo
    int nJogadasBCKUP  = 0; // variável que contém o número de jogadas definidos pelo utilizador
    int vectFicheiro[MAX_NUM_GAMES] ={0}; // vetor que contém a informação de cada jogo do utilizador
    int pos = 0; // variável que determina a posição de escrita no vectFicheiro
    int vitoria = 0; // variável que guarda se o último jogo foi vitória
    int derrota = 0; // variável que guarda se o último jogo foi derrota
    int varInicio = 0; // variável que determina que a janela do jogo foi iniciada e que ainda não carregou na tecla N
    int pos_x_cong = 0; // variável que contém a coordenada em x do ponto congelado
    int pos_y_cong = 0; // variável que contém a coordenada em y do ponto congelado

    obterParametros1(&altura, &largura); // função para ler a altura e largura do tabuleiro inicial
    obterParametros2(name, &nJogadas); // função para ler o nome e o número de jogadas para o jogo
    obterParametros3(&nCores, nPontos); // função para ler o número de cores disponiveis para jogar e o número de pontos objetivo para cada cor
    backup(nJogadas, nPontos, nPontosBCKUP, &nJogadasBCKUP); // função que guarda o número de jogadas iniciais bem como o objetivo dos pontos para
                                                             // atualizar a interface gráfica quando carregar em N
    srand(1234); // inicialização de rand function para gerar valores aleatórios
    limparTab(altura, largura, board); // função para limpar o tabuleiro (colocar a cor braca em todas as posições)
    

    board_pos_x = largura;
    board_pos_y = altura;


    // initialize graphics
    InitEverything(width, height, &serif, imgs, &window, &renderer, &capture);

    while( quit == 0 )
    {
        // while there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            if( event.type == SDL_QUIT )
            {
                quit = 1;
            }
            else if ( event.type == SDL_KEYDOWN )
            {
                switch ( event.key.keysym.sym )
                {
                    case SDLK_n:
                        if(varInicio == 0) // quando o tabuleiro já está feito mas o jogador ainda não carregou na tecla N
                        {
                            gerarCong(altura, largura, board, &pos_x_cong, &pos_y_cong); // gerar um bloco de congelamento
                            gerarCores(altura, largura, nCores, board, corIni, quadrado); // função para gerar cores aleatóriamente
                            varInicio = 1; // significa que está um jogo a decorrer
                        }
                        else if((vitoria == 1) && (derrota == 0) && (varInicio == 1)) // se o jogador tiver obtido uma vitória && se o jogo tiver começado
                        {
                            vectFicheiro[pos] = 1; // o jogador ganhou
                            vectFicheiro[pos + 1] = nJogadasBCKUP - nJogadas; // número de jogadas que utilizou
                            pos = pos + 2; // aumenta 2 posições no vetor
                            vitoria = 0; 
                            derrota = 0;
                        }
                        else if(((derrota == 1) || (derrota == 0 && vitoria == 0)) && (varInicio == 1)) // (se o jogador tiver obtido uma derrota || se estiver a meio de um jogo) 
                                                                                                        // && se o jogo já tenha começado
                        {
                            vectFicheiro[pos] = 0; // o jogador teve uma derrota
                            vectFicheiro[pos + 1] = nJogadasBCKUP - nJogadas; // número de jogadas utilizadas
                            pos = pos + 2; // aumenta 2 posições no vetor
                            vitoria = 0;
                            derrota = 0;
                            renderDerrota(renderer, capture); // aparece derrota no ecrã
                            SDL_RenderPresent(renderer);
							SDL_Delay(500);
                        }
                        limparTab(altura, largura, board); // limpar a matriz do tabuleiro = colocar todos os pontos a branco
                        gerarCong(altura, largura, board, &pos_x_cong, &pos_y_cong); // gerar um bloco de congelamento
                        gerarCores(altura, largura, nCores, board, corIni, quadrado); // colocar cores aletórias nos pontos em branco
                        ativaBACKUP(&nJogadas, nJogadasBCKUP, nPontos, nPontosBCKUP); // atualização dos objetivos na parte gráfica com os parametros definidos inicialmente pelo utilizador
                        break;
                    case SDLK_q:
                        if((vitoria == 1) && (derrota == 0) && (varInicio == 1)) // o utilizador carregou na tecla Q deopis de ter obtido uma vitória
                        {
                            vectFicheiro[pos] = 1; // obteve vitória
                            vectFicheiro[pos + 1] = nJogadasBCKUP - nJogadas; // número de jogadas utilizadas
                            pos = pos + 2; // incrementa 2 posições no vetor
                            vitoria = 0;
                            derrota = 0;
                        }
                        else if((((derrota == 1) && (vitoria == 0)) || ((derrota == 0) && (vitoria == 0))) && (varInicio == 1)) 
                        // (se o jogador tiver obtido uma derrota || se estiver a meio de um jogo)  && se o jogo já tenha começado
                        {
                            vectFicheiro[pos] = 0; // o jogador obteve uma derrota
                            vectFicheiro[pos + 1] = nJogadasBCKUP - nJogadas; // número de jogadas utilizadas
                            pos = pos + 2; // incrementa 2 posições no vetor
                            derrota = 0;
                            vitoria = 0;
                            renderDerrota(renderer, capture); // aparece derrota no ecrã
                            SDL_RenderPresent(renderer);
							SDL_Delay(500);
							
                        }
                        else if (varInicio == 0) // se o utilizador carregar na tecla Q sem ainda ter iniciado um jogo novo
                        {
                            //do nothing
                        }
                        escreverFicheiros(name, vectFicheiro, pos); // antes da janela fechar escrever os ficheiros com as estatisticas
                        quit = 1;
                        break;
                    default:
                        break;
                }
            }
            else if ( (event.type == SDL_MOUSEBUTTONDOWN) && (varInicio == 1) && (derrota == 0 && vitoria == 0) )
            // se o jogo já tiver começado e se o jogador não tiver obtido uam vitória nem uma derrota pois assim garante que todos os movimentos
            // do rato serão ignorados até o jogador carregar em N ou Q
            {
                ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y);
                corIni = board[pt_x][pt_y]; // coloca a cor do ponto inicial de cada jogada na variável corIni
                checkDown = 1; // colocar a variável a 1 quando o botão do rato é premido
                xmov = pt_x; // guarda as posições anteriores do rato para verificar se a jogada foi válida
                ymov = pt_y;    
                matrizCoord[0][0] = pt_x; // coloca na matriz das coordenadas o ponto inicial da jogada
                matrizCoord[1][0] = pt_y;
                linhaCoord = 1; // define a variável que aponta para a matriz das coordenadas para a posição 1 pois a 0 tem o primeiro ponto da jogada
            }
            else if ( (event.type == SDL_MOUSEBUTTONUP) && (varInicio == 1) && (derrota == 0 && vitoria == 0) )
            // se o jogo já tiver começado e se o jogador não tiver obtido uam vitória nem uma derrota pois assim garante que todos os movimentos
            // do rato serão ignorados até o jogador carregar em N ou Q
            {
                checkDown = 0; // coloca a variável a 0 quando o botão do rato é libertado
                ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y);
                if(quadrado == 1 ) // se a jogada foi uma forma fechada
                {
                    eliminaPontosDentroQuadrado(matrizBranca, altura, largura, board); // elimina os pontos dentro da forma fechada
                    eliminaQuadrado(board, matrizBranca, altura, largura, corIni); // elimina todos os pontos da mesma cor da forma fechada
                }

                // para evitar erros, todos os pontos da matriz das coordenadas vão ficar a 1 na matriz branca
                // quando a função eliminaPontos for chamada todos os pontos selecionados vão ser eliminados 
                converteCoordBranca(matrizCoord, matrizBranca, linhaCoord); 
                if(linhaCoord == 1)                                         //caso a jogada seja retrocedida para o incio
                {
                    matrizBranca[matrizCoord[0][0]][matrizCoord[1][0]] = 0; // o ponto inicial da jogada fica a 0 na matriz branca, ou seja não vai ser eliminado
                    limpaJogadas(matrizCoord); // para evitar erros de eliminação de pontos a matriz das coordenadas vai ser toda colocada a 0
                }
                eliminaPontos(board, matrizBranca, nPontos); // elimina todos os pontos selecionados
                descerPontos(board, altura, largura);       // faz descer os pontos do tabuleiro
                gerarCores(altura, largura, nCores, board,corIni, quadrado); // gerar cores nos pontos que não têm cor
                decrementaJogadas(&nJogadas, linhaCoord); // decrementa 1 nas jogadas restantes
                quadrado = 0; // coloca a variavel que deteta forma fechada a 0
                if(verificaJogadas(board, altura, largura)) // caso não hajam jogadas possíveis vai reordenar os pontos presentes na matriz, mantendo o mesmo número de cada cor
                {
                    renderBaralha(renderer,capture); // aparece "shuffling" no ecrã
                    SDL_RenderPresent(renderer);
                    SDL_Delay(1000);
                    reorganiza(board, nCores, altura, largura);
                }
                linhaCoord = 0; // coloca a variável que aponta para a matriz das coordenadas a 0
            }
            else if ( (event.type == SDL_MOUSEMOTION) && (varInicio == 1) && (derrota == 0 && vitoria == 0))
            {
                ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y);
                if (validaMov(board, pt_x, pt_y, corIni) && checkDown) // = caso a jogada seja válida e o botão do rato esteja premido
                {
                    // restrição dos movimentos permitidos apenas aos pontos adjacentes, excluindo as diagonais entre os pontos
                    if((((pt_x - xmov) == 1) && ((pt_y - ymov) == 0)) || (((pt_x - xmov) == -1) && ((pt_y - ymov) == 0)) ||
                        (((pt_x - xmov) == 0) && ((pt_y - ymov) == 1)) || (((pt_x - xmov) == 0) && ((pt_y - ymov) == -1)))
                    {
                        matrizBranca[xmov][ymov] = 1; // vai colocar a branco o ponto inicial da jogada para posteriormente eliminar o ponto
                        matrizBranca[pt_x][pt_y] = 1; // vai colocar a branco o ponto onde o rato se encontra para posteriormente eliminar o ponto
                        matrizCoord[0][linhaCoord] = pt_x; // escrever na matriz as coordenadas do ponto onde o rato se encontra
                        matrizCoord[1][linhaCoord] = pt_y;
                        xmov=pt_x; // guarda as posições atuais do rato 
                        ymov=pt_y;
                        if(retrocesso(matrizCoord, &linhaCoord) == 1) // se não retroceder na jogada atual
                        {
                            quadrado = verificaQuadrado(matrizCoord, pt_x, pt_y, linhaCoord); // verificar se a jogada atual foi um quadrado
                            linhaCoord++; // avança uma unidade na matriz das coordenadas
                        }
                        else if(retrocesso(matrizCoord, &linhaCoord) == -1) // = caso a posição onde o rato se encontra seja repetida, ou seja, retrocedeu
                        {
                            matrizBranca[matrizCoord[0][linhaCoord - 1]][matrizCoord[1][linhaCoord - 1]] = 0; // anula a eliminação do ponto pois significa
                                                                                                              // que o utilizador moveu o rato para trás
                                                                                                              // descelecionando o ponto
                            linhaCoord--; // retrocede uma unidade na matriz das coordenadas
                            quadrado = 0; // garante que a variável que deteta formas fechadas está a 0
                        }
                    }
                }
            }
        }
        // render game table
        square_size_px = RenderTable( board_pos_x, board_pos_y, board_size_px, serif, imgs, renderer, pos_x_cong, pos_y_cong, varInicio, matrizCoord, linhaCoord, corIni);
        // aparece um retângulo na parte superior do tabuleiro a mostrar as instruções do jogo
        renderInstrucoes(renderer, serif);
        // função para atualizar a parte gráfica dos objetivos
        RenderStats(renderer, serif, nPontos, nCores, nJogadas);
        // render board
        RenderPoints(board, board_pos_x, board_pos_y, board_size_px, square_size_px, renderer);
        // render in the screen all changes above
        if( (fimJogo(nJogadas, nPontos)) == 1)
        {
            vitoria = 1;
            renderVitoria(renderer, capture); // aparece no ecrã vitória
        }
        else if ((fimJogo(nJogadas, nPontos)) == -1)
        {
            derrota = 1;
            renderDerrota(renderer, capture); // aparece no ecrã derrota
        }

        SDL_RenderPresent(renderer);
        // add a delay
        SDL_Delay( delay );

    }

    // free memory allocated for images and textures and closes everything including fonts
    TTF_CloseFont(serif);
    TTF_CloseFont(capture);
    SDL_FreeSurface(imgs[0]);
    SDL_FreeSurface(imgs[1]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

/**
 * ProcessMouseEvent: gets the square pos based on the click positions !
 * \param _mouse_pos_x position of the click on pixel coordinates
 * \param _mouse_pos_y position of the click on pixel coordinates
 * \param _board_size_px size of the board !
 * \param _square_size_px size of each square
 * \param _pt_x square nr
 * \param _pt_y square nr
 */
void ProcessMouseEvent(int _mouse_pos_x, int _mouse_pos_y, int _board_size_px[], int _square_size_px,
        int *_pt_x, int *_pt_y )
{
    // corner of the board
    int x_corner = (TABLE_SIZE - _board_size_px[0]) >> 1;
    int y_corner = (TABLE_SIZE - _board_size_px[1] - 15);

    // verify if valid cordinates
    if (_mouse_pos_x < x_corner || _mouse_pos_y < y_corner || _mouse_pos_x > (x_corner + _board_size_px[0])
        || _mouse_pos_y > (y_corner + _board_size_px[1]) )
    {
        *_pt_x = -1;
        *_pt_y = -1;
        return;
    }

    // computes the square where the mouse position is
    _mouse_pos_x = _mouse_pos_x - x_corner;
    _mouse_pos_y = _mouse_pos_y - y_corner;

    *_pt_x = _mouse_pos_x / (_square_size_px + SQUARE_SEPARATOR);
    *_pt_y = _mouse_pos_y / (_square_size_px + SQUARE_SEPARATOR);
}

/**
 * RenderPoints: renders the board
 * \param _board 2D array with integers representing board colors
 * \param _board_pos_x number of positions in the board (x axis)
 * \param _board_pos_y number of positions in the board (y axis)
 * \param _square_size_px size of each square
 * \param _board_size_px size of the board
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderPoints(int _board[][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y,
        int _board_size_px[], int _square_size_px, SDL_Renderer *_renderer )
{
    int clr, x_corner, y_corner, circleX, circleY, circleR;

    // corner of the board
    x_corner = (TABLE_SIZE - _board_size_px[0]) >> 1;
    y_corner = (TABLE_SIZE - _board_size_px[1] - 15);

    // renders the squares where the dots will appear
    for ( int i = 0; i < _board_pos_x; i++ )
    {
        for ( int j = 0; j < _board_pos_y; j++ )
        {
                // define the size and copy the image to display
                circleX = x_corner + (i+1)*SQUARE_SEPARATOR + i*(_square_size_px)+(_square_size_px>>1);
                circleY = y_corner + (j+1)*SQUARE_SEPARATOR + j*(_square_size_px)+(_square_size_px>>1);
                circleR = (int)(_square_size_px*0.4f);
                // draw a circle
                clr = _board[i][j];
                filledCircleRGBA(_renderer, circleX, circleY, circleR, colors[0][clr], colors[1][clr], colors[2][clr]);
        }
    }
}

/**
 * filledCircleRGBA: renders a filled circle
 * \param _circleX x pos
 * \param _circleY y pos
 * \param _circleR radius
 * \param _r red
 * \param _g green
 * \param _b blue
 */
void filledCircleRGBA(SDL_Renderer * _renderer, int _circleX, int _circleY, int _circleR, int _r, int _g, int _b)
{
    int off_x = 0;
    int off_y = 0;
    float degree = 0.0;
    float step = M_PI / (_circleR*8);

    SDL_SetRenderDrawColor(_renderer, _r, _g, _b, 255);

    while (_circleR > 0)
    {
        for (degree = 0.0; degree < M_PI/2; degree+=step)
        {
            off_x = (int)(_circleR * cos(degree));
            off_y = (int)(_circleR * sin(degree));
            SDL_RenderDrawPoint(_renderer, _circleX+off_x, _circleY+off_y);
            SDL_RenderDrawPoint(_renderer, _circleX-off_y, _circleY+off_x);
            SDL_RenderDrawPoint(_renderer, _circleX-off_x, _circleY-off_y);
            SDL_RenderDrawPoint(_renderer, _circleX+off_y, _circleY-off_x);
        }
        _circleR--;
    }
}

/*
 * Shows some information about the game:
 * - Goals of the game
 * - Number of moves
 * \param _renderer renderer to handle all rendering in a window
 * \param _font font to display the scores
 * \param _goals goals of the game
 * \param _ncolors number of colors
 * \param _moves number of moves remaining
 */
void RenderStats( SDL_Renderer *_renderer, TTF_Font *_font, int _goals[], int _ncolors, int _moves)
{
    SDL_Color branco = {255, 255, 255};     // criação da cor de escrita
    int posx = 155;
    int i = 0;

    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);    //criação do fundo preto
    SDL_Rect numJogadas = {70, 65, 120, 40};             // posicionamento do retangulo do número de jogadas
    SDL_RenderFillRect(_renderer, &numJogadas);         //criação do retangulo onde vai estar o número de jogadas restantes
    char totalJogadas[3];                                   // declaração da string a escrever
    sprintf(totalJogadas, "%d", _moves);                    // conversão do número de jogadas para string
    RenderText(80,70,"Nr jogadas:", _font, &branco, _renderer); 
    RenderText(160,70,totalJogadas, _font, &branco, _renderer); //escrever o valor das jogadas

    for(i = 0; i < _ncolors; i++)
    {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);    // inicialização do fundo do retangulo a preto
        SDL_Rect rectangulo = {posx + 50, 65, 60, 40};      //posicionamento do retangulo
        SDL_RenderFillRect(_renderer, &rectangulo);         //desenha o retangulo
        char moveCores[3];                                  // declaração da string para o número de pontos por cor
        sprintf(moveCores, "%d", _goals[i]);                // conversão do número de pontos para string
        RenderText(posx + 85, 70, moveCores, _font, &branco, _renderer);     // escrever o número de cores para cada cor
        filledCircleRGBA(_renderer, posx + 65, 85, 12, colors[0][i], colors[1][i], colors[2][i]); // desenhar uma circulo para cada cor

        posx+=140;
    }
}

/*
 * RenderTable: Draws the table where the game will be played, namely:
 * -  some texture for the background
 * -  the right part with the IST logo and the student name and number
 * -  the grid for game board with squares and seperator lines
 * \param _board_pos_x number of positions in the board (x axis)
 * \param _board_pos_y number of positions in the board (y axis)
 * \param _board_size_px size of the board
 * \param _font font used to render the text
 * \param _img surfaces with the table background and IST logo (already loaded)
 * \param _renderer renderer to handle all rendering in a window
 * \param _pos_x_cong posição em X do ponto congelado
 * \param _pos_y_cong posição em Y do ponto congelado
 * \param _varInicio variável que deteta se o jogo já começou
 * \param _matrizCoord[][] matriz das coordenadas da jogada
 */
int RenderTable( int _board_pos_x, int _board_pos_y, int _board_size_px[],
        TTF_Font *_font, SDL_Surface *_img[], SDL_Renderer* _renderer, int _pos_x_cong, int _pos_y_cong, int _varInicio, int _matrizCoord[2][MAX_MOVE],
        int _linhaCoord, int _corIni)
{
    SDL_Color black = { 0, 0, 0 }; // black
    SDL_Color light = { 205, 193, 181 };
    SDL_Color dark = { 120, 110, 102 };
    SDL_Texture *table_texture;
    SDL_Rect tableSrc, tableDest, board, board_square;
    int height, board_size, square_size_px, max_pos;
    int k = 0;

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
    max_pos = MAX(_board_pos_x, _board_pos_y);
    board_size = (int)(BOARD_SIZE_PER*TABLE_SIZE);
    square_size_px = (board_size - (max_pos+1)*SQUARE_SEPARATOR) / max_pos;
    _board_size_px[0] = _board_pos_x*(square_size_px+SQUARE_SEPARATOR)+SQUARE_SEPARATOR;
    _board_size_px[1] = _board_pos_y*(square_size_px+SQUARE_SEPARATOR)+SQUARE_SEPARATOR;

    // renders the entire board background
    SDL_SetRenderDrawColor(_renderer, dark.r, dark.g, dark.b, dark.a );
    board.x = (TABLE_SIZE - _board_size_px[0]) >> 1;
    board.y = (TABLE_SIZE - _board_size_px[1] - 15);
    board.w = _board_size_px[0];
    board.h = _board_size_px[1];
    SDL_RenderFillRect(_renderer, &board);

    // renders the squares where the numbers will appear
    // iterate over all squares
    for ( int i = 0; i < _board_pos_x; i++ )
    {
        for ( int j = 0; j < _board_pos_y; j++ )
        {
            board_square.x = board.x + (i+1)*SQUARE_SEPARATOR + i*square_size_px;
            board_square.y = board.y + (j+1)*SQUARE_SEPARATOR + j*square_size_px;
            board_square.w = square_size_px;
            board_square.h = square_size_px;              
            SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a );
            SDL_RenderFillRect(_renderer, &board_square);
        }
    }
	// pintar o fundo da jogada atual da cor do ponto
    for(k = 0; k < _linhaCoord; k++)
    {
        board_square.x = board.x + (_matrizCoord[0][k] + 1)*SQUARE_SEPARATOR + _matrizCoord[0][k]*square_size_px;
        board_square.y = board.y + (_matrizCoord[1][k] + 1)*SQUARE_SEPARATOR + _matrizCoord[1][k]*square_size_px;
        board_square.w = square_size_px;
        board_square.h = square_size_px;
        SDL_SetRenderDrawColor(_renderer, colors[0][_corIni], colors[1][_corIni], colors[2][_corIni], 255 );
        SDL_RenderFillRect(_renderer, &board_square);
    }
    // colocar o quadrado congelado a preto apenas quando o jogo está a decorrer
    if(_varInicio == 1)
    {
        SDL_SetRenderDrawColor(_renderer, black.r, black.g, black.b, black.a );
        board_square.x = board.x + (_pos_x_cong+1)*SQUARE_SEPARATOR + _pos_x_cong*square_size_px;
        board_square.y = board.y + (_pos_y_cong+1)*SQUARE_SEPARATOR + _pos_y_cong*square_size_px;
        board_square.w = square_size_px;
        board_square.h = square_size_px;
        SDL_RenderFillRect(_renderer, &board_square);
    }


    // destroy everything
    SDL_DestroyTexture(table_texture);
    // return for later use
    return square_size_px;
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
 * \param _font1 fonte para render vitória, derrota e shuffling
 */
void InitEverything(int width, int height, TTF_Font **_font, SDL_Surface *_img[], SDL_Window** _window, SDL_Renderer** _renderer, TTF_Font **_font1)
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
    // abrir a fonte para render vitória, derrota e shuffling
    *_font1 = TTF_OpenFont("Capture_it.ttf", 90);
    if(!*_font1)
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
    window = SDL_CreateWindow( "ISTDots | Rodrigo Figueiredo | IST 90185", WINDOW_POSX, WINDOW_POSY, width, height, 0 );
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
/*  FUNÇÃO A EXECUTAR QUANDO O PROGRAMA INICIA.
    PERGUNTA AO UTILIZADOR A ALTURA E LARGURA DO TABULEIRO
    A FUNÇÃO APENAS TERMINA QUANDO TODOS OS DADOS INTRODUZIDOS FOREM VÁLIDOS
    \param _altura altura do tabuleiro
    \param _largura largura do tabuleiro
*/
void obterParametros1(int *_altura, int *_largura)
{
    char buf[STRING_SIZE] = {0};

    printf("Introduza a altura do tabuleiro [5;15]: ");
    fgets(buf, STRING_SIZE, stdin);
    sscanf(buf, "%d" ,_altura);
    while(*_altura < MIN_TABLE_SIZE || *_altura > MAX_TABLE_SIZE)
    {
        printf("O valor de altura do tabuleiro não está correto!!! Volte a introduzir um valor [5;15]:  ");
        fgets(buf, STRING_SIZE, stdin);
        sscanf(buf, "%d" ,_altura);
    }

    printf("Introduza a largura do tabuleiro [5;15]: ");
    fgets(buf, STRING_SIZE, stdin);
    sscanf(buf, "%d" ,_largura);
    while(*_largura < MIN_TABLE_SIZE || *_largura > MAX_TABLE_SIZE)
    {
        printf("O valor de comprimento do tabuleiro não está correto!!! Volte a introduzir um valor [5;15]: ");
        fgets(buf, STRING_SIZE, stdin);
        sscanf(buf, "%d" ,_largura);
    }
}

/*  FUNÇÃO A EXECUTAR QUANDO O PROGRAMA INICIA.
    PERGUNTA AO UTILIZADOR O NOME E O NÚMERO DE JOGADAS
    A FUNÇÃO APENAS TERMINA QUANDO TODOS OS DADOS INTRODUZIDOS FOREM VÁLIDOS
    \param _name nome do utilizador
    \param _nJogadas número de jogadas
*/
void obterParametros2(char _name[MAX_NAME_SIZE],  int *_nJogadas)
{
    char buf[STRING_SIZE] = {0};

    printf("Introduza o nome do utilizador sem espaços com 8 caracteres no máximo: ");
    fgets(buf, STRING_SIZE, stdin);
    sscanf(buf, "%s" ,_name);
    while(strlen(_name) == 0 || strlen(_name) >= MAX_NAME_SIZE)
    {
        printf("O nome de utilizador não está correto!!! Volte a introduzir um nome não nulo com menos que 8 caracteres:  ");
        fgets(buf, STRING_SIZE, stdin);
        sscanf(buf, "%s" ,_name);
    }

    printf("Introduza o número de jogadas [0;99]: ");
    fgets(buf, STRING_SIZE, stdin);
    sscanf(buf, "%d" ,_nJogadas);
    while(*_nJogadas < 1 || *_nJogadas > 99)
    {
        printf("O valor de número de jogadas não está correto!!! Volte a introduzir um valor [0;99]: ");
        fgets(buf, STRING_SIZE, stdin);
        sscanf(buf, "%d" ,_nJogadas);
    }
}

/*  FUNÇÃO A EXECUTAR QUANDO O PROGRAMA INICIA.
    PERGUNTA AO UTILIZADOR O NÚMERO DE CORES E O NÚMERO DE PONTOS OBJETIVO PARA CADA COR
    A FUNÇÃO APENAS TERMINA QUANDO TODOS OS DADOS INTRODUZIDOS FOREM VÁLIDOS
    \param _nCores númeo de cores para o jogo
    \param _nPontos[] número de pontos objetivo para cada cor
*/
void obterParametros3( int *_nCores, int _nPontos [])
{
    char buf[STRING_SIZE] = {0};
    int aux = 0; // variável para o vetor _nPontos


    printf("Introduza o número de cores[2;5]: ");
    fgets(buf, STRING_SIZE, stdin);
    sscanf(buf, "%d" ,_nCores);
    while(*_nCores <= 1 || *_nCores > 5)
    {
        printf("O valor número de cores não está correto!!! Volte a introduzir o número de cores [2;5]: ");
        fgets(buf, STRING_SIZE, stdin);
        sscanf(buf, "%d" ,_nCores);
    }

    for(aux = 0; aux< *_nCores; aux++)
    {
        printf("Introduza o número de pontos objetivo para a cor nr%d [0;99]: ", aux +1);
        fgets(buf, STRING_SIZE, stdin);
        sscanf(buf, "%d" ,&_nPontos[aux]);
        while(_nPontos[aux] < 1 || _nPontos[aux] > 99)
        {
            printf("O valor de número de pontos objetivo não está correto!!! Volte a introduzir um valor de pontos objetivo para a cor nr %d [0;99]: ", aux + 1);
            fgets(buf, STRING_SIZE, stdin);
            sscanf(buf, "%d" ,&_nPontos[aux]);
        }
    }
    printf("\n");
}

/*
    FUNÇÃO A EXECUTAR QUANDO FOR NECESSÁRIO GERAR CORES ALEATÓRIAMENTE NA MATRIZ DO TABULEIRO
    A FUNÇAO APENAS GERA CORES NOS PONTOS EM QUE A COR PRESENTE SEJA BRANCA
    \param _altura altura do tabuleiro
    \param _largura largura do tabuleiro
    \param _nCores númeo de cores para o jogo
    \param _board[][] matriz do tabuleiro
    \param _corIni cor da jogada atual
    \param _quadrado variável que deteta formas fechadas
*/
void gerarCores(int _altura, int _largura, int _nCores, int _board[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int _corIni, int _quadrado)
{
    int i = 0, j = 0;
    
    //precorrer todos os pontos da matriz _board
    for(i = 0; i<_altura; i++)
    {
        for(j = 0; j<_largura; j++)
        {
            if(_board[j][i] == corBranco) // = a cor da bola é branca?
            {
                if(_quadrado == 1) // se a jogada tiver sido uma forma fechada
                {   // está sempre a gerar cores enquanto que a cor gerada seja igual à cor da forma fechada eliminada
                    do
                    {
                        _board[j][i] = (rand() % _nCores);
                    }
                    while(_board[j][i] == _corIni);
                }
                else if (_quadrado == 0) // se a jogada não tiver sido uma forma fechada
                {
                    _board[j][i] = (rand() % _nCores); // gerar cores normalmente
                }
            }
        }
    }
}

/*
    FUNÇÃO PARA VERIFICAR SE UMA DETERMINADA JOGADA É VÁLIDA
    RETORNA 1 CASO A JOGADA SEJA VÁLIDA
    RETORNA 0 CASO A JOGADA NÃO SEJA VÁLIDA
    A FUNÇÃO É EXECUTADA SEMPRE QUE SE MEXE SE PRETENDE VERIFICAR A VALIDADE DA JOGADA
    \param _board[][] matriz do tabuleiro
    \param _pt_x posição X do rato
    \param _pt_y posição Y do rato
    \param _corIni cor da jogada atual    
*/
int validaMov(int _board[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int _pt_x, int _pt_y, int _corIni)
{
    if(_corIni == _board[_pt_x][_pt_y]) // ser a cor da posição do rato for igual à cor da posição inicial
    {
        return 1; // jogada válida
    }
    else
    {
        return 0; // jogada inválida
    }
}

/*
    FUNÇÃO QUE COLOCA TODOS OS PONTOS DO TABULEIRO COM A COR BRANCA
    É EXECUTADA NO INICIO DO PROGRAMA E QUANDO O UTILIZADOS PRESSIONA A TECLA N
    \param _altura altura do tabuleiro
    \param _largura largura do tabuleiro
    \param _board[][] matriz do tabuleiro
*/
void limparTab(int _altura, int _largura, int _board[MAX_TABLE_SIZE][MAX_TABLE_SIZE])
{
    int i = 0, j = 0;
    // coloca todos os pontos da _board a branco
    for(i = 0; i<_altura; i++)
    {
        for(j = 0; j<_largura; j++)
        {
            _board[j][i] = corBranco;
        }
    }
}

/*
    FUNÇÃO QUE ELIMINA OS PONTOS DE UMA DETERMINADA JOGADA E DECREMENTA O NÚMERO DE PONTOS RESTANTES DESSA COR
    VERIFICA OS PONTOS EM QUE A MATRIZBRANCA É 1 E RETIRA A COR NESSES PONTOS
    É EXECUTADA QUANDO O BOTÃO DO RATO É LARGADO
    \param _board[][] matriz do tabuleiro
    \param _matrizBranca matriz que tem 1 nos pontos que vão ser eliminados e 0 nos restantes
    \param _nPontos[] número de pontos objetivo para cada cor
*/
void eliminaPontos(int _board[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int _matrizBranca[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int _nPontos[])
{
    int i = 0, j = 0;
    // precorrer todos os pontos da board
    for (i = 0; i < MAX_TABLE_SIZE; i++)
    {
        for(j = 0; j < MAX_TABLE_SIZE; j++)
        {
            if(_matrizBranca[j][i] == 1) // se esse ponto for eliminado
            {
                if(_nPontos[_board[j][i]]<=0) // caso os objetivos dessa cor já estejam a 0 
                {
                    _nPontos[_board[j][i]] = 0; // os objetivos dessa cor vão continuar a 0
                }
                else // caso ainda tenha pontos objetivo para essa cor
                {
                    (_nPontos[_board[j][i]])--; // decrementa 1 ponto por cada ponto dessa cor que vai ser eliminado
                }
                _board[j][i] = corBranco; // coloca a board a branco
                _matrizBranca[j][i] = 0; // repõe 0 na matriz Branca
            }
        }
    }
}

/*
    FUNÇÃO QUE FAZ COM QUE OS PONTOS COM COR DESÇAM AO LONGO DAS COLUNAS E QUE OS PONTOS BRANCOS FIQUEM NA PARTE SUPERIOR DO TABULEIRO
    É EXECUTADA SEMPRE QUE O BOTÃO DO RATO É LARGADO
    \param _board[][] matriz do tabuleiro
    \param _altura altura do tabuleiro
    \param _largura largura do tabuleiro
*/
void descerPontos(int _board[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int _altura, int _largura)
{
    int i = 0, j = 0, k = 0;

    // precorrer todos os pontos da _board
    for(i = 0; i<_altura; i++)
    {
        for(j = 0; j<_largura; j++)
        {
            if (_board[j][i] == corBranco) // se o ponto for branco
            {
                for(k = i; k > 0; k--) // "agarrar" esse ponto e colocar na posição mais acima do tabuleiro
                {
                    if(_board[j][k-1] == corPreto && ((k-1) > 0)) // se o ponto acima for um ponto congelado
                    {
                        _board[j][k] = _board[j][k-2]; // o ponto fica com a cor do ponto acima do ponto congelado
                        _board[j][k-1] = corPreto; // manter o bloco congelado
                        k=k-1; // saltar a posição do bloco congelado
                    }
                    else //if (_board[j][k-1] != 6)
                    {
                        _board[j][k] = _board[j][k-1]; // colocar a cor do ponto superior no ponto atual
                    }

                }

                if(_board[j][0] == corPreto) // o primeiro bloco da coluna j é preto
                {
                    _board[j][1] = corBranco; // coloca cor branco no ponto abaixo do congelado
                    _board[j][0] = corPreto; // coloca o ponto congelado a preto
                }
                else
                {
                    _board[j][0] = corBranco; // coloca o primeiro ponto da coluna a branco
                }

            }
        }
    }
}

/*
    FUNÇÃO QUE AVALIA SE HÁ RETROCESSO DO MOVIMENTO NUMA JOGADA
    EM CASO AFIRMATIVO RETORNA -1. VAI ANDAR PARA TRÁS NA MATRIZ DAS COORDENADAS
    EM CASO NEGATIVO RETORNA 1 QUE VAI ANDAR PARA A FRENTE NA MATRIZ DAS COORDENADAS
    A FUNÇÃO É EXECUTADA QUANDO O UTILIZADOR ESTÁ A SELECIONAR PONTOS
    \param _matrizCoord[][] matriz das coordenadas da jogada
    \param _linhaCoord variável que aponta para uma coluna da _matrizCoord
*/
int retrocesso(int _matrizCoord[MAX_MOVE][MAX_MOVE], int * _linhaCoord)
{   
    // verifica se a coluna que escreveu na jogada atual é igual ao ponto dois pontos antes
    if((_matrizCoord[0][*_linhaCoord] == _matrizCoord[0][*_linhaCoord-2]) && (_matrizCoord[1][*_linhaCoord] == _matrizCoord[1][*_linhaCoord-2]))
    {
        return -1; // anda para trás no vetor das coordenadas
    }
    return 1; // anda para a frente no vetor das coordenadas
}

/*
    FUNÇÃO PARA VERIFICAR SE EXISTEM JOGADAS POSSIVEIS NO TABULEIRO
    EM CASO AFIRMATIVO RETORNA 0 E NADA ACONTECE
    EM CASO NEGATIVO RETORNA 1 E OS PONTOS NA MATRIZ TERÃO QUE SER REORDENADOS
    A FUNÇÃO SERÁ EXECUTADA SEMPRE QUE O BOTÃO DO RATO FOR LARGADO
    \param _board[][] matriz do tabuleiro
    \param _altura altura do tabuleiro
    \param _largura largura do tabuleiro

*/
int verificaJogadas(int _board[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int _altura, int _largura)
{
    int i = 0, j = 0;

    // precorre todos os valores da matriz menos a última linha e a última coluna
    for(i = 0; i < _altura - 1; i++)
    {
        for(j = 0; j < _largura - 1; j++)
        {
            // se a cor nesse ponto for igual à cor do ponto à direita ou abaixo
            if((_board[j][i] == _board[j+1][i]) || (_board[j][i] == _board[j][i+1])) 
            {
                return 0; // há jogadas possiveis
            }
        }
    }
    // precorre todos os pontos da última linha da matriz
    for(j = 0; j < _largura - 1; j++)
    {
        if((_board[j][_altura - 1] == _board[j+1][_altura - 1])) // se a cor do ponto for igual à cor do ponto à sua direita
            {
                return 0; // há jogadas possiveis
            }
    }
    // precorre todos os pontos da última coluna da matriz
    for(i = 0; i < _altura - 1; i++)
    {
        if((_board[_largura - 1][i] == _board[_largura - 1][i + 1])) // se a cor desse ponto for igual à cor do ponto abaixo
            {
                return 0; // há jogadas possiveis
            }
    }
    return 1; // não há jogadas possiveis
}

/*
    FUNÇÃO QUE REORGANIZA O TABULEIRO DO JOGO ALEATÓRIAMENTE, MANDENDO O NÚMERO DE PONTOS DE CADA COR
    A FUNÇÃO É INVOCADA QUANDO NÃO EXISTEM MAIS POSSIBILIDADES DE JOGADAS
    \param _board[][] matriz do tabuleiro
    \param _nCores númeo de cores para o jogo
    \param _altura altura do tabuleiro
    \param _largura largura do tabuleiro

*/
void reorganiza(int _board[MAX_TABLE_SIZE][MAX_TABLE_SIZE],int _nCores, int _altura, int _largura)
{
    int vectsomacores[5] = {0,0,0,0,0};
    int i = 0, j = 0 , aux = 0;

    // soma e coloca num vetor o número de cores existentes no tabuleiro para gerar o mesmo número de cores sem contar com o ponto congelado
    for(i = 0; i < _altura; i++)
    {
        for(j = 0; j < _largura; j++)
        {
            if(_board[j][i] != corPreto) // não contabilida o ponto congelado
            {
                (vectsomacores[_board[j][i]])++; // incrementa uma unidade na posição igual à cor daquele ponto
                _board[j][i] = corBranco; // coloca aquele ponto a branco para depois serem geradas novas cores
            }
        }
    }
    // atribui uma nova cor a todos os pontos da _board
    for(i = 0; i < _altura; i++)
    {
        for(j = 0; j < _largura; j++)
        {
            if(_board[j][i] != corPreto) // se o ponto não tiver a cor preta
            {
                do
                {
                    aux = (rand() % _nCores); // gerar uma nova cor
                }
                while((vectsomacores[aux] == 0)); // enquanto que ainda hajam pontos dessa cor para serem gerados
                _board[j][i] = aux;
                (vectsomacores[aux])--;         // decrementa uma unidade nos pontos restantes para serem gerados dessa cor
            }
        }
    }
}

/*
    FUNÇÃO QUE VERIFICA SE A JOGADA EFETUADA FOI UMA FORMA FECHADA
    CASO TENHA SIDO UMA FORMA FECHADA RETORNA 1, CASO CONTRÁRIO RETORNA 0
    A FUNÇÃO É INVOCADA SEMPRE QUE NÃO HÁ RETROCESSO NA JOGADA, OU SEJA, QUANDO O JOGADOR NÃO VOLTA PARA TRÁS NAS POSIÇÕES ATUAIS DO RATO
    ASSIM, EM TODAS AS JOGADAS VÁLIDAS, SEMPRE QUE O JOGADOR SELECIONA UM NOVO PONTO, SEM ESTAR A RETROCEDER, O PROGRAMA VAI VERIFICAR SE FOI UM QUADRADO
    \param _matrizCoord[][] matriz das coordenadas da jogada
    \param _pt_x posição X do rato
    \param _pt_y posição Y do rato
    \param _linhaCoord variável que aponta para uma coluna da _matrizCoord
*/

int verificaQuadrado(int _matrizCoord[MAX_MOVE][MAX_MOVE], int _pt_x, int _pt_y, int _linhaCoord)
{
    int i = 0;

    // verifica todos os pontos da matriz das coordenadas
    for(i = 0; i < _linhaCoord - 1; i++)
    {
        if((_pt_x ==_matrizCoord[0][i]) && (_pt_y == _matrizCoord[1][i])) // caso algum desses pontos seja igual à posição atual do rato
        {
            return 1; // houve forma fechada
        }
    }
    return 0; // não houve forma fechada
}

/*
    FUNÇÃO QUE, CASO A JOGADA TENHA SIDO UMA FORMA FECHADA, VAI ELIMINAR TODOS OS PONTOS DE COR IGUAL À DA FORMA FECHADA
    FUNÇÃO A EXECUTAR SEMPRE QUE O BOTÃO DO RATO É LARGADO E A JOGADA REALIZADA FOI UMA FORMA FECHADA
    \param _board[][] matriz do tabuleiro
    \param _matrizBranca matriz que tem 1 nos pontos que vão ser eliminados e 0 nos restantes
    \param _altura altura do tabuleiro
    \param _largura largura do tabuleiro
    \param _corIni cor da jogada atual
*/
void eliminaQuadrado(int _board[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int _matrizBranca[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int _altura, int _largura, int _corIni)
{
    int i = 0, j = 0;
    // precorre todos os pontos da _board e verifica se têm a mesma cor que a cor da forma fechada
    for(i = 0; i < _altura; i++)
    {
        for(j = 0; j < _largura; j++)
        {
            if(_board[j][i] == _corIni)
            {
                _matrizBranca[j][i] = 1;
            }
        }
    }
}

/*
    FUNÇÃO QUE SELECIONA TODOS OS PONTOS DA MATRIZ QUE CONTÉM AS COORDENADAS DA JOGADA E, COLOCA A 1 NA MATRIZBRANCA(MATRIZ COM 1 NOS PONTOS QUE VÃO SER ELIMINADOS)
    DESTA FORMA, ATÉ À COLUNA EM QUE O JOGADOR ESTÁ A ESCRVER NA MATRIZ DAS JOGADAS, OS PONTOS VÃO TODOS SER ELIMINADOS
    A EXECUÇÃO DESTA FUNÇÃO GARANTE QUE TODOS OS PONTOS SELECIONADOS PELO JOGADOR NAQUELA JOGADA VÃO SER ELIMINADOS QUANDO A FUNÇÃO ELIMINAPONTOS FOR EXECUTADA
    FUNÇÃO A EXECUTAR SEMPRE QUE O BOTÃO DO RATO É LARGADO
    \param _matrizCoord[][] matriz das coordenadas da jogada
    \param _matrizBranca matriz que tem 1 nos pontos que vão ser eliminados e 0 nos restantes
    \param _linhaCoord variável que aponta para uma coluna da _matrizCoord
*/
void converteCoordBranca(int _matrizCoord[MAX_MOVE][MAX_MOVE], int _matrizBranca[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int _linhaCoord)
{
    int i = 0;
    // precorre todas as colunas da _matrizCoord e coloca a matriz branca nesse ponto a 1
    for (i = 0; i < _linhaCoord; i++)
    {
        _matrizBranca[_matrizCoord[0][i]][_matrizCoord[1][i]] = 1;
    }
}

/*
    FUNÇÃO QUE DETETA OS PONTOS DENTRO DE UMA FORMA FECHADA
    A FUNÇÃO RETORNA 1 CASO SE ENCONTRE DENTRO DE UMA FORMA FECHADA E RETORNA 0 NO CASO NEGATIVO
    FUNÇÃO A EXECUTAR SEMPRE QUE VÃO SER ELIMINADOS PONTOS RESULTANTES DE UMA FORMA FECHADA
    \param _cordx coordenada em X do ponto para verificar se está dentro da forma fechada
    \param _cordy coordenada em Y do ponto para verificar se está dentro da forma fechada
    \param _matrizBranca matriz que tem 1 nos pontos que vão ser eliminados e 0 nos restantes
    \param _altura altura do tabuleiro
    \param _largura largura do tabuleiro
*/
int pontoRodeado(int _cordx, int _cordy, int _matrizBranca[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int _altura, int _largura)
{
    int i = 0;
    int checke = 0, checkd = 0, checkc = 0, checkb = 0;

    // verifica se o ponto está limitado à esquerda por um ponto que vai ser eliminado
    for(i = _cordx - 1; i >= 0; i--)
    {
        if(_matrizBranca[i][_cordy] == 1)
        {
            checke = 1; // check à esquerda = 1  
        }
    }
    // verifica se o ponto está limitado à diretia por um ponto que vai ser eliminado
    for(i = _cordx + 1; i < _largura; i++)
    {
        if(_matrizBranca[i][_cordy] == 1)
        {
            checkd = 1; // cheack à direita = 1
        }
    }
    // verifica se o ponto está limitado a cima por um ponto que vai ser eliminado
    for(i = _cordy - 1; i >= 0; i--)
    {
        if(_matrizBranca[_cordx][i] == 1)
        {
            checkc = 1; // cheack a cima = 1
        }
    }
    // verifica se o ponto está limitado a baixo por um ponto que vai ser eliminado
    for(i = _cordy + 1; i < _altura; i++)
    {
        if(_matrizBranca[_cordx][i] == 1)
        {
            checkb = 1; // cheack a baixo = 1
        }
    }

    if(checke && checkd && checkc && checkb) // se estiver limitado por todos os lados
    {
        return 1; // o ponto pertence à forma fechada
    }
    return 0; // o ponto não pertence à forma fechada
}

/*
    FUNÇÃO QUE ELIMINA PONTOS DENTRO DE UMA FORMA FECHADA
    CASO EXISTAM ESSES PONTOS SERÃO COLOCADOS A 1 NA MATRIZBRANCA
    FUNÇÃO A EXECUTAR SEMPRE QUE O UTILIZADOR EXECUTE UMA JOGADA COM UMA FORMA FECHADA
    \param _matrizBranca matriz que tem 1 nos pontos que vão ser eliminados e 0 nos restantes
    \param _altura altura do tabuleiro
    \param _largura largura do tabuleiro
    \param _board[][] matriz do tabuleiro
*/
void eliminaPontosDentroQuadrado(int _matrizBranca[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int _altura, int _largura, int _board[MAX_TABLE_SIZE][MAX_TABLE_SIZE])
{
    int i = 0, j = 0;
    // verifica os pontos da board, excluindo a borda superior, inferior, esquerda e direita, verificando apenas os pontos "interiores"
    for(i = 1; i < (_altura - 1); i++)
    {
        for(j = 1; j < (_largura - 1); j++)
        {
            if((_matrizBranca[j][i] == 0) && (_board[j][i] != 6)) // se esse ponto não tiver sido selecionado para eliminar e se não for um bloco congelado
            _matrizBranca[j][i] = pontoRodeado(j, i, _matrizBranca, _altura, _largura); // fica a 1 se for ponto rodeado e 0 se não for
        }
    }
}

/*
    FUNÇÃO QUE GERA ALEATÓRIAMENTE UMA POSIÇÃO PARA O PONTO CONGELADO
    FUNÇÃO A EXECUTAR QUANDO O JOGO INICIA OU QUANDO O UTILIZADOR PRESSIONA A TECLA N
    \param _altura altura do tabuleiro
    \param _largura largura do tabuleiro
    \param _board[][] matriz do tabuleiro
    \param _pos_x_cong posição em X do ponto congelado
    \param _pos_y_cong posição em Y do ponto congelado
*/
void gerarCong(int _altura, int _largura, int _board[MAX_TABLE_SIZE][MAX_TABLE_SIZE], int * _pos_x_cong, int * _pos_y_cong)
{
    int x = 0, y = 0;
    // gerar uma posição aleatória para o ponto congelado
    y = (rand() % _altura);
    x = (rand() % _largura);

    * _pos_x_cong = x;
    * _pos_y_cong = y;

    _board[x][y] = corPreto; // coloca uma bola preta no ponto
}

/*
    FUNÇÃO QUE, APÓS CADA JOGADA VÁLIDA, DECREMENTA 1 UNIDADE NAS JOGADAS RESTANTES
    FUNÇÃO A EXECUTAR NO FINAL DE CADA JOGADA
    \param _nJogadas número de jogadas
    \param _linhaCoord variável que aponta para uma coluna da _matrizCoord
*/
void decrementaJogadas(int *_nJogadas, int _linhaCoord)
{
    if(_linhaCoord != 1) // se não voltou ao incio da jogada, anulando-a
    {
        (*_nJogadas) = (*_nJogadas) - 1; // decrementa 1 jogada
    }
}

/*
    FUNÇÃO QUE GUARDA ALGUNS DOS PARAMETROS INTRODUZIDOS PELO UTILIZADOR INICIALMENTE.
    GUARDA O NÚMERO DE JOGADAS E OS PONTOS OBJETIVO PARA CADA COR
    ESTES VALORES VÃO SER UTEIS QUANDO O UTILIZADOR CARREGAR NA TECLA N, POIS, DESTA FORMA, VAI INICIAR UM NOVO JOGO COM OS MESMOS PARAMETROS
    A FUNÇÃO É EXECUTADA NO INICIO DO JOGO, APÓS O UTILIZADOR INTRODUZIR OS PARÂMETROS.
    \param _nJogadas número de jogadas
    \param _nPontos[] número de pontos objetivo para cada cor
    \param _nPontosBCKUP[] número de pontos objetivo para cada cor definidos inicialmente pelo utilizador
    \param _nJogadasBCKUP número de jogadas definidos inicialmente pelo utilizador
*/
void backup(int _nJogadas, int _nPontos[MAX_COLOURS_USER], int _nPontosBCKUP[MAX_COLOURS_USER], int * _nJogadasBCKUP)
{
    _nPontosBCKUP[0] = _nPontos[0];
    _nPontosBCKUP[1] = _nPontos[1];
    _nPontosBCKUP[2] = _nPontos[2];
    _nPontosBCKUP[3] = _nPontos[3];
    _nPontosBCKUP[4] = _nPontos[4];
    *_nJogadasBCKUP = _nJogadas;
}

/*
    FUNÇÃO QUE DETETA O FIM DE UM JOGO
    RETORNA 1 CASO HAJA VITÓRIA
    RETORNA -1 CASO HAJA DERROTA
    RETORNA 0 CASO O JOGO AINDA NÃO TENHA TERMINADO
    A FUNÇÃO ESTÁ SEMPRE A SER EXECUTADA E ASSIM PERMITE SABER SE, NO PRESENTE JOGO, O UTILIZADOR GANHOU OU PERDEU
    \param _nJogadas número de jogadas
    \param _nPontos[] número de pontos objetivo para cada cor
*/
int fimJogo( int _nJogadas, int _nPontos[MAX_COLOURS_USER])
{
    //soma do vetor _nPontos
    int i = 0, soma = 0;
    for(i = 0; i < MAX_COLOURS_USER; i++)
    {
        soma += _nPontos[i];
    }
    
    if(_nJogadas == 0) // já não tem jogadas restantes
    {
        if(soma > 0) // o utilizador ainda tem objetivos para completar
        {
            return -1; //DERROTA
        }

        else if(soma == 0) // o utilizador compriu com todos os objetivos
        {
            return 1; //VITÓRIA
        }
    }
    else if(_nJogadas > 0) // ainda tem jogadas
    {
        if(soma == 0) // completou todos os objetivos
        {
            return 1; //VITÓRIA
        }
    }
    return 0;
}

/*  
    FUNÇÃO QUE ATIVA OS PARAMETROS INCIAIS DAS JOGADAS E DOS OBJETIVOS DOS PONTOS, COLOCANDO NAS VARIÁVEIS QUE SÃO ATUALIZADAS NA PARTE GRÁFICA
    FUNÇÃO A EXECUTAR SEMPRE O UTILIZADOR PRESSIONA A TECLA N
    \param _nJogadas número de jogadas
    \param _nJogadasBCKUP número de jogadas definidos inicialmente pelo utilizador
    \param _nPontos[] número de pontos objetivo para cada cor
    \param _nPontosBCKUP[] número de pontos objetivo para cada cor definidos inicialmente pelo utilizador
*/
void ativaBACKUP(int *_nJogadas, int _nJogadasBCKUP, int _nPontos[MAX_COLOURS_USER], int _nPontosBCKUP[MAX_COLOURS_USER])
{
    *_nJogadas = _nJogadasBCKUP;
    _nPontos[0] = _nPontosBCKUP[0];
    _nPontos[1] = _nPontosBCKUP[1];
    _nPontos[2] = _nPontosBCKUP[2];
    _nPontos[3] = _nPontosBCKUP[3];
    _nPontos[4] = _nPontosBCKUP[4];
}

/*
    FUNÇÃO QUE LIMPA A MATRIZ QUE CONTÉM AS COORDENADAS DAQUELA JOGADA EVITANDO QUE SEJAM ELIMINADOS PONTOS NÃO DESEJADOS
    FUNÇÃO A EXECUTAR QUANDO A JOGADA É COMPLETAMENTE RETROCEDIDA, ANULANDO A MESMA
    \param _matrizCoord[][] matriz das coordenadas da jogada
*/
void limpaJogadas(int _matrizCoord[2][MAX_MOVE])
{
    int i = 0;
    // coloca todos os pontos da matiz das coordenadas a 0
    for ( i = 0; i < MAX_MOVE; i++)
    {
        _matrizCoord[0][i] = 0;
        _matrizCoord[1][i] = 0;
    }
}

/*
    FUNÇÃO QUE FAZ RENDER NO ECRÃ UM RETANGULO VERDE QUE DIZ VITÓRIA
    É EXECUTADA SEMPRE QUE O UTILIZADOR TERMINAR O JOGO E ESTE TENHA SIDO UMA VITÓRIA
    \param _renderer render tudo na janela
    \param _font fonte para mostrar o texto
*/
void renderVitoria ( SDL_Renderer *_renderer, TTF_Font *_font)
{
    SDL_Color preto = {0, 0, 0};


    SDL_SetRenderDrawColor(_renderer, 0, 255, 0, 255);    //criação do fundo verde
    SDL_Rect vitoria = {235, 440, 420, 120};             // posicionamento do retangulo de vitoria
    SDL_RenderFillRect(_renderer, &vitoria);         //criação do retangulo onde vai escrever vitória
    RenderText(240,450,"Vitoria !!!", _font, &preto, _renderer);  //escrever vitória
}

/*
    FUNÇÃO QUE FAZ RENDER NO ECRÃ UM RETANGULO VERMELHO QUE DIZ DERROTA
    É EXECUTADA SEMPRE QUE O UTILIZADOR TERMINAR O JOGO E ESTE TENHA SIDO UMA DERROTA
    \param _renderer render tudo na janela
    \param _font fonte para mostrar o texto
*/
void renderDerrota ( SDL_Renderer *_renderer, TTF_Font *_font)
{
    SDL_Color branco = {255, 255, 255};

    SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);    //criação do fundo vermelho
    SDL_Rect derrota = {200, 440, 480, 120};             // posicionamento do retangulo de derrota
    SDL_RenderFillRect(_renderer, &derrota);         //criação do retangulo onde vai escrever derrota
    RenderText(210,450,"Derrota !!!", _font, &branco, _renderer);  //escrever derrota

}

/*
    FUNÇÃO QUE FAZ RENDER NO ECRÃ UM RETANGULO AZUL QUE DIZ SHUFFLING
    É EXECUTADA SEMPRE QUE O UTILIZADOR NÃO TEM MAIS OPÇÕES DE JOGO
    \param _renderer render tudo na janela
    \param _font fonte para mostrar o texto
*/
void renderBaralha ( SDL_Renderer *_renderer, TTF_Font *_font)
{
    SDL_Color branco = {255, 255, 255};

    SDL_SetRenderDrawColor(_renderer, 0, 0, 255, 255);    //criação do fundo azul
    SDL_Rect baralha = {200, 440, 535, 120};             // posicionamento do retangulo de shuffling
    SDL_RenderFillRect(_renderer, &baralha);         //criação do retangulo onde vai escrever shuffling
    RenderText(210,450,"Shuffling !!!", _font, &branco, _renderer);  //escrever shuffling
}

/*
    FUNÇÃO QUE ESCREVE NO FICHEIRO "STATS.TXT" OS RESULTADOS DAQUELE UTILIZADOR
    ESCREVE O NÚMERO DE JOGOS QUE REALIZOU E, PARA CADA UM DELES SE FOI VITÓRIA OU DERROTA
    BEM COMO O NÚMERO DE JOGADAS QUE UTILIZOU PARA O FINALIZAR
    É EXECUTADA QUANDO O UTILIZADOR PRESSIONA A TECLA Q
    \param _name nome do utilizador
    \param _vectFicheiro[] contém se ocorreu uma vitória ou derrota e o número de jogadas que utilizou
    \param _pos posição em que está a escrever no vetor dos ficheiros

*/
void escreverFicheiros(char _name[MAX_NAME_SIZE], int _vectFicheiro[MAX_NUM_GAMES], int _pos)
{
    FILE *fp;
    char nomeFicheiro []= "stats.txt";
    int nJogos = 1;
    int nVitorias = 0;
    int nDerrotas = 0;
    int i = 0;
    
    for (i = 0; i < _pos; i = i + 2)
    {
		if(_vectFicheiro[i] == 0)
		{
			nDerrotas++;
		}
		if(_vectFicheiro[i] == 1)
		{
			nVitorias++;
		}
	}

    fp = fopen (nomeFicheiro, "w");
    if (fp == NULL)
    {
        printf("Impossível de abrir o ficheiro %s\n", nomeFicheiro);
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "-- Jogador %s\n", _name); // escrever o nome de utilizador
    fprintf(fp, "-- %d jogos: %d vitorias e %d derrotas.\n", _pos/2, nVitorias, nDerrotas); // escrever o número de vitórias e derrotas
    
    for(i = 0; i < _pos; i = i + 2)
    {
        if(_vectFicheiro[i] == 0)
        {
            fprintf(fp, "\t -- %dº jogo : D\n", nJogos); //escrever derrota
        }
        if(_vectFicheiro[i] == 1)
        {
            fprintf(fp, "\t -- %dº jogo: %d V\n",nJogos, _vectFicheiro[i + 1]); // escrever vitória e o número de jogadas utilizadas
        }
        nJogos++;
    }
    fclose(fp);
}

/*
	FUNÇÃO QUE FAZ RENDER NO ECRÃ UM RETÂNGULO COM INTRUÇÕES DE JOGO
	É CONSTANTEMENTE EXECUTADA PARA MANTER A PARTE GRÁFICA ATUALIZADA
    \param _renderer render tudo na janela
    \param _font fonte para mostrar o texto
*/ 
void renderInstrucoes(SDL_Renderer *_renderer, TTF_Font *_font)
{
	SDL_Color preto = {0, 0, 0};
	SDL_Color light = { 205, 193, 181 };
	char texto1[STRING_SIZE] = "-- Bem vindo ao jogo ISTDots!";
	char texto2[STRING_SIZE] = "-- Para comecar um novo jogo carregue na tecla N";
	char texto3[STRING_SIZE] = "-- Se desejar sair da aplicacao carregue, em qualquer momento na tecla Q";
	char texto4[STRING_SIZE] = "-- Para ganhar elimine os pontos da cor acima num numero igual ou inferior as jogadas restantes";
	char texto5[STRING_SIZE] = "-- Apos uma vitoria ou derrota carregue em N (novo jogo) ou Q (sair do jogo e escrever estatisticas)";
	
	
	SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a);    
    SDL_Rect instrucoes = {45, 120, 780, 110};             
    SDL_RenderFillRect(_renderer, &instrucoes);
         
    RenderText(50,120,texto1, _font, &preto, _renderer);
    RenderText(50,140,texto2, _font, &preto, _renderer);
    RenderText(50,160,texto3, _font, &preto, _renderer);  
    RenderText(50,180,texto4, _font, &preto, _renderer);  
    RenderText(50,200,texto5, _font, &preto, _renderer);
}







