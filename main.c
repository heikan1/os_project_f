/*
GRUP NO: 37
Metin Hakan Yılmaz / B221210075
Elşat Mirdesoğlu / B221210579
Osman Yıldız / B221210021
Abdulsamed Kurubal / B221210009
Ferdi Kaynar / G201210311 
*/

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "main.h"
#define TOK_BUFSIZE 64
#define DELIMITERS " \n\t\r\a"

// Arkaplan süreç listesi
pid_t background_processes[256];
int bg_count = 0;

//Ekrana prompt yazdırır
void showPrompt() {
    printf("> ");
    fflush(stdout);
}

// Built-in komutlar
char *builtin_commands[] = {"cd", "help", "quit"};
int (*builtin_funcs[])(char **) = {&cmd_cd, &cmd_help, &cmd_quit};

// Built-in komutların sayısını döner
int num_builtins() {
    return sizeof(builtin_commands) / sizeof(char *);
}
// 'cd' komutunu işler, dosya dizinini değiştirir
int cmd_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "myshell: 'cd' komutu bir dizin belirtmeli\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("myshell");
        }
    }
    return 1;
}
// 'help' komutunu işler, desteklenen komutları listeler
int cmd_help(char **args) {
    printf("myshell: Desteklenen komutlar:\n");
    for (int i = 0; i < num_builtins(); i++) {
        printf("  %s\n", builtin_commands[i]);
    }
    printf("Ayrıca, sistem komutları ve I/O yönlendirme desteklenir.\n");
    return 1;
}
// 'quit' komutunu işler, kabuktan çıkar, çıkmadan önce arkaplan proseslerinin bitmesini bekler
int cmd_quit(char **args) {
    for (int i = 0; i < bg_count; i++) {
        int status;
        waitpid(background_processes[i], &status, 0);
        printf("[%d] retval: %d\n", background_processes[i], WEXITSTATUS(status));
    }
    while (wait(NULL) > 0); // Wait for all remaining child processes
    printf("Kabuktan çıkış yapılıyor...\n");
    exit(0);
}
// Çocuk süreç sinyalini işler
void sig_child(int signo) {
    int status;
    pid_t pid;
    //eğer ki proses bittiyse dokümanda istenilen çıktıyı ver
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            int child_val = WEXITSTATUS(status);
            printf("[%d] retval: %d\n>", pid, child_val);
        }
    }
}
// Komutu arkaplanda çalıştırır
int execute_in_background(char **args) {
    pid_t pid;
    //arkaplanda çalışacak prosesi takip edebilmek için bir sinyal handler tanımla
    struct sigaction sa;
    sa.sa_handler = sig_child;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDSTOP | SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        fprintf(stderr, "sigaction failed\n");
        return 1;
    }
    //prosesi fokla ve çalıştır ama 
    //önplanda çalışan prosesler aksine ebeveyn proseste bitmesini bekleme
    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            printf("Komut bulunamadi");
            kill(getpid(), SIGTERM);
        }
    } else if (pid < 0) {
        perror("fork failed");
        return 1;
    } else {
        // Parent process
        printf("Process running in background with PID: %d\n", pid);
    }

    return 0;
}
// Shell döngüsünü başlatır
void shell_loop() {
    char *line = NULL;
    size_t bufsize = 0;
    char **commands;

    int status;
    //status 1 olduğu sürece shell çalışmaya devam eder
    do {
        int command_count = 0;
        showPrompt();
        fflush(stdout);
        getline(&line, &bufsize, stdin);
        commands = semicolon_parse(line,&command_count);
        
        // her bir komutu gez ve sırası ile argümanlarına ayırarak çalıştır
        for(int i = 0; i<command_count; i++){
            bool isBackground = false;
            char** a = space_parse(commands[i]);
            for(int j = 0; a[j]; j++){
                if(strcmp(a[j],"&") == 0){
                    isBackground=true;
                    a[j] = NULL;
                    break;
                }
            }
            status = execute_command(a,isBackground);
            //commandler arasında 2 saniye bekleme yap
            if(command_count > 1 && i <command_count-1){
                sleep(2);
            }
        }
    } while (status);

    free(commands);
    free(line);
}
// Programın giriş noktası
int main(int argc, char **argv) {
    //kabuk döngüsünü başlat
    shell_loop();
    return 0;
}
