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

/*------------------------------------------------------------------
 * PARSER RULES
 *------------------------------------------------------------------*/
 
cmd_line       
        :	process (process)* BLANK?
        |	EXIT;

pipecreator 
	: (BLANK? pipeto)
	{
		LOG("Dieser Prozess braucht eine Pipe\n");
	};
		
process     :   BLANK? exe redir* pipecreator?

		{
			/*
			int i;
			int fd_out, fd_in;
			LOG("FORK [Eingabeumleitung aktiv = \%d, Ausgabeumleitung aktiv = \%d]\n", (szInRedir == NULL)?0:1, (szOutRedir == NULL)?0:1);

			if((i = fork()) == 0) //child
			{
			
				// Any redirections ?
				if(szInRedir != NULL)
				{
      					LOG("INPUT Redirection wird durchgefuehrt");
      					fd_in = open(szInRedir, O_RDONLY);
      					dup2(fd_in, fileno(stdin)); // redirect to stdout
      					close(fd_in); // Close unused file descriptors
				}
								
				if(szOutRedir != NULL)
				{
					LOG("OUTPUT Redirection wird durchgefuehrt");
					fd_out = open(szOutRedir, O_RDWR|O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
					dup2(fd_out, fileno(stdout)); // redirect to stdout
					close(fd_out); // Close unused file descriptors
				}	
				
				// Here, the process is started
				i = execvp(argv[0], argv);
				LOG("ERROR : Creation of process [\%s] returned : \%d \n",(char*)$exe.text->chars,i);
				
				// SPeicher freigeben
				cleanup();
				
				exit(0);
			}
			
			waitpid(i, 0 , 0);		
			
			// Speicher auch im Parent freigeben
			cleanup();*/
		};

pipeto    :	'|';
binary 	:	STRING
		{
        		/* 
        		argv[0] = (char *) malloc(strlen((char*)$binary.text->chars));
        		strcpy(argv[0], (char*) $binary.text->chars);	
        		LOG("BINARY '\%s' gefunden ... wird an die Stelle 0 geschrieben\n",argv[0]);
        		*/
		};
param	:	STRING
		{
        		/*
        		nArgsUsed ++;
                   	argv[nArgsUsed] = (char *) malloc(strlen((char*)$param.text->chars));
        		strcpy(argv[nArgsUsed], (char*) $param.text->chars);	
        		LOG("PARAMETER '\%s' gefunden ... wird an die Stelle \%d geschrieben\n",argv[nArgsUsed],nArgsUsed); 
        		*/
		};
file	:	STRING;

redir	:	(BLANK? inredir)
	|	(BLANK? outredir);
	
exe 	:	binary (BLANK param)*;

inredir
        :       INPUT_REDIR BLANK? file 
        	{
			/*
			szInRedir = (char *) malloc(strlen((char*)$file.text->chars));
        		strcpy(szInRedir, (char*) $file.text->chars);
			LOG("EINGABESTROM wird umgeleitet in '\%s' \n",szInRedir);;
			*/
        	};

outredir
        :       OUTPUT_REDIR BLANK? file 
        	{
			/*
			szOutRedir = (char *) malloc(strlen((char*)$file.text->chars));
        		strcpy(szOutRedir, (char*) $file.text->chars);
			LOG("AUSGABESTROM wird umgeleitet in '\%s' \n",szOutRedir);
			*/
        	};


/*------------------------------------------------------------------
 * LEXER RULES
 *------------------------------------------------------------------*/
fragment CHAR  	: ('0'..'9'|'A'..'Z'|'a'..'z'|'/'|'-'|'*'|'.');
BLANK	: ( '\t' | ' ')+;
STRING  	: (CHAR)+;
