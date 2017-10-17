#include "ArbreAbstrait.h"
#include "Exceptions.h"
#include "Interpreteur.h"
#include "TableSymboles.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv) {
    
    // To take the file motsCles.txt to perform correctly the execution
    fstream motCDist("../motsCles.txt", ios::in);
    fstream motC("motsCles.txt");
    if (!motCDist.good() && !motC.good()) {
        throw FichierException();
    } else if (motCDist.good()) {
        motC.open("motsCles.txt", ios::out);
        motC << motCDist.rdbuf();
    }
    motC.close();
    motCDist.close();
    //-----------------------
    
    string nomFich;
    if (argc != 2) {
        cout << "Usage : " << argv[0] << " nom_fichier_source" << endl << endl;
        cout << "Entrez le nom du fichier que voulez-vous interpréter : ";
        getline(cin, nomFich);
    } else {
        nomFich = argv[1];
    }
    ifstream fichier(nomFich);
    try {
        Interpreteur interpreteur(fichier);
        interpreteur.analyse();
        // Si pas d'exception levée, l'analyse syntaxique a réussi
        cout << endl << "================ Syntaxe Correcte" << endl;
        // On affiche le contenu de la table des symboles avant d'exécuter le programme
        cout << endl << "================ Table des symboles avant exécution : "
                << interpreteur.getTable();
        cout << endl << "================ Execution de l'arbre" << endl;
        // On exécute le programme si l'arbre n'est pas vide
        if (interpreteur.getArbre() != nullptr)
            interpreteur.getArbre()->executer();
        // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
        cout << endl << "================ Table des symboles apres exécution : "
                << interpreteur.getTable();
    } catch (InterpreteurException & e) {
        cout << e.what() << endl;
    }
    return 0;
}
