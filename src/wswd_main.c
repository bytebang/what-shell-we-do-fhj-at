/*
 * wswd_main.c
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

	// Immer weiter fragen
	while(fgets(user_input, sizeof(user_input), stdin))
	{
		// Abarbeiten
		what_shell_we_do(user_input);

		// Prompt neu ausgeben
		print_prompt();
	}
	return 0;
}
//-----------------------------------------------------------------------------
/**
 * Fuehrt ein Kommando aus
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

	// Jetzt gehts los !
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
	fflush(stdout);
	printf("\nwhat-shell-we-do@%s [%s]: ", szHostName, szWorkingDir);
}
//-----------------------------------------------------------------------------
/**
 * Funktion zum bereinigen des Speichers
 */
void cleanup_struct(void)
{
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

		// Pipewerte zuruecksetzen
		ps->pipe_verbindung[0] = 0;
		ps->pipe_verbindung[1] = 0;

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
	// Aktueller und Vorgaengerprozess
	wswd_proz *w, *w_prev;
	int i;
	// Alle Pipe werden "geoeffnet"
	for (i = 0; i < processes_used-1; i++)
	{
		w = processes[i];
		LOG("Create pipe for %d\n", i);
		pipe(w->pipe_verbindung);
	}

	// Jetzt werden die Prozesse abgearbeitet
	for (i = 0; i < processes_used; i++)
	{
		LOG("Process %d\n", i);

		// Aktueller Prozess
		w = processes[i];
		LOG("Befehl: %s\n", w->argv[0]);
		if (i > 0)
		{
			// Es gibt einen Vorgaenger
			w_prev = processes[i-1];
		}
		else
		{
			// Kein Vorgaenger
			w_prev = NULL;
		}
		if (w->nUsePipe && w_prev == NULL)
		{
			// Wenn eine Pipe vorhanden ist und dies der erste Prozess ist
			LOG("Outgoing Pipe: Fork Process %d\n", i);
			if ((w->nPID = fork())==0)
			{
				// Moegliche Redirections
				redirect(w);
				// Pipe fuer Ausgabestrom
				dup2(w->pipe_verbindung[PWRITE],STDOUT_FILENO);
				// Alle Pipes werden geschlossen
				close_all_pipes();
				// Prozess 1 wird gestartet
				do_exec(w);
			}
		}
		else if (!w->nUsePipe && w_prev != NULL)
		{
			// Letzter Prozess
			LOG("Incoming Pipe: Fork Process %d\n", i);
			if ((w->nPID = fork())==0)
			{
				// Moegliche Redirections
				redirect(w);
				// Pipe auf Eingabestrom des Prozesses
				dup2(w_prev->pipe_verbindung[PREAD],STDIN_FILENO);
				// Schliessen aller anderen Pipes
				close_all_pipes();
				// Prozess wird angestartet
				do_exec(w);
			}
		}
		else if (w->nUsePipe && w_prev != NULL)
		{
			// Ein Prozess in der Mitte sowohl ein- als auch ausgehende Pipes
			LOG("Bidirectional Pipe: Fork Process %d\n", i);
			if ((w->nPID = fork())==0)
			{
				// Moegliche Redirections
				redirect(w);
				// Eingagestrom
				dup2(w_prev->pipe_verbindung[PREAD],STDIN_FILENO);
				// Ausgabestrom
				dup2(w->pipe_verbindung[PWRITE], STDOUT_FILENO);
				// Schliessen aller Pipes
				close_all_pipes();
				// Prozess wird angestartet
				do_exec(w);
			}
		}
		else
		{
			// Nur ein Prozess keine Pipes
			if ((w->nPID = fork())==0)
			{
				// Moegliche Redirections
				redirect(w);
				// Prozess wird gestartet
				do_exec(w);
			}
		}

	}

	// Alle Pipes im Vaterprozess werden geschlossen
	close_all_pipes();

	// Warten auf die ausstaendigen Kindprozesse
	for (i = 0; i < processes_used; i++)
	{
		LOG("waitpid(%d, 0, 0);\n", processes[i]->nPID);
		waitpid(processes[i]->nPID,NULL,0);
	}
}
//-----------------------------------------------------------------------------
/**
 * Kuemmert sich um die redirections, sofern notwendig
 * @w	fuer welche prozessstruktur sollte die redirection aufgesetzt werden
 */
