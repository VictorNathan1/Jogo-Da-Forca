#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

typedef struct {
    unsigned int idDica;
    unsigned char palavra[201];
} Registro;

typedef struct {
    unsigned char dica[201];
} Dica;

unsigned int palavraso;

FILE* abrirArq(char nome[], char modo[]) {
    FILE *arq = fopen(nome, modo);
    if (arq == NULL) {
        printf("\nNão foi possível abrir o arquivo %s\n", nome);
        exit(1);
    }
    return arq;
}

FILE* fecharArq(FILE *arq) {
    if (arq != NULL) fclose(arq);
    return NULL;
}

void pausa() {
    printf("\nPressione ENTER para continuar...");
    getchar();
}

void exibeboneco(int membros) {
    const char *bonecos[] = {
        "\n\n\n\n\n\n\n\n\n\n",
        "\n   ***\n  *   *\n   ***\n\n\n\n\n\n\n",
        "\n   ***\n  *   *\n   ***\n    |\n    |\n    |\n\n\n\n",
        "\n   ***\n  *   *\n   ***\n   /|\n  / |\n    |\n\n\n\n",
        "\n   ***\n  *   *\n   ***\n   /|\\\n  / | \\\n    |\n\n\n\n",
        "\n   ***\n  *   *\n   ***\n   /|\\\n  / | \\\n    |\n   /\n  /\n\n",
        "\n   ***\n  *   *\n   ***\n   /|\\\n  / | \\\n    |\n   / \\\n  /   \\\n\n"
    };
    printf("%s", bonecos[membros]);
}

void resultado(char palavra[201], bool ganhou) {
    system("cls");
    printf("-------RESULTADO-------");
    printf("\n\tPALAVRA: %s", palavra);
    if (ganhou)
        printf("\n\tVOCÊ GANHOU!");
    else
        printf("\n\tVOCÊ PERDEU!");
    pausa();
}

bool procLetra(char letra, char palavraEdit[], char *esconde, char palavra[]) {
    bool erro = true;
    for (int i = 0; i < strlen(palavraEdit); i++) {
        if (letra == palavraEdit[i]) {
            erro = false;
            esconde[i] = palavra[i];
        }
    }
    return erro;
}

void sorteio() {
    FILE *arq = abrirArq("palavras.fc", "rb");
    Registro temp;
    unsigned int count = 0;
    while (fread(&temp, sizeof(temp), 1, arq) == 1) count++;
    palavraso = rand() % count;
    fecharArq(arq);
}

void jogar() {
    Registro guarda;
    Dica dicaLida;
    FILE *arq;
    char palavraEdit[201], esconde[201], letra;
    char letDig[100] = "";
    int tamanho = 0, qtdErro = 0, j = 0;
    bool fim = false, ganhou = false, achou;

    sorteio();
    arq = abrirArq("palavras.fc", "rb");
    fseek(arq, palavraso * sizeof(Registro), SEEK_SET);
    fread(&guarda, sizeof(Registro), 1, arq);
    fecharArq(arq);

    arq = abrirArq("dicas.fc", "rb");
    fseek(arq, guarda.idDica * sizeof(Dica), SEEK_SET);
    fread(&dicaLida, sizeof(Dica), 1, arq);
    fecharArq(arq);

    for (int i = 0; guarda.palavra[i]; i++) {
        if (strchr("ÁÀÃÂÄ", guarda.palavra[i])) palavraEdit[i] = 'A';
        else if (strchr("ÉÈÊË", guarda.palavra[i])) palavraEdit[i] = 'E';
        else if (strchr("ÍÌÎÏ", guarda.palavra[i])) palavraEdit[i] = 'I';
        else if (strchr("ÓÒÕÔÖ", guarda.palavra[i])) palavraEdit[i] = 'O';
        else if (strchr("ÚÙÛÜ", guarda.palavra[i])) palavraEdit[i] = 'U';
        else if (guarda.palavra[i] == 'Ç') palavraEdit[i] = 'C';
        else palavraEdit[i] = toupper(guarda.palavra[i]);
    }
    palavraEdit[strlen(guarda.palavra)] = '\0';

    for (int i = 0; palavraEdit[i]; i++) {
        esconde[i] = (palavraEdit[i] == ' ' || palavraEdit[i] == '-') ? palavraEdit[i] : '_';
        if (esconde[i] == '_') tamanho++;
    }
    esconde[strlen(palavraEdit)] = '\0';

    while (!fim) {
        system("cls");
        exibeboneco(qtdErro);
        printf("\n\tPALAVRA: %s", esconde);
        printf("\n\tLETRAS DIGITADAS: %s", letDig);
        printf("\n\tDICA: %s", dicaLida.dica);
        printf("\n\tTAMANHO: %d", tamanho);
        printf("\n\n\tDIGITE UMA LETRA: ");
        fflush(stdin);
        scanf(" %c", &letra);
        letra = toupper(letra);

        achou = false;
        for (int i = 0; i < j; i++) {
            if (letDig[i] == letra) {
                achou = true;
                break;
            }
        }

        if (!achou) {
            letDig[j++] = letra;
            letDig[j] = '\0';
            if (procLetra(letra, palavraEdit, esconde, guarda.palavra))
                qtdErro++;

            if (qtdErro >= 6)
                fim = true;
            else if (strcmp(esconde, guarda.palavra) == 0)
                fim = true, ganhou = true;
        }
    }

    resultado(guarda.palavra, ganhou);
}

