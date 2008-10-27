/*
 * gsh.c
 *
 *  Created on: 06.10.2008
 *      Author: jars
 */

#include "wswd_main.h"
#include "wswdLexer.h"
#include "wswdParser.h"

//-----------------------------------------------------------------------------
/*
 * *********************** Funktionsimplementierungen*************************
 */
//-----------------------------------------------------------------------------
int main(int argc, char * argv[])
{
	char user_input[1024];

	print_welcomeBanner();
	print_prompt();

	while(fgets(user_input, sizeof(user_input), stdin))
	{
		what_shell_we_do(user_input);
		print_prompt();
	}
	return 0;
}
//-----------------------------------------------------------------------------
/**
 * Fuehrt ein Kommando ai=us
 * @param thats_to_do	eigentliches Kommando
 */
void what_shell_we_do(char* thats_to_do)
{
	pANTLR3_INPUT_STREAM		input;
	pwswdLexer					lex;
	pANTLR3_COMMON_TOKEN_STREAM tokens;
	pwswdParser					parser;

	LOG("Anweisung : %s \n", thats_to_do);

	// wir lesen vom uebergebenen String
	input  = antlr3NewAsciiStringInPlaceStream((pANTLR3_UINT8)thats_to_do,strlen(thats_to_do),NULL);
	if(input == NULL)
	{
		printf("### Fehler beim Erzeugen des Inputstreams");
	}

	lex    = wswdLexerNew                		(input);
	tokens = antlr3CommonTokenStreamSourceNew  	(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
	parser = wswdParserNew               		(tokens);

	processes_used = 0;
	parser -> cmd_line(parser); // Befuellen der internen strukturen

	print_processes();

	// Must manually clean up
	parser ->free(parser);
	tokens ->free(tokens);
	lex    ->free(lex);
	input  ->close(input);


	process_struct();
	cleanup_struct();

}
//-----------------------------------------------------------------------------
/*
 * Gibt am Schirm eine Wilkommensmessage aus
 */
void print_welcomeBanner(void)
{
	printf("\n");
	printf("+----------------------------------------------------------------+\n");
	printf("| Willkommen zu 'what-shell-we-do', der ultimativen Unix Shell   |\n");
	printf("| Developers : Hutter G. und Juvan J. (SWD05)                    |\n");
	printf("| Compiletime : " __DATE__ " " __TIME__ "                             |\n");
	printf("+----------------------------------------------------------------+\n\n");
}
//-----------------------------------------------------------------------------
/**
 * Fragt den Benuzer nach einer Eingabe
 */
void print_prompt(void)
{
	char szHostName[254];
	char szWorkingDir[254];

	gethostname(szHostName, sizeof(szHostName));
	getcwd(szWorkingDir, sizeof(szWorkingDir));

	printf("\nwhat-shell-we-do@%s [%s]: ", szHostName, szWorkingDir);
}
//-----------------------------------------------------------------------------
/**
 * Funktion zum bereinigen des Speichers
 */
void cleanup_struct(void)
{
	int i;

    while(processes_used > 0)
	{
    	wswd_proz *ps = processes[processes_used-1];
    	// Speicher freigeben, falls angebraucht
		if(ps->szInRedir != NULL)
		{
			free(ps->szInRedir);
		}
		ps->szInRedir = NULL; // Als ungebraucht markieren

		// Speicher freigeben, falls angebrauc
		if(ps->szOutRedir != NULL)
		{
			free(ps->szOutRedir);
		}
		ps->szOutRedir = NULL;// Als ungebraucht markieren

		// Wir muessen den speicher wieder freigeben
		while(ps->nArgsUsed > 0)
		{
			free(ps->argv[ps->nArgsUsed-1]);
			ps->argv[ps->nArgsUsed-1] = NULL;
			ps->nArgsUsed --;
		}
		ps->nArgsUsed = 0;

		// Wir loeschen das pipe flag
		ps->nUsePipe = 0;

		processes[processes_used-1] = NULL;
		processes_used --;
	}
}
//-----------------------------------------------------------------------------
/**
 * Arbeitet das Strukturarray ab, startet die Prozesse, legt redirections an und
 * erzeugt pipes (sofern notwendig)
 */
void process_struct(void)
{
	int pipe_connection[2];
	int pipes_initialized;

	pipes_initialized  = 0;
	// juvis work goes in here

	/* Empfehlung der Vorgehensweile lt. Kvas :
	Erzeuge einen oder mehrere Kindprozess(e)
	Richte für diese(n) Kindprozess(e) das Environment ein

	    * Redirection einrichten
	    * Pipes einrichten
	    * Background

	Starte das Kommando
	Warten im Parent auf das beenden das Prozesses
	*/

	// Die Kommandozeile ist bereits in einen AST zerlegt und
	// der kann nun abgearbeitet werden.

	int pidx;
	wswd_proz* w;

	// Fuer jeden Prozess
	for (pidx = 0; pidx < processes_used; pidx++)
	{
		// Prozessstrukturpointer umhaengen
		w= processes[pidx];

		// Ungueltig -> Fehlermeldung
		if (w == NULL)
		{
			LOG("Ups, Der Prozess %d NULL der das besser nicht waere !\n", pidx);
			break;
		}

		// Offensischtlich haben wir einen gueltigen Prozess
		// den sehen wir uns naeher an
		if (w->nUsePipe &&
						!pipes_initialized)
		{
				LOG("pipe(pipe_connection);");
				pipe(pipe_connection);
				pipes_initialized = 1;
		}


		int childPid;
		if ((childPid = fork()) == 0)
		{
			int fd_out, fd_in; //!< Filedeskriptoren fuer in und outredir

			// Redirections einrichten
			// Eingaberedirection
			if(w->szInRedir != NULL)
			{
  					LOG("INPUT Redirection wird durchgefuehrt");
  					fd_in = open(w->szInRedir, O_RDONLY);
  					dup2(fd_in, fileno(stdin)); // redirect to stdout
  					close(fd_in); // Close unused file descriptors
			}
			// Ausgaberedirection
			if(w->szOutRedir != NULL)
			{
				LOG("OUTPUT Redirection wird durchgefuehrt");
				fd_out = open(w->szOutRedir, O_RDWR|O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				dup2(fd_out, fileno(stdout)); // redirect to stdout
				close(fd_out); // Close unused file descriptors
			}

			// Pipes einrichten

			//Prozess starten
			int exec_retval;
			exec_retval = execvp(w->argv[0], w->argv);
			// Alles gutgegangen ?
			if(exec_retval != 0)
			{
				// Nein -> Fehler
				LOG("Command %s returned with Errorcode %d\n", w->argv[0], exec_retval);
			}

			// Wir beenden den Kindprozess wieder, damit es weitergehen kann.
			exit(0);
		}

		// Wir warten auf den Kindprozess
		waitpid(childPid,0,0);
	}
}

//-----------------------------------------------------------------------------
void init_test_struct(void)
{

        processes[0] = malloc(sizeof(wswd_proz));
        processes[1] = malloc(sizeof(wswd_proz));

        wswd_proz* w1 = processes[0];


        char argv0_0[] = "ls";
        char argv0_1[] = "-l";

        w1->argv[0] = malloc(strlen(argv0_0));
        strcpy(w1->argv[0], argv0_0);
        w1->nArgsUsed = 1;

        w1->argv[1] = malloc(strlen(argv0_1));
        strcpy(w1->argv[1], argv0_1);
        w1->nArgsUsed = 2;

        w1->nUsePipe = 1;

        char argv1_0[] = "grep";
        char argv1_1[] = "2008";

        wswd_proz* w2 = processes[1];

        w2->argv[0] = malloc(strlen(argv1_0));
        strcpy(w2->argv[0], argv1_0);
        w2->nArgsUsed = 1;

        w2->argv[1] = malloc(strlen(argv1_1));
        strcpy(w2->argv[1], argv1_1);
        w2->nArgsUsed = 2;
        w2->nUsePipe = 0;
        processes_used = 2;
/*
        processes[2] = malloc(sizeof(wswd_proz));
        w2->nUsePipe = 1;
        char argv2_0[] = "sort";
        char argv2_1[] = "-r";

        wswd_proz* w3 = processes[2];

        w3->argv[0] = malloc(strlen(argv2_0));
        strcpy(w3->argv[0], argv2_0);
        w3->nArgsUsed = 1;

        w3->argv[1] = malloc(strlen(argv2_1));
        strcpy(w3->argv[1], argv2_1);
        w3->nArgsUsed = 2;
        w3->nUsePipe = 0;
        processes_used = 3;
*/
}
//-----------------------------------------------------------------------------
/**
 * Leert die uebergebene Struktur
 */
void init_struct(wswd_proz* p)
{
	p->nArgsUsed = 0;
	p->nUsePipe = 0;
	p->szInRedir = NULL;
	p->szOutRedir = NULL;

	int i = 0;
	for(i = 0; i < MAX_ARGS; i++)
	{
		p->argv[i] = NULL;
	}
}
//-----------------------------------------------------------------------------
/**
 * Gibt den Inhalt der uebergebenen Struktur aus
 */
void print_struct(wswd_proz* p)
{
	printf("\n---INHALT von wswd_proz---\n");

	printf(" p->nUsePipe = %d\n",p->nUsePipe);

	if(p->szInRedir == NULL)
	{
		printf(" p->szInRedir = NULL\n");
	}
	else
	{
		printf(" p->szInRedir = %s\n",p->szInRedir);
	}

	if(p->szOutRedir == NULL)
	{
		printf(" p->szOutRedir = NULL\n");
	}
	else
	{
		printf(" p->szOutRedir = %s\n",p->szOutRedir);
	}

	printf(" p->nArgsUsed = %d\n",p->nArgsUsed);
	if(p->nArgsUsed > 0)
	{
		int i = 0;
		for(i = 0; i < p->nArgsUsed; i++)
		{
			printf(" p->argv[%d] = %s\n",i,p->argv[i]);
		}
	}
	printf("--------------------\n");

}
//-----------------------------------------------------------------------------
/**
 * Gibt alle Prozesse aus
 */
void print_processes(void)
{
	int i=0;
	printf("\n###Anzahl Prozesse : %d###\n",processes_used);
	for (i = 0; i < processes_used; i++)
	{
		printf("processes[%d]\n",i);
		if (processes[i] != NULL)
			{
				print_struct(processes[i]);
			}
	}

}
//-----------------------------------------------------------------------------
