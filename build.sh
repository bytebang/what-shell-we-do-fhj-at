#!/bin/sh

echo "building antlr c files"
java -cp $CLASSPATH:./Tools/antlr-3.1.1.jar org.antlr.Tool  -fo ./antlr/output ./antlr/wswd.g 

echo "compiling everything"
gcc -Wall -I/usr/include -I/usr/local/include -I"/home/jars/workspace/what-shell-we-do/src" -I"/home/jars/workspace/what-shell-we-do/antlr/output" -g3  -c ./src/wswd_main.c ./antlr/output/wswdLexer.c ./antlr/output/wswdParser.c 

echo "Linking everything together"
gcc -L/usr/local/lib -lantlr3c -o"what-shell-we-do" *.o
