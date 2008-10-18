/*
 * gsh.c
 *
 *  Created on: 06.10.2008
 *      Author: jars
 */


 #include "wswdLexer.h"
 #include "wswdParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
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

		 // wir verabschieden uns wenn der Benutzer exit eingibt
		 if(strcmp(user_input,"exit") == 0)
		 {
			 break;
		 }
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

	//printf("Anweisung : %s \n", thats_to_do);
	//return;

	// wir lesen vom uebergebenen String
	input  = antlr3NewAsciiStringInPlaceStream((pANTLR3_UINT8)thats_to_do,strlen(thats_to_do),NULL);
	if(input == NULL)
	{
		printf("### Fehler beim Eryeugen des Inputstreams");
	}

	lex    = wswdLexerNew                		(input);
	tokens = antlr3CommonTokenStreamSourceNew  	(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
	parser = wswdParserNew               		(tokens);

	parser -> cmd_line(parser);
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



