#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string.h>

#define LINHAS 100 // Define o máximo de linhas do txt
#define TAMANHO 100 // Define o tamanho máximo de uma linha do txt
#define LARGURA_TELA 640 //Define a largura da janela
#define ALTURA_TELA 480 // Define a altura da janela
#define TAMANHO_BLOCO 32 // Define o tamanho do bloco
#define LARGURA_MAPA 11 // Define a largura do mapa
#define ALTURA_MAPA 11 // Define a altura do mapa

char mapa[LARGURA_MAPA][ALTURA_MAPA]; // Definindo o mapa

enum StatusEntidade
{
    vivo,
    morto
}; // Lógica de jogador/inimigo

enum ultimoMov
{
    esquerda,
    direita,
    frente,
    tras
}; // Lógica de jogador/inimigo

enum StatusJogo
{
    naoIniciado,
    aguardandoNome,
    vitoria,
    derrota,
    pausado,
    iniciado
}; // Status pra lógica do jogo

typedef struct {
    int x;
    int y;
    enum StatusEntidade status;
    enum ultimoMov ultimoMov;
} Entidade; // Estrutura pra movimentar jogador / inimigo

typedef struct {
    int x;
    int y;
} Bomba; // Estrutura pra bomba

Entidade jogador;

// Definindo status iniciais de cada item do jogo:

//Entidade jogador = {jogador.texture_up ,jogador.texture_up , jogador.texture_down, jogador.texture_left, jogador.texture_right, 1 ,1, vivo};
Entidade jogador = {1 ,1, vivo};
Entidade inimigos[3] = {{9, 9, vivo}, {9, 9, vivo}, {9, 9, vivo}};
Bomba bomba = {-1, -1}; // Coordenadas da bomba, inicialmente fora do mapa

// Definindo variáveis do jogo:

int pontuacao_inicial = 0;
int pontuacao_final = 0;
int temporizadorBomba = 0; // Temporizador para detonar a bomba
int menuPrincipal = 0; // 1: Iniciar Jogo, 2: Placar, 3: Sair
enum StatusJogo statusJogo = naoIniciado; // Indica se o jogo está em iniciado, não iniciado ou aguardando o nome do jogador
char nomeJogador[20] = "";
char auxNomeJogador[20];
int numInimigos = 1; // Número de inimigos que será gerado (baseado na dificuldade)
int salvo = 0;
int contadorRecordes;

// Inicializa o mapa com paredes e espaços vazios
void inicializarMapa() {
    for (int i = 0; i < LARGURA_MAPA; i++) {
        for (int j = 0; j < ALTURA_MAPA; j++) {
            if (i == 0 || i == LARGURA_MAPA - 1 || j == 0 || j == ALTURA_MAPA - 1) {
                mapa[i][j] = '#'; // Parede
            } else if (i % 2 == 0 && j % 2 == 0) {
                mapa[i][j] = '#'; // Obstáculo
            } else {
                mapa[i][j] = ' '; // Espaço vazio
            }
        }
    }
}

