#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

using namespace std;

int main() {
    // implementação de comunicação via pipes para controlar ordem de execução dos processos
    int pipefd[2];
    pipe(pipefd);  
    // pipefd[0] = leitura
    // pipefd[1] = escrita

    // ------------------------------  
    // Cria F1
    // ------------------------------  
    pid_t f1 = fork();

    if (f1 == 0) {
        // -------- FILHO F1 --------

        pid_t n1 = fork();
        if (n1 == 0) {
            //não realiza operação de leitura e escrita (vale para os demais N)
            close(pipefd[0]);
            close(pipefd[1]);
            execl("/bin/pwd", "pwd", (char*)NULL); // caminho do diretório atual
            exit(1);
        }

        pid_t n2 = fork();
        if (n2 == 0) {
            close(pipefd[0]);
            close(pipefd[1]);
            execl("/bin/date", "date", (char*)NULL); //data
            exit(1);
        }

        // Espera N1 e N2
        waitpid(n1, NULL, 0);
        waitpid(n2, NULL, 0);

        // Sinaliza 2 execuções concluídas
        write(pipefd[1], "x", 1);
        write(pipefd[1], "x", 1);

        // Esperar os sinais de N3 e N4 
        char buf;
        read(pipefd[0], &buf, 1);  // sinal de N3
        read(pipefd[0], &buf, 1);  // sinal de N4

        printf("Sou F1 executado após N1, N2, N3 e N4\n");
        printf("Meu PID é %d e o do meu pai é %d\n---------------------\n", getpid(), getppid());

        // sinaliza para F2
        write(pipefd[1], "x", 1);

        // fecha leitura e escrita
        close(pipefd[0]);
        close(pipefd[1]);

        exit(0);
    }

    // ------------------------------  
    // Cria F2
    // ------------------------------  
    pid_t f2 = fork();

    if (f2 == 0) {
        // -------- FILHO F2 --------

        pid_t n3 = fork();
        if (n3 == 0) {
            close(pipefd[0]);
            close(pipefd[1]);
            execl("/bin/echo", "echo", "Trabalho de Gabriel e Elo", (char*)NULL); //texto no terminal
            exit(1);
        }

        pid_t n4 = fork();
        if (n4 == 0) {
            close(pipefd[0]);
            close(pipefd[1]);
            execl("/usr/bin/whoami", "whoami", (char*)NULL); //quem é o usuário
            exit(1);
        }

        // Espera N3 e N4
        waitpid(n3, NULL, 0);
        waitpid(n4, NULL, 0);

        // Sinaliza conclusão de N3 e N4
        write(pipefd[1], "x", 1);
        write(pipefd[1], "x", 1);

        // Espera F1 finalizar
        char buf;
        read(pipefd[0], &buf, 1);

        printf("Sou F2, meu PID é %d e o do meu pai é %d\n---------------------\n", getpid(), getppid());

        // sinaliza para o pai
        write(pipefd[1], "x", 1);

        // fecha leitura e escrita
        close(pipefd[0]);
        close(pipefd[1]);

        exit(0);
    }

    // ------------------------------
    // Pai P1
    // ------------------------------
  
    // fecha escrita
    close(pipefd[1]);
    
    waitpid(f1, NULL, 0);
    waitpid(f2, NULL, 0);

    // espera sinal final de F2
    char buf;
    read(pipefd[0], &buf, 1);

    printf("Sou o pai %d e o programa acaba aqui\n", getpid());

    // fecha leitura
    close(pipefd[0]);
            
    return 0;
}