void redirect(wswd_proz* w)
{
	int fd_out, fd_in; //!< Filedeskriptoren fuer in und outredir

	// Redirections einrichten
	// Eingaberedirection
	if(w->szInRedir != NULL)
	{
			LOG("INPUT Redirection wird durchgefuehrt\n");
			fd_in = open(w->szInRedir, O_RDONLY);
			dup2(fd_in, fileno(stdin)); // redirect to stdout
			close(fd_in); // Close unused file descriptors
	}
	// Ausgaberedirection
	if(w->szOutRedir != NULL)
	{
		LOG("OUTPUT Redirection wird durchgefuehrt\n");
		fd_out = open(w->szOutRedir, O_RDWR|O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		dup2(fd_out, fileno(stdout)); // redirect to stdout
		close(fd_out); // Close unused file descriptors
	}
}
//-----------------------------------------------------------------------------
/**
 * Schliesst alle Pipes
 */
void close_all_pipes(void)
{
	int i;
	for (i = 0; i < processes_used-1; i++)
	{
		close(processes[i]->pipe_verbindung[PREAD]);
		close(processes[i]->pipe_verbindung[PWRITE]);
	}
}
//-----------------------------------------------------------------------------
/**
 * Leert die uebergebene Struktur
 * @p fuer pointer auf die zu initialisierende struktur
 */
void init_struct(wswd_proz* p)
{
	p->nArgsUsed = 0;
	p->nUsePipe = 0;
	p->szInRedir = NULL;
	p->szOutRedir = NULL;
	p->nPID = -1;
	p->pipe_verbindung[0] = 0;
	p->pipe_verbindung[1] = 0;

	int i = 0;
	for(i = 0; i < MAX_ARGS; i++)
	{
		p->argv[i] = NULL;
	}
}
//-----------------------------------------------------------------------------
/**
 * Gibt den Inhalt der uebergebenen Struktur aus
 * @p pointer auf auszugebende Struktur
 */
void print_struct(wswd_proz* p)
{

	LOG(" ---INHALT von wswd_proz: [%p] aus der Sicht von Prozess %d ---\n", &p, getpid());
	LOG("|   p->nUsePipe = %d\n",p->nUsePipe);

	LOG("|   p->pipe_verbindung[0] = %d\n",p->pipe_verbindung[0]);
	LOG("|   p->pipe_verbindung[1] = %d\n",p->pipe_verbindung[1]);

	if(p->szInRedir == NULL)
	{
		LOG("|   p->szInRedir = NULL\n");
	}
	else
	{
		LOG("|   p->szInRedir = %s\n",p->szInRedir);
	}

	if(p->szOutRedir == NULL)
	{
		LOG("|   p->szOutRedir = NULL\n");
	}
	else
	{
		LOG("|   p->szOutRedir = %s\n",p->szOutRedir);
	}

	LOG("|   p->nArgsUsed = %d\n",p->nArgsUsed);
	if(p->nArgsUsed > 0)
	{
		int i = 0;
		for(i = 0; i < p->nArgsUsed; i++)
		{
			LOG("|   p->argv[%d] = %s\n",i,p->argv[i]);
		}
	}
	LOG("---------------------------------------\n");

}
//-----------------------------------------------------------------------------
/**
 * Gibt alle Prozesse aus
 */
void print_processes(void)
{
	int i=0;
	LOG("###Anzahl Prozesse : %d###\n",processes_used);
	for (i = 0; i < processes_used; i++)
	{
		LOG(" processes[%d]\n",i);
		if (processes[i] != NULL)
		{
			print_struct(processes[i]);
		}
	}
}
//-----------------------------------------------------------------------------
/**
 * Fuehrt den uebergebenen Prozess aus
 * @ pointer auf auszufuehrende struktur
 */
void do_exec(wswd_proz* w)
{
	int nRetVal;

	nRetVal = execvp(w->argv[0], w->argv);

	if(nRetVal != 0)
	{
		fprintf(stderr,"Fehler beim ausfuehren von [%s], Errorcode=%d",w->argv[0], nRetVal);
	}
}
//-----------------------------------------------------------------------------
