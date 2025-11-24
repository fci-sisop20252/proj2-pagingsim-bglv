# Relatório do Projeto 2: Simulador de Memória Virtual

**Disciplina:** Sistemas Operacionais
**Professor:** Lucas Figueiredo
**Data:**

## Integrantes do Grupo

- Luiza marinho de Mesquita - 10438045
- Beatriz Carvalho Di Palma - 10439477
- Guilherme Limeira De Souza - 10439777
- Vinicius Barbosa Pereira Allegretti - 10437502

---

## 1. Instruções de Compilação e Execução

### 1.1 Compilação

Descreva EXATAMENTE como compilar seu projeto. Inclua todos os comandos necessários.

**Exemplo:**
```bash
gcc -o simulador simulador.c
```

ou

```bash
make
```

### 1.2 Execução

Forneça exemplos completos de como executar o simulador.

**Exemplo com FIFO:**
```bash
./simulador fifo tests/config_1.txt tests/acessos_1.txt
```

**Exemplo com Clock:**
```bash
./simulador clock tests/config_1.txt tests/acessos_1.txt
```

---

## 2. Decisões de Design

### 2.1 Estruturas de Dados

Descreva as estruturas de dados que você escolheu para representar:

**Tabela de Páginas:**
- Qual estrutura usou? (array, lista, hash map, etc.)
Array.
- Quais informações armazena para cada página?
int atual: indica se a página está na RAM (1) ou não (0).
int frame: armazena o índice do frame na memória física onde está a página.
- Como organizou para múltiplos processos?
Cada processo possui um ponteiro (tabela) para seu próprio array em EntradaPagina, alocado dinamicamente.
- **Justificativa:** Por que escolheu essa abordagem?
O número da página é o offset dentro da tabela de páginas do processo, permitindo acesso rápido após a tradução do endereço virtual.

**Frames Físicos:**
- Como representou os frames da memória física?
Por um array chamado Frame, alocado dinamicamente.
- Quais informações armazena para cada frame?
pid: id do processo alocado no frame
pag: número da página que sestá alocada no frame (dentro do pid)
rbit: bit de referência do Clock
int ocupado: indica se o frame está em uso (1)
- Como rastreia frames livres vs ocupados?
 A variável quadros_ocupados rastreia o número de frames em uso, e a variável proximo_livre rastreia o índice do próximo frame a ser alocado sequencialmente.
- **Justificativa:** Por que escolheu essa abordagem?
A abordagem com contadores é mais simples e permite alocar frames na ordem crescente, garantindo a ordem sequencial desejada pelo sistema de testes.

**Estrutura para FIFO:**
- Como mantém a ordem de chegada das páginas?
A ordem é mantida implicitamente pelo índice do próximo frame a ser substituído, rastreado pela variável ponteiro_fifo.
- Como identifica a página mais antiga?
A página mais antiga é a página que está sendo apontada pelo ponteiro_fifo.
- **Justificativa:** Por que escolheu essa abordagem?
A abordagem FIFO é a mais fácil. Um ponteiro circular no array de frames é usado para identificar a primeira a entrar e, portanto, a próxima a sair.

**Estrutura para Clock:**
- Como implementou o ponteiro circular?
Através da variável clock, que armazena o índice do frame a ser inspecionado. Ele é incrementado e mantido dentro dos limites do array de frames usando: clock = (clock + 1) % num_quadros.
- Como armazena e atualiza os R-bits?
O R-bit é armazenado no campo rbit da estrutura Frame. Ele é setado pra 1 sempre que uma página chega no frame ou quando já está no frame e é acessada. Se o ponteiro clock inspeciona e o seu valor é 1, o algoritmo reseta para 0.
- **Justificativa:** Por que escolheu essa abordagem?
Reutilizar o array de Frame e adicionar o campo rbit é a maneira mais direta de dar às páginas uma "segunda chance" sem precisar mover entradas em uma fila ou lista.

### 2.2 Organização do Código

Descreva como organizou seu código:

- Quantos arquivos/módulos criou?
O código está organizado em um único arquivo (simulador.c).
- Qual a responsabilidade de cada arquivo/módulo?
Contém todas as estruturas de dados, as funções e o loop principal de simulação. É o módulo central que gerencia a memória virtual, traduz acessos e executa os algoritmos de substituição (FIFO e Clock).
- Quais são as principais funções e o que cada uma faz?
├── main() -  Lê argumentos, abre arquivos, inicializa estruturas (proc, quadros, livres), e executa o loop principal de simulação de acessos, chamando a função de busca e a lógica de tratamento de Page Faults/Substituição
├──buscarProcesso() - Função auxiliar para localizar o índice de um processo no array proc a partir de um pid. Percorre o vetor de processos para encontrar e retornar o índice do processo que corresponde ao PID fornecido.

**Exemplo:**
```
simulador.c
├── main() - lê argumentos e coordena execução
├── ler_config() - processa arquivo de configuração
├── processar_acessos() - loop principal de simulação
├── traduzir_endereco() - calcula página e deslocamento
├── consultar_tabela() - verifica se página está na memória
├── tratar_page_fault() - lida com page faults
├── algoritmo_fifo() - seleciona vítima usando FIFO
└── algoritmo_clock() - seleciona vítima usando Clock
```

### 2.3 Algoritmo FIFO

Explique **como** implementou a lógica FIFO:

