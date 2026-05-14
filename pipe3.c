#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define buffer_size 13
#define read_end 0
#define write_end 1

int main()
{
    pid_t pid_a;
    pid_t pid_b;
    pid_t pid_c;

    int pipa1[2];
    int pipa2[2];

    pipe(pipa1);
    pipe(pipa2);

    pid_a = fork();

    if (pid_a < 0)
    {
        perror("fork failed");
        return 1;
    }
    else if (pid_a == 0)
    {
        sleep(2);
        printf("\n\n");
        printf("Proceso hijo A \n");
        printf("PID A: %d\n", getpid());
        printf("PID padre P: %d\n\n", getppid());

        dup2(pipa1[1], STDOUT_FILENO);

        close(pipa1[0]);
        close(pipa1[1]);
        close(pipa2[0]);
        close(pipa2[1]);

        char *cadena[] = {"ls", "-lh", NULL};

        execvp("ls", cadena);

        perror("execvp fallo"); // si falla exec, se imprime y corre todo lo demas, por eso se hace en el hijo solamente
        exit(1);
    }
    else
    {

        wait(NULL);

        pid_b = fork();

        if (pid_b < 0)
        {
            perror("fork failed");
            return 1;
        }
        else if (pid_b == 0)
        {
            sleep(2);
            printf("\n\n");
            printf("Proceso hijo B \n");
            printf("PID B: %d\n", getpid());
            printf("PID padre P: %d\n\n", getppid());

            dup2(pipa1[0], STDIN_FILENO);
            dup2(pipa2[1], STDOUT_FILENO);

            close(pipa1[0]);
            close(pipa1[1]);
            close(pipa2[0]);
            close(pipa2[1]);

            char *cadena[] = {"head", "-1", NULL};

            execvp("head", cadena);

            perror("execvp fallo"); // si falla exec, se imprime y corre todo lo demas, por eso se hace en el hijo solamente
            exit(1);
        }
        else
        {

            wait(NULL);

            pid_c = fork();

            if (pid_c < 0)
            {
                perror("fork failed");
                return 1;
            }
            else if (pid_c == 0)
            {
                sleep(2);
                printf("\n\n");
                printf("Proceso hijo C \n");
                printf("PID C: %d\n", getpid());
                printf("PID padre P: %d\n\n", getppid());

                dup2(pipa2[0], STDIN_FILENO);

                close(pipa1[0]);
                close(pipa1[1]);
                close(pipa2[0]);
                close(pipa2[1]);

                char *cadena[] = {"wc", "-c", NULL};

                execvp("wc", cadena);

                perror("execvp fallo"); // si falla exec, se imprime y corre todo lo demas, por eso se hace en el hijo solamente
                exit(1);
            }
            else
            {
                close(pipa1[0]);
                close(pipa1[1]);
                close(pipa2[0]);
                close(pipa2[1]);

                wait(NULL);

                printf("\n\n");
                printf("Proceso P\n");
                printf("PID P: %d\n", getpid());
                printf("PID padre de P: %d\n", getppid());
                printf("PID hijo A: %d\n\n", pid_a);

            }
        }
    }

    return 0;
}