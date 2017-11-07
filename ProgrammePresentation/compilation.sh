#!/bin/zsh
./PI -c ../programmeAll.txt progTest.cpp
echo "\e[32mProgramme Compilé !\e[0m"
echo "\e[32mCompilation avec g++ (appuyer sur entrer)\e[0m"
read
g++ progTest.cpp -o progTest
echo "\e[36mCompilation avec g++ Terminée\e[0m"
echo "\e[33mSuppression du .cpp\e[0m"
rm progTest.cpp
echo "\e[32mLancement du programme cpp (appuyer sur entrer)\e[0m"
read
./progTest
echo "\e[36mLancement du programme cpp Terminé\e[0m"
echo "\e[33mSuppression des elements\e[0m"
rm progTest motsCles.txt