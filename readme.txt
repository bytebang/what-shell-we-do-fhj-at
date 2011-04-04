Compilieren mittels des build.sh Files
======================================

Um das Projekt ohne Eclipse zu kompilieren muss lediglich das build.sh File ausfuehren.
Voraussetzung ist, dass die antlr3 library unter /usr/local/lib zu finden ist.


Features :
==========

- Input redirection
- Output redirection
- Beliebig viele Pipes 
- Anzeige des aktuellen Verzeichnisses
- Wechseln des aktuellen verzeichnisses 'cd'
- Anzeige des aktuellen Benutzers
- Benden der Shell mit 'exit'


+++++++++++++++++++++++++++++++++++++++
====          ECLIPSE CDT          ====
+++++++++++++++++++++++++++++++++++++++

Compilieren des Projekts :
==========================

Um das Projekt kompilieren zu koennen muss ANTLR V3.1 am System installiert sein.
Das ist unter http://www.antlr.org/download/ erhaeltlich.

Danach muss aus dem Grammatikfile (./antlr/wswd.g) der C Code erzeugt werden.
Am besten geht das mit antlrworks (./tools/antlrworks/antlrworks-1.2.1.jar) indem man das
Grammatikfile laedt und auf "Generate->Generate Code" klickt.

Die erzeugten C Files werden unter ./antlr/output abgelegt.

Jetzt kann das Projekt in Eclipse CDT kompiliert werden.
Die verwendete antlr3c library wird direkt in das binary gelinkt. 

Die Shell kann danach ausgefuehrt werden.

Anmerkung Shared library : Moechte man antlr3c als shared library verwenden, 
muss man die Linkeropition -static entfernen und die Umgebungsvariable 
LD_LIBRARY_PATH auf den entsprechenden Pfad (/usr/local/lib) setzen.