void desenharfundo(SDL_Renderer* renderizador, SDL_Surface* fundo){
    SDL_Rect retangulo;
    retangulo.w = TAMANHO_BLOCO;
    retangulo.h = TAMANHO_BLOCO;

    SDL_Texture* textura = NULL;
    SDL_Surface* superficie = NULL;
    superficie = fundo;
    textura = SDL_CreateTextureFromSurface(renderizador, superficie);
    SDL_FreeSurface(superficie);

    SDL_Rect fundoRect;
    fundoRect.x = 0;
    fundoRect.y = 0;
    fundoRect.w = LARGURA_TELA;
    fundoRect.h = ALTURA_TELA;
    SDL_RenderCopy(renderizador, textura, NULL, &fundoRect);
    SDL_DestroyTexture(textura);

}
// Desenha o mapa na tela
void desenharMapa(SDL_Renderer* renderizador) {
    SDL_Rect retangulo;
    retangulo.w = TAMANHO_BLOCO;
    retangulo.h = TAMANHO_BLOCO;

    int offsetX = (LARGURA_TELA - LARGURA_MAPA * TAMANHO_BLOCO) / 2;
    int offsetY = (ALTURA_TELA - ALTURA_MAPA * TAMANHO_BLOCO) / 2;

    SDL_Texture* textura = NULL;
    SDL_Surface* superficie = NULL;
    superficie = IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\areia.jpg");
    textura = SDL_CreateTextureFromSurface(renderizador, superficie);
    SDL_FreeSurface(superficie);

    SDL_Rect fundoRect;
    fundoRect.x = offsetX;
    fundoRect.y = offsetY;
    fundoRect.w = LARGURA_MAPA * TAMANHO_BLOCO;
    fundoRect.h = ALTURA_MAPA * TAMANHO_BLOCO;
    SDL_RenderCopy(renderizador, textura, NULL, &fundoRect);

    for (int i = 0; i < LARGURA_MAPA; i++) {
        for (int j = 0; j < ALTURA_MAPA; j++) {
            retangulo.x = offsetX + j * TAMANHO_BLOCO;
            retangulo.y = offsetY + i * TAMANHO_BLOCO;

            if (mapa[i][j] == '#') {
                SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
                SDL_RenderFillRect(renderizador, &retangulo);
            }
        }
    }
}

// Desenha um item na tela (jogador, inimigo, bomba)
void desenharInimigo(SDL_Renderer* renderizador, Entidade inimigo) {
    if(inimigo.status == vivo)
    {
        SDL_Texture* textura = NULL;
        SDL_Surface* superficie = NULL;
        if(inimigo.ultimoMov == frente)
        {
            superficie = IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\inimigo_baixo.png");
        }
        else if(inimigo.ultimoMov == tras)
        {
            superficie = IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\inimigo_cima.png");
        }
        else if(inimigo.ultimoMov == esquerda)
        {
            superficie = IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\inimigo_esquerda.png");
        }
        else if(inimigo.ultimoMov == direita)
        {
            superficie = IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\inimigo_direita.png");
        }
        textura = SDL_CreateTextureFromSurface(renderizador, superficie);
        SDL_FreeSurface(superficie);


        SDL_Rect retangulo;
        int eixoX = (LARGURA_TELA - LARGURA_MAPA * TAMANHO_BLOCO) / 2;
        int eixoY = (ALTURA_TELA - ALTURA_MAPA * TAMANHO_BLOCO) / 2;
        retangulo.x = eixoX + inimigo.x * TAMANHO_BLOCO;
        retangulo.y = eixoY + inimigo.y * TAMANHO_BLOCO;

        retangulo.w = TAMANHO_BLOCO;
        retangulo.h = TAMANHO_BLOCO;

        SDL_RenderCopy(renderizador, textura, NULL, &retangulo);
    }
}

void desenharJogador(SDL_Renderer* renderizador, Entidade jogador) {
    if(jogador.status == vivo)
    {
        SDL_Texture* textura = NULL;
        SDL_Surface* superficie = NULL;
        if(jogador.ultimoMov == frente)
        {
            superficie = IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\tras.png");
        }
        else if(jogador.ultimoMov == tras)
        {
            superficie = IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\frente.png");
        }
        else if(jogador.ultimoMov == esquerda)
        {
            superficie = IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\esquerda.png");
        }
        else if(jogador.ultimoMov == direita)
        {
            superficie = IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\direita.png");
        }
        textura = SDL_CreateTextureFromSurface(renderizador, superficie);
        SDL_FreeSurface(superficie);


        SDL_Rect retangulo;
        int eixoX = (LARGURA_TELA - LARGURA_MAPA * TAMANHO_BLOCO) / 2;
        int eixoY = (ALTURA_TELA - ALTURA_MAPA * TAMANHO_BLOCO) / 2;
        retangulo.x = eixoX + jogador.x * TAMANHO_BLOCO;
        retangulo.y = eixoY + jogador.y * TAMANHO_BLOCO;

        retangulo.w = TAMANHO_BLOCO;
        retangulo.h = TAMANHO_BLOCO;

        SDL_RenderCopy(renderizador, textura, NULL, &retangulo);
    }
}

