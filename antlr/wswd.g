grammar wswd;


options
{
    language=C;
    //buildAST=true;
    //output=AST;
}

tokens
{
	NL      	= '\n';
	OUTPUT_REDIR    = '>';
	INPUT_REDIR     = '<';
	EXIT    	= 'exit';
	BACKGROUND	= '&'; 
}

@parser::includes
{
	// Include our noddy C++ example class
	#include "wswd_main.h"
}

@members
{
	//---------------------------------------------------------
	// Variablen und defines fuer Prozesse starten
	//---------------------------------------------------------
	
	//! Defines fuer die Parameterliste
	#define MAX_ARGS 	100

	//! Parameterarray fuer execvp
	char *argv[MAX_ARGS];

	//! Anzahl der tatsaechlich verwendeten Parameter
	//! wird verwendet um im exec Array an die richtige stelle zu schreiben
	int nArgsUsed = 0;


	//---------------------------------------------------------
	// Variablen und defines fuer Redirections
	//---------------------------------------------------------
	
}
/*------------------------------------------------------------------
 * PARSER RULES
 *------------------------------------------------------------------*/
cmd_line       
        :	process (BLANK? pipeto process)* BLANK?
        |	NL 
        	{
        		LOG("No command given!\n");
        	}
        |	EXIT 
        	{
        		// Wir verabschieden uns
        		LOG("Thanks for using the shell, bye!\n");
        		exit(0);
        	};


process     :   BLANK? exe redir*
		{
			//printf("starte prozess '\%s'\n",$exe.text->chars);
			int i;
			
			if((i = fork()) == 0) //child
			{
				//printf("Prozess wird gestartet\n");
				// Here, the process is changed
				i = execvp(argv[0], argv);
				LOG("Creation of process [\%s] returned : \%d",(char*)$exe.text->chars,i);
				exit(0);
			}
	
			waitpid(i, 0 , 0);			
	
			// Wir muessen den speicher wieder freigeben
			while(nArgsUsed >= 0)
			{
				free(argv[nArgsUsed]);
				nArgsUsed --;
			}
			nArgsUsed = 0;

		};

pipeto    :	'|'
		{
        		printf("da erzeugen wir mal schnell eine PIPE");	
        	};
binary 	:	STRING
		{
        		argv[0] = (char *) malloc(strlen((char*)$binary.text->chars));
        		strcpy(argv[0], (char*) $binary.text->chars);	
        		LOG("BINARY '\%s' gefunden ... wird an die Stelle 0 geschrieben\n",(char*)$binary.text->chars);
		};
param	:	STRING
		{
        		nArgsUsed ++;
        		LOG("PARAMETER '\%s' gefunden ... wird an die Stelle \%d geschrieben\n",(char*)$param.text->chars, nArgsUsed);

                   	argv[nArgsUsed] = (char *) malloc(strlen((char*)$param.text->chars));
        		strcpy(argv[nArgsUsed], (char*) $param.text->chars);	
		};
file	:	STRING;

redir	:	(BLANK? inredir)
	|	(BLANK? outredir);
	
exe 	:	binary (BLANK param)*;

inredir
        :       INPUT_REDIR BLANK? file 
        	{
			LOG("leite Eingabestrom in '\%s' um\n",(char*)$file.text->chars);
        	};

outredir
        :       OUTPUT_REDIR BLANK? file 
        	{
			LOG("leite Ausgabestrom in '\%s' um\n",(char*)$file.text->chars);
        	};


/*------------------------------------------------------------------
 * LEXER RULES
 *------------------------------------------------------------------*/
fragment CHAR  	: ('0'..'9'|'A'..'Z'|'a'..'z'|'/'|'-'|'*'|'.');
BLANK 		: ( '\t' | ' ')+;
STRING  	: (CHAR)+;
