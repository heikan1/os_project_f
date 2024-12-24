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

//


//

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
