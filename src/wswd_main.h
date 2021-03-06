#ifndef WSWD_MAIN_H
#define WSWD_MAIN_H

#include<sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
/**
  * Zentrale Logfunktion fuer interne Loggings.
  * Diese funktion kann in der Releaseversion unterdrueckt werden.
  */
#define LOG(...)  fprintf(stderr, "%d ",getpid()); fprintf(stderr, " >> LOG : " __VA_ARGS__ ); fflush(stderr);


//! Defines fuer die Parameterliste
#define MAX_ARGS 		100
#define MAX_PROCESSES 	100

typedef struct _wswd_proz
{
	//! Parameterarray fuer execvp
	char *argv[MAX_ARGS];

	//! Anzahl der tatsaechlich verwendeten Parameter
	//! wird verwendet um im exec Array an die richtige stelle zu schreiben
	int nArgsUsed;

	//! Inputredirection
	//! In der Regel inredir wird dieser Pointer mit dem Filenamen
	//! der Einzulesenden Datei befuellt
	char *szInRedir;

	//! Outputredirection
	//! In der Regel outredir wird dieser Pointer mit dem Filenamen
	//! der Einzulesenden Datei befuellt
	char *szOutRedir;

	//! Pipes verwendet
	int nUsePipe;

	//! Die Pipeverbindung zum naechsten Prozess
	//! pipe_verbindung[0] zum Lesen und pipe_verbindung[1] zum Schreiben
	int pipe_verbindung[2];

	//! Die ChildPid gibt an welche PID dieser Kindprozess hat
	int nPID;

}wswd_proz;

wswd_proz* processes[MAX_PROCESSES];
int processes_used;

// Defines fuer PIPE_Richtungen
#define PREAD 0
#define PWRITE 1
//-----------------------------------------------------------------------------
/*
 * *********************** Prototypendeklarationen ***************************
 * normalerweise gehoert das in ein H File, aber nachdem das Projekt so klein
 * ist und es in C nicht verboten ist deklarieren wir unsere Funktionen hier
 */
//-----------------------------------------------------------------------------


void what_shell_we_do(char* thats_to_do);
void print_welcomeBanner(void);
void print_prompt(void);
void cleanup_struct(void);
void process_struct(void);
void print_struct(wswd_proz* p);
void init_struct(wswd_proz* p);
void print_processes(void);
void close_all_pipes(void);
void redirect(wswd_proz* w);
void do_exec(wswd_proz* w);

#endif //WSWD_MAIN_H
