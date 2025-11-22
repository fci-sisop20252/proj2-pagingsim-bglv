#include <stdio.h>
#include <stdlib.h>

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
}Fifo;

//procurar o processo q possui o PID inserido
int processo(Fifo *proc, int n, int pid){
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
    if(!fc) return 1;

    int num_quadros, tam_pag, num_proc;
    fscanf(fc, "%d", &num_quadros);
    fscanf(fc, "%d", &tam_pag);
    fscanf(fc, "%d", &num_proc);

    //cria um vetor p cada processo
    Fifo *proc = malloc(sizeof(Fifo) * num_proc);

    for(int i = 0; i < num_proc; i++){
        int pid, tam_virtual;
        fscanf(fc,"%d %d", &pid, &tam_virtual);

        proc[i].pid = pid; // guarda pid no processo
        proc[i].num_pag = (tam_virtual + tam_pag - 1) / tam_pag; //qnts paginas o proc precisa
        proc[i].tabela = calloc(proc[i].num_pag, sizeof(EntradaPagina)); //cria a tabela de pag do proc

    }

    fclose(fc);

    Frame *quadros = calloc(num_quadros, sizeof(Frame));
    int *livres = malloc(sizeof(int) * num_quadros);
    int topo_livre = num_quadros;

    for(int i = 0; i < num_quadros; i++){
        livres[i] = 1;
    }
    or (int i = 0; i < num_quadros; i++)
        livres[i] = i;

    int clock = 0;

    int hits = 0, faults = 0, subs = 0;

    FILE *fa = fopen(acc, "r");
    if (!fa) return 1;

    int pid, end;

    while (fscanf(fa, "%d %d", &pid, &end) == 2) {

        int idx = buscar_processo(procs, num_procs, pid);
        if (idx < 0) continue;

        Processo *p = &procs[idx];

        int pagina = end / tam_pag;
        int desloc = end % tam_pag;

        printf("PID %d END %d -> pag %d des %d : ", pid, end, pagina, desloc);

        if (p->tabela[pagina].valid) {
            int q = p->tabela[pagina].frame;
            quadros[q].rbit = 1;
            hits++;
            printf("HIT (quadro %d)\n", q);
            continue;
        }

        faults++;

        if (topo_livre > 0) {

            int q = livres[--topo_livre];

            quadros[q].ocupado = 1;
            quadros[q].pid = pid;
            quadros[q].pagina = pagina;
            quadros[q].rbit = 1;

            p->tabela[pagina].valid = 1;
            p->tabela[pagina].frame = q;

            printf("PAGE FAULT (livre quadro %d)\n", q);
            continue;
        }

        subs++;

        int vitima;

        if (strcmp(alg, "fifo") == 0) {
            vitima = 0;
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
        int pag_v = quadros[vitima].pagina;

        int idxv = buscar_processo(procs, num_procs, pid_v);
        procs[idxv].tabela[pag_v].valid = 0;

        quadros[vitima].pid = pid;
        quadros[vitima].pagina = pagina;
        quadros[vitima].rbit = 1;

        p->tabela[pagina].valid = 1;
        p->tabela[pagina].frame = vitima;

        printf("PAGE FAULT (substituiu quadro %d)\n", vitima);
    }

    fclose(fa);

    printf("\n=== RESUMO ===\n");
    printf("HITs: %d\n", hits);
    printf("PAGE FAULTs: %d\n", faults);
    printf("SUBSTITUICOES: %d\n", subs);

    return 0;
}