void desenharBomba(SDL_Renderer* renderizador, Bomba bomba){
    SDL_Texture* textura = NULL;
    SDL_Surface* superficie = IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\bomba.png");
    textura = SDL_CreateTextureFromSurface(renderizador, superficie);
    SDL_FreeSurface(superficie);

    SDL_Rect retangulo;
    int eixoX = (LARGURA_TELA - LARGURA_MAPA * TAMANHO_BLOCO) / 2;
    int eixoY = (ALTURA_TELA - ALTURA_MAPA * TAMANHO_BLOCO) / 2;
    retangulo.x = eixoX + bomba.x * TAMANHO_BLOCO;
    retangulo.y = eixoY + bomba.y * TAMANHO_BLOCO;

    retangulo.w = TAMANHO_BLOCO;
    retangulo.h = TAMANHO_BLOCO;

    SDL_RenderCopy(renderizador, textura, NULL, &retangulo);
}

void desenharBombaExplodida(SDL_Renderer* renderizador, Bomba bomba){
    SDL_Texture* textura = NULL;
    SDL_Surface* superficie = IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\bombaexplodida.png");
    textura = SDL_CreateTextureFromSurface(renderizador, superficie);
    SDL_FreeSurface(superficie);

    SDL_Rect retangulo;
    int eixoX = (LARGURA_TELA - LARGURA_MAPA * TAMANHO_BLOCO) / 2;
    int eixoY = (ALTURA_TELA - ALTURA_MAPA * TAMANHO_BLOCO) / 2;
    retangulo.x = eixoX + bomba.x * TAMANHO_BLOCO;
    retangulo.y = eixoY + bomba.y * TAMANHO_BLOCO;
//adicionando explosao da bomba, fundo de areia no mapa de jogo e inves de ter o nome do jogo no menu vai ser uma fotinha
    retangulo.w = TAMANHO_BLOCO;
    retangulo.h = TAMANHO_BLOCO;

    SDL_RenderCopy(renderizador, textura, NULL, &retangulo);
} //deus

// Move uma entidade (jogador, inimigo) no mapa
void moverEntidade(Entidade* entidade, int dx, int dy)
{
    if (mapa[entidade->x + dx][entidade->y + dy] != '#' && entidade->status == vivo) {
        entidade->x += dx;
        entidade->y += dy;
    }
}

// Move o inimigo de maneira aleatória
void mover_inimigos()
{
    for (int i = 0; i < numInimigos; i++)
    {
        int movimento;
        switch (rand() % 3 - 1){
            case -1:
                movimento = rand() % 3 - 1;
                moverEntidade(&inimigos[i], movimento, 0);
                if(movimento == -1)
                {
                    inimigos[i].ultimoMov = esquerda;
                }
                else if(movimento == 1)
                {
                    inimigos[i]. ultimoMov = direita;
                }
                break;

            case 1:
                movimento = rand() % 3 - 1;
                moverEntidade(&inimigos[i], 0, rand() % 3 - 1);
                if(movimento == -1)
                {
                    inimigos[i].ultimoMov = frente;
                }
                else if(movimento == 1)
                {
                    inimigos[i]. ultimoMov = tras;
                }
                break;
        }
    }
}

// Coloca uma bomba na posição do jogador
void colocarBomba() {
    if (bomba.x == -1 && bomba.y == -1) {
        bomba.x = jogador.x;
        bomba.y = jogador.y;
        temporizadorBomba = 0; // Pra reiniciar o temporizador
    }
}

// Detona a bomba se o temporizador atingir 3 segundos
void detonarBomba()
{
    // Verifica se passaram 3 segundos desde que a bomba foi colocada
    if (temporizadorBomba >= 10) {
        for (int i = 0; i < numInimigos; i++)
        {
            // Verifica se a bomba está no range do inimigo
            if ((bomba.x == inimigos[i].x && abs(bomba.y - inimigos[i].y) == 1) || (bomba.y == inimigos[i].y && abs(bomba.x - inimigos[i].x) == 1)) {
                inimigos[i].x = -1;
                inimigos[i].y = -1;
                inimigos[i].status = morto;
            }
        }
        bomba.x = -1;
        bomba.y = -1; // Remove a bomba do mapa
    }
}