- Como mantém o controle da ordem de chegada?
O ponteiro_fifo aponta sempre para o primeiro frame que foi alocado quando a memória encheu (a página mais antiga).
- Como seleciona a página vítima?
A vítima é o índice apontado por ponteiro_fifo.
- Quais passos executa ao substituir uma página?
Guarda o índice do frame a ser substituído, avança o ponteiro circularmente, desaloja a página antiga (vítima), aloca a nova página e atualiza a Tabela de Páginas do Processo da nova página para atual.

**Não cole código aqui.** Explique a lógica em linguagem natural.

### 2.4 Algoritmo Clock

Explique **como** implementou a lógica Clock:

- Como gerencia o ponteiro circular?
O ponteiro (clock) avança circularmente (% num_quadros) a cada inspeção, dando a impressão de um relógio.
- Como implementou a "segunda chance"?
O algortimo entra em loop até encontrar uma vítima. Se o frame atual inspecionado tem rbit == 1, significa que a página foi recentemente acessada, então o rbit é resetado para 0, e o ponteiro avança, dando à página uma "segunda chance".
- Como trata o caso onde todas as páginas têm R=1?
O ponteiro irá varrer todos os frames, resetando todos os R-bits para 0. Ao completar a volta, o ponteiro começará a inspecionar novamente, assim, o primeiro frame encontrado terá rbit = 0 e será escolhido como vítima.
- Como garante que o R-bit é setado em todo acesso?
O R-bit é setado para 1 em 2 momentos: HIT (Se a página procurada for encontrada na memória, o bit de referência daquele frame é setado) e Alocação (Seja em um frame livre ou após uma substituição, a página recém-alocada recebe o valor 1 em rbit, para garantir que ela não seja substituída imediatamente).
**Não cole código aqui.** Explique a lógica em linguagem natural.

### 2.5 Tratamento de Page Fault

Explique como seu código distingue e trata os dois cenários:

**Cenário 1: Frame livre disponível**
- Como identifica que há frame livre?
- Quais passos executa para alocar a página?

**Cenário 2: Memória cheia (substituição)**
- Como identifica que a memória está cheia?
- Como decide qual algoritmo usar (FIFO vs Clock)?
- Quais passos executa para substituir uma página?

---

## 3. Análise Comparativa FIFO vs Clock

### 3.1 Resultados dos Testes

Preencha a tabela abaixo com os resultados de pelo menos 3 testes diferentes:

| Descrição do Teste | Total de Acessos | Page Faults FIFO | Page Faults Clock | Diferença |
|-------------------|------------------|------------------|-------------------|-----------|
| Teste 1 - Básico  |                  |                  |                   |           |
| Teste 2 - Memória Pequena |          |                  |                   |           |
| Teste 3 - Simples |                  |                  |                   |           |
| Teste Próprio 1   |                  |                  |                   |           |

### 3.2 Análise

Com base nos resultados acima, responda:

1. **Qual algoritmo teve melhor desempenho (menos page faults)?**

2. **Por que você acha que isso aconteceu?** Considere:
   - Como cada algoritmo escolhe a vítima
   - O papel do R-bit no Clock
   - O padrão de acesso dos testes

3. **Em que situações Clock é melhor que FIFO?**
   - Dê exemplos de padrões de acesso onde Clock se beneficia

4. **Houve casos onde FIFO e Clock tiveram o mesmo resultado?**
   - Por que isso aconteceu?

5. **Qual algoritmo você escolheria para um sistema real e por quê?**

---

## 4. Desafios e Aprendizados

### 4.1 Maior Desafio Técnico

Descreva o maior desafio técnico que seu grupo enfrentou durante a implementação:

- Qual foi o problema?
- Como identificaram o problema?
- Como resolveram?
- O que aprenderam com isso?

### 4.2 Principal Aprendizado

Descreva o principal aprendizado sobre gerenciamento de memória que vocês tiveram com este projeto:

- O que vocês não entendiam bem antes e agora entendem?
- Como este projeto mudou sua compreensão de memória virtual?
- Que conceito das aulas ficou mais claro após a implementação?

---

## 5. Vídeo de Demonstração

**Link do vídeo:** [Insira aqui o link para YouTube, Google Drive, etc.]

### Conteúdo do vídeo:

Confirme que o vídeo contém:

- [ ] Demonstração da compilação do projeto
- [ ] Execução do simulador com algoritmo FIFO
- [ ] Execução do simulador com algoritmo Clock
- [ ] Explicação da saída produzida
- [ ] Comparação dos resultados FIFO vs Clock
- [ ] Breve explicação de uma decisão de design importante

---

## Checklist de Entrega

Antes de submeter, verifique:

- [ ] Código compila sem erros conforme instruções da seção 1.1
- [ ] Simulador funciona corretamente com FIFO
- [ ] Simulador funciona corretamente com Clock
- [ ] Formato de saída segue EXATAMENTE a especificação do ENUNCIADO.md
- [ ] Testamos com os casos fornecidos em tests/
- [ ] Todas as seções deste relatório foram preenchidas
- [ ] Análise comparativa foi realizada com dados reais
- [ ] Vídeo de demonstração foi gravado e link está funcionando
- [ ] Todos os integrantes participaram e concordam com a submissão

---
## Referências
Liste aqui quaisquer referências que utilizaram para auxiliar na implementação (livros, artigos, sites, **links para conversas com IAs.**)


---

## Comentários Finais

Use este espaço para quaisquer observações adicionais que julguem relevantes (opcional).

---
