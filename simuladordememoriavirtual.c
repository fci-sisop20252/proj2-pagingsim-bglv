#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 256

typedef struct {
    int pid;        //pagina
    int page;       // num da pagina
    int rbit;       // bit para o clock
    int ocupado;    // 1 se frame estiver ocupado, 0 se livre
} Frame;

typedef struct {
    int valid;       // 1 se a pag esta na RAM
    int frame;       // indice do frame onde a pag esta
    int rbit         // bit para o clock
} PageTableEntry;

//le o arq de configuracao
FILE *Fconfiguracao = fopen (argv[2], "rbit");
if (!fconfiguracao){
    perror("Erro ao abrir o arquivo de configuração");
    return 1;
}
int numero_de_frames = 0;
int tamanho_de_pagina = 0;
int numero_de_processos = 0;

fscanf(fconfig, "%d", &numero_de_frames);       // le numero de frames
fscanf(fconfig, "%d", &tamanho_da_pagina);      // le tamanho da pagina
fscanf(fconfig, "%d", &numero_de_processos);    // le numero de processos

int pids [MAX_PROCS];       //armazena pid

for (int i = 0; i < numero_de_processos; i++){
    int pid, memsize;
    int npages = memsize / tamanho_de_pagina;
    if(memsize % tamanho_de_pagina != 0) npages +=1; // para add uma pag extra se precisar
}


// cria RAM
Frame *frames = malloc (numero_de_frames * sizeof(Frame));
for (int i = o; i < numero_de_frames; i++){
    frames[i].ocupado = 0;      //frame livre
    frames[i].pid = -1;         //sem pid
    frames[i].page = -1;        //sem pag
    frames[i].rbit = 0;         //bit zerado
}


int fifo_pointer = 0; // ponteiro fifo
int clock_pointer = 0; //ponteiro clock

//FIFO
 int substituir_fifo(Frame *memoria, int num_frames){
    int vitima = fifo_pointer;
    fifo_pointer = (fifo_pointer +1) % num_frames
    return vitima;
 }
//CLOCK
int substituir_clock(Frame *memoria, int num_frames){
    while (1){
        if (memoria[clock_pointer].rbit == 0){
            int vitima = clock_pointer;
            clock_pointer = (clock_pointer + 1) % num_frames;
            return vitima;
        }
        memoria [clock_pointer].rbit = 0;
        clock_pointer = (clock_pointer + 1) % num_frames;
    }
}