// Confera se o jogador ganhou
void confere_vitoria()
{
    int contador_inimigos_mortos = 0;
    for (int i = 0; i < numInimigos; i++)
    {
        if(inimigos[i].status == morto)
        {
            contador_inimigos_mortos++;
        }
        if(contador_inimigos_mortos == numInimigos)
        {
            for (int i = 0; i < numInimigos; i++)
            {
                inimigos[i].x = 9;
                inimigos[i].y = 9;
                inimigos[i].status = vivo;
                jogador.x = 1;
                jogador.y = 1;
            }
            nomeJogador[0] = '\0'; //Apaga o nome do jogador
            statusJogo = vitoria;
        }
    }
}

// Confere se o jogador perdeu
void confere_derrota()
{
    for (int i = 0; i < numInimigos; i++)
    {
        if (jogador.x == inimigos[i].x && jogador.y == inimigos[0].y)
        {
            for (int i = 0; i < numInimigos; i++)
            {
                inimigos[i].x = 9;
                inimigos[i].y = 9;
                inimigos[i].status = vivo;
                jogador.x = 1;
                jogador.y = 1;
            }
            nomeJogador[0] = '\0'; // Apaga o nome do jogador
            statusJogo = derrota;
            bomba.x = -1;
            bomba.y = -1; // Remove a bomba do mapa
            pontuacao_final = pontuacao_inicial; // Reseta a pontuação
        }
    }
}

// Função auxiliar para renderizar texto
int renderizarTexto(SDL_Renderer* renderizador, TTF_Font* fonte, const char* texto, SDL_Color cor, int x, int y) {
    SDL_Surface* superficie = TTF_RenderText_Solid(fonte, texto, cor);
    if (superficie == NULL) {
        //Erro ao criar superfície de texto
        return 1; //Código de erro a partir daqui só aumenta
    }
    SDL_Texture* textura = SDL_CreateTextureFromSurface(renderizador, superficie);
    if (textura == NULL) {
        //Erro ao criar textura de texto
        SDL_FreeSurface(superficie);
        return 2;
    }
    SDL_Rect destino = {x, y, superficie->w, superficie->h};
    SDL_FreeSurface(superficie);
    SDL_RenderCopy(renderizador, textura, NULL, &destino);
    SDL_DestroyTexture(textura);
}

// Exibe o menu principal na tela
void exibirMenuPrincipal(SDL_Renderer* renderizador, TTF_Font* fonte) {
    SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);
    SDL_RenderClear(renderizador);

    desenharfundo(renderizador, IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\menu_principal.png"));

    SDL_RenderPresent(renderizador);
}

// Exibe o menu de dificuldade na tela
void exibirMenuDificuldade(SDL_Renderer* renderizador, TTF_Font* fonte) {
    SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);
    SDL_RenderClear(renderizador);

    desenharfundo(renderizador, IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\menu_dificuldades.png"));

    SDL_RenderPresent(renderizador);
}

// Exibe a tela para inserir o nome do jogador
void exibirInserirNome(SDL_Renderer* renderizador, TTF_Font* fonte) {
    SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);
    SDL_RenderClear(renderizador);

    SDL_Color corPreta = {0, 0, 0, 0}; // Preto

    desenharfundo(renderizador, IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\menu_inserirnome.png"));

    // Renderiza instruções e o nome digitado
    renderizarTexto(renderizador, fonte, "Digite seu nome:", corPreta, 200, ALTURA_TELA / 2 - 50);
    renderizarTexto(renderizador, fonte, nomeJogador, corPreta, 200, ALTURA_TELA / 2);

    SDL_RenderPresent(renderizador);
}

// Exibe o menu de pausa na tela (ainda n to iusando)
void exibirMenuPausa(SDL_Renderer* renderizador, TTF_Font* fonte) {
    SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);
    SDL_RenderClear(renderizador);

    desenharfundo(renderizador, IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\menu_jogopausado.png"));

    SDL_RenderPresent(renderizador);
}

