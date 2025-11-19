#include <stdio.h>

typedef struct {
    int pid;        //pagina
    int page;       // num da pagina
    int rbit;       // clock
    int ocupado;    // 1 se frame estiver ocupado 
} Frame;

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
