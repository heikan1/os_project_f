/*
GRUP NO: 37
Metin Hakan Yılmaz / B221210075
Elşat Mirdesoğlu / B221210579
Osman Yıldız / B221210021
Abdulsamed Kurubal / B221210009
Ferdi Kaynar / G201210311 
*/

/* program.h */
#ifndef MAIN_H
#define MAIN_H
#include <stdbool.h> 

#define TRUE 1
#define FALSE 0
//
void handle_redirection(char **args);
void sig_child(int signo);
int execute_in_background(char **args);
char** space_parse(char* commands);
char** semicolon_parse(char* line, int* cmd_count);
void shell_loop();
//
void showPrompt();
char **parse_command(char *line);
int execute_command(char **args, bool isBackground);

int cmd_cd(char **args);
int cmd_help(char **args);
int cmd_quit(char **args);

#endif