// Atualiza a lógica do jogo a cada ciclo
void atualizarJogo() {
    // Diminui a pontuação do jogador por ciclo do jogo
    pontuacao_final--;

    // Atualiza o temporizador da bomba se estiver colocada
    if (bomba.x != -1 && bomba.y != -1) {
        temporizadorBomba++;
    }

    // Detona a bomba se o temporizador atingir 3 segundos
    detonarBomba();

    confere_vitoria(); // Confere se o jogador por acaso ganhou
    confere_derrota(); // Ou se ele perdeu
}

// Salva os dados do jogador no txt
int SalvarPlacar() {
    if(salvo == 0){
        FILE *pont_arq = fopen("PlacarJogo.txt", "a+");
        if (pont_arq == NULL) {
            // Problema em abrir arquivo
            return 3;
        }

        char valor[100];
        snprintf(valor, sizeof(valor), "%d", pontuacao_final);

        char pontuacao_str[40];
        snprintf(pontuacao_str, sizeof(pontuacao_str), "%s: %s\n", auxNomeJogador, valor);

        fprintf(pont_arq, "%s", pontuacao_str);

        salvo = 1;
        contadorRecordes ++;
        printf("ContadorRecordes = %d",contadorRecordes);
        auxNomeJogador[0] = '\0';
        fclose(pont_arq);
    }
}

typedef struct {
    char nome[LINHAS];
    int pontuacao;
} Recorde; // Struct pra definir um recorde

// Função pra ordenar o placar
int ordenar_e_salvar(const char *nomeArquivo) {
    FILE *pont_arq = fopen("PlacarJogo.txt", "r");
    if (pont_arq == NULL) {
        // Erro ao abrir o arquivo
        return 4 ;
    }

    Recorde recordes[LINHAS];
    int contador = 0;
    char line[TAMANHO];

    // Lê o txt
    while (fgets(line, sizeof(line), pont_arq)) {
        if (sscanf(line, "%[^:]: %d", recordes[contador].nome, &recordes[contador].pontuacao) == 2) {
            contador++;
        }
    }
    fclose(pont_arq);


    // Lógica pra armazenar no txt as pontuações em ordem decrescente
    for (int i = 0; i < contador - 1; i++) {
        for (int j = 0; j < contador - 1 - i; j++) {
            if (recordes[j].pontuacao < recordes[j + 1].pontuacao) {
                Recorde temp = recordes[j];
                recordes[j] = recordes[j + 1];
                recordes[j + 1] = temp;
            }
        }
    }

    // Abre o arquivo no modo de escrita para salvar os dados ordenados
    pont_arq = fopen(nomeArquivo, "w");
    if (pont_arq == NULL) {
        //Erro ao abrir o arquivo.
        return 5;
    }

    for (int i = 0; i < contador; i++) {
        fprintf(pont_arq, "%s: %d\n", recordes[i].nome, recordes[i].pontuacao);
    }
    fclose(pont_arq);
}

