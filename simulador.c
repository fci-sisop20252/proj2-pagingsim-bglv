#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256

typedef struct {
    int atual;       // 1 se a pag esta na RAM
    int frame;       // indice do frame onde a pag esta
} EntradaPagina;

typedef struct {
    int pid;        //pagina
    int pag;       // num da pagina
    int rbit;       // bit para o clock
    int ocupado;    // 1 se frame estiver ocupado, 0 se livre
} Frame;

typedef struct{
    int pid;
    int num_pag;
    EntradaPagina *tabela; //vetor da pag virtual
}Processo;

//procurar o processo q possui o PID inserido
int buscarProcesso(Processo *proc, int n, int pid){
    for(int i = 0; i < n; i++){
        if(proc[i].pid == pid){
            return i;
        }
    }
    return -1;
}

//argv precisa ser ponteiro p acessar a string inserida pelo usuario
int main(int argc, char *argv[]){
    if (argc != 4) {
        printf("Uso: %s <algoritmo> <config> <acessos>\n", argv[0]);
        printf("algoritmo: fifo | clock\n");
        return 1;
    }

    char *algoritmo = argv[1];
    char *config = argv[2];
    char *accesso = argv[3];

    FILE *fc = fopen(config, "r");
    if (!fc) { 
        perror("ERRO - abrir config"); 
        return 1; 
    }

    int num_quadros, tam_pag, num_proc;
    if (fscanf(fc, "%d", &num_quadros) != 1) { fclose(fc); return 1; }
    if (fscanf(fc, "%d", &tam_pag) != 1) { fclose(fc); return 1; }
    if (fscanf(fc, "%d", &num_proc) != 1) { fclose(fc); return 1; }

    // !!!!TESTAR SUB POR!!!!!
    //int num_quadros, tam_pag, num_procs;
    //fscanf(fc, "%d %d %d", &num_quadros, &tam_pag, &num_procs);

    //cria um vetor p cada processo
    Processo *proc = malloc(sizeof(Processo) * num_proc);
    if (!proc) { fclose(fc); return 1; }

    for(int i = 0; i < num_proc; i++){
        int pid, tam_virtual;
        fscanf(fc,"%d %d", &pid, &tam_virtual);

        proc[i].pid = pid; // guarda pid no processo
        proc[i].num_pag = (tam_virtual + tam_pag - 1) / tam_pag; //qnts paginas o proc precisa
        proc[i].tabela = calloc(proc[i].num_pag, sizeof(EntradaPagina)); //cria a tabela de pag do proc

    }

    fclose(fc);

    Frame *quadros = calloc(num_quadros, sizeof(Frame));

    int proximo_livre = 0;
    int quadros_ocupados = 0;
    
    int clock = 0, hits = 0, faults = 0, subs = 0;

    FILE *fa = fopen(accesso, "r");
    if (!fa) {
        printf("ERRO - abrir acessos");
        return 1;
    }

    int pid, end;

    while (fscanf(fa, "%d %d", &pid, &end) == 2) {
        int aux = buscarProcesso(proc, num_proc, pid);
        if (aux < 0) continue;

        Processo *p = &proc[aux];

        int pag = end / tam_pag;
        int desloc = end % tam_pag;

        printf("Acesso: PID %d, Endereço %d (Página %d, Deslocamento %d) -> ", pid, end, pag, desloc);

        if (p -> tabela[pag].atual) {
            int f = p -> tabela[pag].frame;
            quadros[f].rbit = 1;
            hits++;
            printf("HIT: Página %d (PID %d) já está no Frame %d\n", pag, pid, f);
            continue;
        }

        faults++;

        if (quadros_ocupados < num_quadros) {

            int q = proximo_livre; // Frame a ser alocado (0, 1, 2...)
            
            proximo_livre++;
            quadros_ocupados++;

            quadros[q].ocupado = 1;
            quadros[q].pid = pid;
            quadros[q].pag = pag;
            quadros[q].rbit = 1;

            p->tabela[pag].atual = 1;
            p->tabela[pag].frame = q;

            printf("PAGE FAULT -> Página %d (PID %d) alocada no Frame livre %d\n",
       pag, pid, q);
            continue;
        }

        subs++;
        int vitima;

        static int ponteiro_fifo = 0;

        if (strcmp(algoritmo, "fifo") == 0) {
            vitima = ponteiro_fifo;
            ponteiro_fifo = (ponteiro_fifo + 1) % num_quadros;
        }


        else {
            while (1) {
                if (quadros[clock].rbit == 0) {
                    vitima = clock;
                    clock = (clock + 1) % num_quadros;
                    break;
                }
                quadros[clock].rbit = 0;
                clock = (clock + 1) % num_quadros;
            }
        }

        int pid_v = quadros[vitima].pid;
        int pag_v = quadros[vitima].pag;

        int auxv = buscarProcesso(proc, num_proc, pid_v);
        proc[auxv].tabela[pag_v].atual = 0;

        quadros[vitima].pid = pid;
        quadros[vitima].pag = pag;
        quadros[vitima].rbit = 1;

        p->tabela[pag].atual = 1;
        p->tabela[pag].frame = vitima;

        printf("PAGE FAULT -> Memória cheia. Página %d (PID %d) (Frame %d) será desalocada. \
-> Página %d (PID %d) alocada no Frame %d\n", pag_v, pid_v, vitima, pag, pid, vitima);

    }

    fclose(fa);

    printf("--- Simulação Finalizada (Algoritmo: %s)\n", algoritmo);
    printf("Total de Acessos: %d\n", hits + faults);
    printf("Total de Page Faults: %d\n", faults);


    return 0;
}