void adicionarDica() {
    Dica guarda, temp;
    char resp;
    bool existe = false;

    do {
        system("cls");
        printf("\nDIGITE UMA DICA DE ATE 200 CARACTERES: ");
        fflush(stdin);
        fgets((char *)guarda.dica, 201, stdin);
        guarda.dica[strcspn((char *)guarda.dica, "\n")] = '\0';
        printf("\nDESEJA CONTINUAR COM A DICA \"%s\"? (S/N): ", guarda.dica);
        scanf(" %c", &resp);
    } while (toupper(resp) != 'S');

    FILE *arq = abrirArq("dicas.fc", "rb");
    while (fread(&temp, sizeof(temp), 1, arq) == 1) {
        if (strcmp((char *)guarda.dica, (char *)temp.dica) == 0) {
            existe = true;
            break;
        }
    }
    fecharArq(arq);

    if (!existe) {
        arq = abrirArq("dicas.fc", "ab");
        fwrite(&guarda, sizeof(Dica), 1, arq);
        fecharArq(arq);
    } else {
        printf("\nEssa dica já existe!");
    }

    pausa();
}

void adicionarPalavra() {
    Registro guarda, temp;
    Dica dicaLida;
    char resp;
    bool encontrou = false;

    system("cls");
    printf("DIGITE UMA PALAVRA: ");
    fflush(stdin);
    fgets((char *)guarda.palavra, 201, stdin);
    guarda.palavra[strcspn((char *)guarda.palavra, "\n")] = '\0';

    printf("DIGITE A DICA: ");
    fgets((char *)dicaLida.dica, 201, stdin);
    dicaLida.dica[strcspn((char *)dicaLida.dica, "\n")] = '\0';

    FILE *arq = abrirArq("dicas.fc", "rb");
    int id = 0;
    while (fread(&temp, sizeof(Dica), 1, arq) == 1) {
        if (strcmp((char *)temp.dica, (char *)dicaLida.dica) == 0) {
            encontrou = true;
            break;
        }
        id++;
    }
    fecharArq(arq);

    if (!encontrou) {
        arq = abrirArq("dicas.fc", "ab");
        fwrite(&dicaLida, sizeof(Dica), 1, arq);
        fecharArq(arq);
    }

    guarda.idDica = id;

    arq = abrirArq("palavras.fc", "ab");
    fwrite(&guarda, sizeof(Registro), 1, arq);
    fecharArq(arq);

    printf("\nPalavra e dica cadastradas com sucesso!");
    pausa();
}

void menu() {
    int op;
    do {
        system("cls");
        printf("\n-----------JOGO DA FORCA------------\n");
        printf("\n1 - JOGAR");
        printf("\n2 - ADICIONAR PALAVRA");
        printf("\n3 - ADICIONAR DICA");
        printf("\n0 - SAIR");
        printf("\nOPÇÃO: ");
        scanf("%d", &op);
        switch (op) {
            case 1: jogar(); break;
            case 2: adicionarPalavra(); break;
            case 3: adicionarDica(); break;
            case 0: break;
            default: printf("\nOpção inválida!"); pausa();
        }
    } while (op != 0);
}

int main() {
    srand(time(NULL));
    menu();
    return 0;
}
