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

	process_struct();
	cleanup_struct();
	/*
pANTLR3_COMMON_TOKEN	    t;
do
{
t = TOKENSOURCE(lex)->nextToken(TOKENSOURCE(lex));
pANTLR3_STRING s = t->getText(t);
if (t != NULL)
{
printf("[%s] ", s->chars);
fflush(stdout);
}
}
while (t == NULL || t->getType(t) != ANTLR3_TOKEN_EOF);
	 */
	// Must manually clean up
	parser ->free(parser);
	tokens ->free(tokens);
	lex    ->free(lex);
	input  ->close(input);

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

		processes_used --;
		ps = NULL;
	}
}
//-----------------------------------------------------------------------------
/**
 * Arbeitet das Strukturarray ab, startet die Prozesse, legt redirections an und
 * erzeugt pipes (sofern notwendig)
 */
void process_struct(void)
{
	return;
	int pipe_connection[2];
	int pipes_initialized;

	pipes_initialized  = 0;
        // juvis work goes in here
        int i, pid;
        for (i = 0; i < processes_used; i++)
        {
                if (processes[i] != NULL)
                {
                        wswd_proz* w;
                        w= processes[i];
                        if (w->nUsePipe &&
                                        !pipes_initialized)
                        {
                                LOG("pipe(pipe_connection);");
                                pipe(pipe_connection);
                                pipes_initialized = 1;
                        }
                        int j;
                        if ((j = fork()) == 0)
                        {
                                pid = j;
                                if (w->nUsePipe == 1)
                                {
                                        if (i == 0)
                                        {
                                                LOG("Pipe from");
                                                dup2(pipe_connection[1],1);
                                        close(pipe_connection[0]);
                                        }

                                }

                                if (processes[i-1] != NULL)
                                {
                                        if (processes[i-1]->nUsePipe)
                                        {
                                                if (!w->nUsePipe)
                                                {
                                                        LOG("Pipe to");
                                                        dup2(pipe_connection[0],0);
                                                        close(pipe_connection[1]);
                                                }
                                                else
                                                {
                                                        LOG("Double Pipe");
                                                        dup2(pipe_connection[0],0);
                                                        dup2(pipe_connection[1],1);
                                                }
                                        }
                                }

                                j = execvp(w->argv[0], w->argv);
                                exit(0);

                        }
                }
        }
        waitpid(pid,0,0);
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
