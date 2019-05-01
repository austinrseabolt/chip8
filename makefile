CC=g++

checkforout: checkformaincpp
	g++ -g main.o -o 8chipper -lsfml-graphics -lsfml-window -lsfml-system

checkformaincpp: main.cpp 
	g++ -c main.cpp


