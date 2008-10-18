
#include<sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
/**
  * Zentrale Logfunktion fuer interne Loggings.
  * Diese funktion kann in der Releaseversion unterdrueckt werden.
  */
#define LOG(...)// printf(" >> LOG : " __VA_ARGS__ )

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

