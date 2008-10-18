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
	//
	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/stat.h>
	#include <fcntl.h>
}

@members
{
	//! Defines fuer die Parameterliste
	#define MAX_ARGS 	100

	//! Parameterarray fuer execvp
	char *argv[MAX_ARGS];

	//! Anzahl der tatsaechlich verwendeten Parameter
	//! wird verwendet um im exec Array an die richtige stelle zu schreiben
	int nArgsUsed = 0;
	
}
/*------------------------------------------------------------------
 * PARSER RULES
 *------------------------------------------------------------------*/
cmd_line       
        :	process (BLANK? pipeto process)* BLANK?
        |	EXIT;


process     :   BLANK? exe redir*
		{
			//printf("starte prozess '\%s'\n",$exe.text->chars);
			int i;
			
			if((i = fork()) == 0) //child
			{
				//printf("Prozess wird gestartet\n");
				// Here, the process is changed
				i = execvp(argv[0], argv);
				printf("i nach execvp:\%d",i);
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
        		argv[0] = (char *) malloc(strlen($binary.text->chars));
        		strcpy(argv[0], $binary.text->chars);	
        		
        		//printf("BINARY '\%s' gefunden ... wird an die Stelle 0 geschrieben\n",$binary.text->chars);
		};
param	:	STRING
		{
        		nArgsUsed ++;
        		//printf("PARAMETER '\%s' gefunden ... wird an die Stelle \%d geschrieben\n",$param.text->chars, nArgsUsed);

                   	argv[nArgsUsed] = (char *) malloc(strlen($param.text->chars));
        		strcpy(argv[nArgsUsed], $param.text->chars);	
		};
file	:	STRING;

redir	:	(BLANK? inredir)
	|	(BLANK? outredir);
	
exe 	:	binary (BLANK param)*;

inredir
        :       INPUT_REDIR BLANK? file 
        	{
			printf("leite Eingabestrom in '\%s' um\n",$file.text->chars);
        	};

outredir
        :       OUTPUT_REDIR BLANK? file 
        	{
			printf("leite Ausgabestrom in '\%s' um\n",$file.text->chars);
        	};


/*------------------------------------------------------------------
 * LEXER RULES
 *------------------------------------------------------------------*/
fragment CHAR  	: ('0'..'9'|'A'..'Z'|'a'..'z'|'/'|'-'|'*'|'.');
BLANK 		: ( '\t' | ' ')+;
STRING  	: (CHAR)+;
