Compilieren mittels des build.sh Files
======================================

Um das Projekt ohne Eclipse zu kompilieren muss lediglich das build.sh File ausfuehren.
Voraussetzung ist, dass die antlr3 library unter /usr/local/lib zu finden ist.



+++++++++++++++++++++++++++++++++++++++
====          ECLIPSE CDT          ====
+++++++++++++++++++++++++++++++++++++++

Compilieren des Projekts :
==========================

Um das Projekt kompilieren zu koennen muss ANTLR V3.1 am Szstem installiert sein.
Das ist unter http://www.antlr.org/ erhaeltlich.

Danach muss aus dem Grammatikfile (./antlr/wswd.g) der C Code erzeugt werden.
Am besten geht das mit antlrworks (./tools/antlrworks/antlrworks-1.2.1.jar) indem man das
Grammatikfile laedt und auf "Generate->Generate Code" klickt.

Die erzeugten C Files werden unter ./antlr/output abgelegt.

Jetzt kann das Projekt in Eclipse CDT kompiliert werden.

Ausfuehren der shell:
=====================

Um das binary ausfuehren zu koennen muss der Pfad zur antlr library (LD_LIBRARY_PATH) 
gesetzt werden. Diese ist (bei einer ANTLR Standardinstallation) unter /usr/local/lib
zu finden.

Um das Projekt aus Eclipse ausfuehren zu koennen muss unter "Run->Run Configurations->Environment" 
die Variable LD_LIBRARY_PATH auf den Wert /usr/local/lib gesetzt werden.

Ab diesem Zeitpunkt sollte die shell ausfuehrbar sein.

