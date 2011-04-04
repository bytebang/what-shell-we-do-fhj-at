#!/bin/sh

echo "----------------- building antlr c files from grammar ------------------------"
java -cp $CLASSPATH:./Tools/antlr-3.2.jar org.antlr.Tool  -fo ./antlr/output ./antlr/wswd.g 
if [ "$?" -ne "0" ]; then
  echo "Sorry, there is a problem with antlr. Make sure that you have java installed"
  exit 1
fi
echo ""
echo "... sucessfully finished building c files from grammar"
echo ""

echo "------------------------- compiling everything -------------------------------"
gcc -I/usr/include -I/usr/local/include -I"./src" -I"./antlr/output" -O0 -g3 -Wall -c ./src/wswd_main.c ./antlr/output/wswdLexer.c ./antlr/output/wswdParser.c 
if [ "$?" -ne "0" ]; then
  echo "Sorry, there is a problem during compiling. Make sure that you have gcc installed, and that tere are no errors in the code."
  exit 1
fi
echo ""
echo "... sucessfully finished compiling"
echo ""

echo "-------------------------- Linking everything together -----------------------"
gcc  -o"what-shell-we-do" *.o -static -L/usr/local/lib -lantlr3c
if [ "$?" -ne "0" ]; then
  echo "Sorry, there is a problem during linking. Make sure that you have the antlr3c library installed in /usr/local/lib"
  exit 1
fi
echo ""
echo "... sucessfully finished linking"
echo ""


echo "---------------------------- Removing build objects --------------------------"
rm ./*.o
echo ""
echo "... sucessfully deleted all objects"
echo ""
echo ""

echo "---------------------------- Creating documentation --------------------------"
rm -rf ./doc/html/
doxygen  ./doc/Doxyfile > /dev/null
if [ "$?" -ne "0" ]; then
  echo "Sorry, there is a problem with the documentation. Make sure that you have doxygen and graphviz installed."
  exit 1
fi
echo ""
echo "... sucessfully created the documentation"
echo ""
echo ""

echo "##############################################################################"
echo "#Finished. you can start the shell by typing ./what-shell-we-do at your shell#"
echo "##############################################################################"
echo ""
echo ""

