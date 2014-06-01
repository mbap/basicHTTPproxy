#! /bin/bash

doxygen finalproj.cfg
cd latex
pdflatex refman.tex
cp refman.pdf ../proxyCodeDocs.pdf
cd ..
rm -r html 
rm -r latex

#htmldoc --webpage -f lab3docs.pdf ./html/*.html