// Exibe o menu de placar :p
void exibirMenuPlacar(SDL_Renderer* renderizador, TTF_Font* fonte){
    ordenar_e_salvar("PlacarJogo.txt");
    SDL_Color corTextoTitulo = {0,0,0,255}; // Pretp
    SDL_Color corPrimeiro = {0, 0, 0, 255}; // Dourado
    SDL_Color corSegundo = {0, 0, 0, 255}; // Prata
    SDL_Color corTerceiro = {0, 0, 0, 255}; // Bronze

    FILE* file = fopen("PlacarJogo.txt", "r");
    while (file == NULL)
    {
        fclose(file);
        return;
    }

    char lines[3][20];
    int count = 0;  // Contador de linhas

    // Lendo até 3 linhas do arquivo pra compor o placar
    while (fgets(lines[count], 20, file) != NULL && count < 3) {
        // Remove o caractere de nova linha, se houver
        size_t len = strlen(lines[count]);
        if (len > 0 && lines[count][len - 1] == '\n') {
            lines[count][len - 1] = '\0';
        }
        count++;
    }

    fclose(file);

    // Renderiza opções do menu de pausa
    if(count == 0)
    {
        renderizarTexto(renderizador, fonte, "Ninguem ousou me ganhar ainda!", corTextoTitulo, 125, ALTURA_TELA / 2 - 50);
        renderizarTexto(renderizador, fonte, "Tenta ai e volta mais tarde.", corTextoTitulo, 150, ALTURA_TELA / 2);
    }
    if(count>0)
    {
        renderizarTexto(renderizador, fonte, lines[0], corPrimeiro, 160, ALTURA_TELA / 2 );
    }
    if(count>1)
    {
        renderizarTexto(renderizador, fonte, lines[1], corSegundo, 160, ALTURA_TELA / 2 + 50);
    }
    if(count>2)
    {
        renderizarTexto(renderizador, fonte, lines[2], corSegundo, 160, ALTURA_TELA / 2 + 100);
    }
}

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        //Erro ao inicializar o SDL
        return 6;
    }

    if (TTF_Init() == -1)
    {
        //Erro ao inicializar SDL_ttf
        return 7;
    }

    SDL_Window* janela = SDL_CreateWindow("BomberCAT  :)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, LARGURA_TELA, ALTURA_TELA, SDL_WINDOW_SHOWN);
    if (janela == NULL)
    {
        //Erro ao criar janela
        return 8;
    }

    SDL_Renderer* renderizador = SDL_CreateRenderer(janela, -1, SDL_RENDERER_ACCELERATED);
    if (renderizador == NULL)
    {
        //Erro ao criar renderizador
        return 9;
    }

    // Caminho para fonte na sua máquina (Favor substituir toda vez que abrir a pasta do projeto em um novo pc)
    TTF_Font* fonte = TTF_OpenFont("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\Arial.ttf", 42);
    if (fonte == NULL)
    {
        //Erro ao carregar a fonte
        return 10;
    }

    inicializarMapa();

    int quit = 0;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = 1;
            }
            else if (statusJogo == naoIniciado)
            {
                if(e.type == SDL_KEYDOWN)
                {
                    if (menuPrincipal == 0)
                    {
                        switch (e.key.keysym.sym) {
                            case SDLK_1:
                                menuPrincipal = 1;
                                break;
                            case SDLK_2:
                                menuPrincipal = 2;
                                break;
                            case SDLK_3:
                                quit = 1;
                                break;
                        }
                    }
                    else if (menuPrincipal == 1)
                    {
                        salvo = 0;
                        switch (e.key.keysym.sym)
                        {
                            case SDLK_1:
                                numInimigos = 1;
                                pontuacao_inicial = 1000;
                                pontuacao_final = pontuacao_inicial;
                                statusJogo = aguardandoNome;
                                SDL_StopTextInput();
                                break;
                            case SDLK_2:
                                numInimigos = 2;
                                pontuacao_inicial = 2000;
                                pontuacao_final = pontuacao_inicial;
                                statusJogo = aguardandoNome;
                                SDL_StopTextInput();
                                break;
                            case SDLK_3:
                                numInimigos = 3;
                                pontuacao_inicial = 3000;
                                pontuacao_final = pontuacao_inicial;
                                statusJogo = aguardandoNome;
                                SDL_StopTextInput();
                                break;
                        }
                    }
                    else if(menuPrincipal == 2)
                    {
                        switch (e.key.keysym.sym)
                        {
                            case SDLK_1:
                                menuPrincipal = 0;
                                break;
                        }
                    }
                }
            }
            else if(statusJogo == aguardandoNome)
            {
                SDL_StartTextInput();
                if(e.type == SDL_TEXTINPUT)
                {
                    strcat(nomeJogador, e.text.text);
                }
                else if(e.type == SDL_KEYDOWN)
                {
                    if (e.key.keysym.sym == SDLK_RETURN)
                    {
                        strcat(auxNomeJogador,nomeJogador);
                        statusJogo = iniciado;
                        SDL_StopTextInput();
                    }
                    else if(e.key.keysym.sym == SDLK_BACKSPACE && strlen(nomeJogador) > 0)
                    {
                        nomeJogador[strlen(nomeJogador) - 1] = '\0';
                    }
                }
            }
            else if (statusJogo == iniciado)
            {
                if(e.type == SDL_KEYDOWN)
                {
                    switch (e.key.keysym.sym)
                    {
                        case SDLK_UP:
                            moverEntidade(&jogador, 0, -1);
                            jogador.ultimoMov = frente;

                            break;
                        case SDLK_DOWN:
                            moverEntidade(&jogador, 0, 1);
                            jogador.ultimoMov = tras;

                            break;
                        case SDLK_RIGHT:
                            moverEntidade(&jogador, 1, 0);
                            jogador.ultimoMov = direita;

                            break;
                        case SDLK_LEFT:
                            moverEntidade(&jogador, -1, 0);
                            jogador.ultimoMov = esquerda;

                            break;
                        case SDLK_SPACE:
                            colocarBomba();
                            break;
                        case SDLK_ESCAPE:
                            statusJogo = pausado;
                    }

                }
            }
            else if (statusJogo == pausado)
            {
                if(e.type == SDL_KEYDOWN)
                {
                    switch(e.key.keysym.sym)
                    {
                        case SDLK_1:
                            statusJogo = iniciado;
                            break;
                        case SDLK_2:
                            statusJogo = naoIniciado;
                            menuPrincipal = 0;
                            nomeJogador[0] = '\0';
                            break;
                    }
                }
            }
            else if(statusJogo == vitoria)
            {
                SalvarPlacar();
                if(e.type == SDL_KEYDOWN)
                {
                    switch(e.key.keysym.sym)
                    {
                        case SDLK_1:
                            statusJogo = naoIniciado;
                            menuPrincipal = 0;
                            break;
                    }
                }
            }
            else if(statusJogo == derrota)
            {
                if(e.type == SDL_KEYDOWN)
                {
                    switch (e.key.keysym.sym)
                    {
                        case SDLK_1:
                            statusJogo = iniciado;
                            break;
                        case SDLK_2:
                            statusJogo = naoIniciado;
                            menuPrincipal = 0;
                            break;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);
        SDL_RenderClear(renderizador);

        if (menuPrincipal == 0 && statusJogo == naoIniciado)
        {
            exibirMenuPrincipal(renderizador, fonte);
        }
        else if (menuPrincipal == 1 && statusJogo == naoIniciado)
        {
            exibirMenuDificuldade(renderizador, fonte);
        }
        else if (menuPrincipal == 2 && statusJogo == naoIniciado)
        {
            desenharfundo(renderizador, IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\menu_placar.png"));
            exibirMenuPlacar(renderizador, fonte);
        }
        else if (statusJogo == aguardandoNome)
        {
            exibirInserirNome(renderizador, fonte);
        }
        else if (statusJogo == pausado)
        {
            exibirMenuPausa(renderizador, fonte);
        }
        else if(statusJogo == iniciado)
        {
            atualizarJogo();
            desenharfundo(renderizador, IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\fundo.jpg"));
            desenharMapa(renderizador);
            desenharJogador(renderizador, jogador);
            for (int i = 0; i < numInimigos; i++) {
                desenharInimigo(renderizador, inimigos[i]); // Desenha os inimigos em vermelho
            }
            if (bomba.x != -1 && bomba.y != -1) {
                desenharBomba(renderizador, bomba); // Desenha a bomba em amarelo
            }
            mover_inimigos();

            SDL_RenderPresent(renderizador);
            SDL_Delay(100); // Define o tempo de "processamento"
        }
        else if (statusJogo == derrota)
        {
            desenharfundo(renderizador, IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\tela_perdeu.png"));

        }
        else if (statusJogo == vitoria)
        {
            SDL_Color corfim = {255, 255, 255, 0}; // Preto
            char valor[100];
            desenharfundo(renderizador, IMG_Load("C:\\Users\\vtcar\\Documents\\GitHub\\ProjetoBomberManApresentacao\\sdl2\\tela_ganhou.png"));

            snprintf(valor, sizeof(valor), " Pontuacao: %d", pontuacao_final);
            renderizarTexto(renderizador, fonte, valor, corfim, 210, ALTURA_TELA/2 + 80);
        }

        SDL_RenderPresent(renderizador);
    }

    TTF_CloseFont(fonte);
    fonte = NULL;

    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(janela);
    janela = NULL;
    renderizador = NULL;

    TTF_Quit();
    SDL_Quit();

    return 11;
}