# ProjetInterpreteur
Projet interpréteur C++ s3\
## Features :
### Primary Features
- [x] Instruction si/sisinon/sinon </li>
- [x] Instruction tantque </li>
- [x] Instruction repeter </li>
- [x] Instruction pour </li>
- [x] Instruction ecrire </li>
- [x] Instruction lire </li>
- [x] Recuperation après erreur (partial) </li>
### Secondary Features
- [x] Generation code cpp (compile) </li>
- [x] Test cours et pertinents </li>
- [x] Test Unitaire du si </li>
- [x] Priorite des operateurs </li>
- [x] Instruction supplementaire (selon) </li>
- [x] Variables de chaine </li>
- [ ] Procedures/Fonctions </li>


## Installation
executer dans Debug ou Testing (test unitaire) : 
```zsh
make all
```
Verifier qu'il y a bien le fichier motsCles.txt à l'endroit de l'execution ! (bien évidemment, à chager dans une prochaine version)

## Exemples
### Debug
maintenant, vous pouvez executer un programme ainsi :
```zsh
./ProjetInterpreteur -i fichierSource.txt
```
ou compiler en c++ :
```zsh
./ProjetInterpreteur -c fichierSource.txt [dest.cpp]
```
### Testing
```zsh
./ProjetInterpreteur
```
## Contributors
Nous avons été deux à travailler sur ce projet :
- Luca Deslot
- Ludovic Jozeau
