grammar wswd;

options
{
	language    	= C;
	//ASTLabelType	= pANTLR3_BASE_TREE;
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
	//Pointer auf die wswd_proz struktur
	wswd_proz* w_akt;
}


/*------------------------------------------------------------------
 * PARSER RULES
 *------------------------------------------------------------------*/
 
cmd_line       
        :	process (process)* BLANK?
        |	EXIT;

pipecreator 
	: (BLANK? pipeto)
	{
		LOG("Prozess verwendet eine Pipe\n");
		w_akt->nUsePipe = 1;
	};
		
process     :   BLANK? exe redir* pipecreator?
		{
			LOG("Ende des Prozesses\n");
			print_struct(w_akt);
			w_akt = NULL;
		};

pipeto    :	'|';
binary 	:	STRING
		{
	       		// Wir erzeugen eine neue wswd_proc struktur
			processes_used ++;
			processes[processes_used -1] = malloc(sizeof(wswd_proz));
			w_akt = processes[processes_used -1];
			init_struct(w_akt);
			
        		// Das binary kann bereits ausgefuellt werden
        		w_akt->argv[w_akt->nArgsUsed] = (char *) malloc(strlen((char*)$binary.text->chars));
        		strcpy(w_akt->argv[w_akt->nArgsUsed], (char*) $binary.text->chars);	
       		
        		LOG("BINARY '\%s' gefunden ... wird an die Stelle 0 geschrieben\n",w_akt->argv[w_akt->nArgsUsed]);
        		w_akt->nArgsUsed ++;        		
		};
param	:	STRING
		{
                   	w_akt->argv[w_akt->nArgsUsed] = (char *) malloc(strlen((char*)$param.text->chars));
        		strcpy(w_akt->argv[w_akt->nArgsUsed], (char*) $param.text->chars);	
        		LOG("PARAMETER '\%s' gefunden ... wird an die Stelle \%d geschrieben\n",w_akt->argv[w_akt->nArgsUsed],w_akt->nArgsUsed); 
        		w_akt->nArgsUsed ++;
		};
file	:	STRING;

redir	:	(BLANK? inredir)
	|	(BLANK? outredir);
	
exe 	:	binary (BLANK param)*;

inredir
        :       INPUT_REDIR BLANK? file 
        	{
			w_akt->szInRedir = (char *) malloc(strlen((char*)$file.text->chars));
        		strcpy(w_akt->szInRedir, (char*) $file.text->chars);
			LOG("EINGABESTROM wird umgeleitet in '\%s' \n",w_akt->szInRedir);;
        	};

outredir
        :       OUTPUT_REDIR BLANK? file 
        	{
			w_akt->szOutRedir = (char *) malloc(strlen((char*)$file.text->chars));
        		strcpy(w_akt->szOutRedir, (char*) $file.text->chars);
			LOG("AUSGABESTROM wird umgeleitet in '\%s' \n",w_akt->szOutRedir);
        	};


/*------------------------------------------------------------------
 * LEXER RULES
 *------------------------------------------------------------------*/
fragment CHAR  	: ('0'..'9'|'A'..'Z'|'a'..'z'|'/'|'-'|'*'|'.');
BLANK	: ( '\t' | ' ')+;
STRING  	: (CHAR)+;
