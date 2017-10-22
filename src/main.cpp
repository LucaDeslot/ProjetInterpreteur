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
    if (argc < 3) {
        cout << "Usage (interpretation): " << argv[0] << " -i nom_fichier_source" << endl;
        cout << "Usage (compilation cpp): " << argv[0] << " -c nom_fichier_source"
                << " [nom_fichier_destination]" << endl;
        return -1;
    } else if (argv[1][0] == '-' && (argv[1][1] == 'i' || argv[1][1] == 'c')) {
        nomFich = argv[2];
        ifstream fichier(nomFich);
        try {
            Interpreteur interpreteur(fichier);
            interpreteur.analyse();
            // Si pas d'exception levée, l'analyse syntaxique a réussi
            cout << endl << "//================ Syntaxe Correcte" << endl;

            // interpretation
            // On affiche le contenu de la table des symboles avant d'exécuter le programme
            if (argv[1][1] == 'i' && argc == 3) {
                cout << endl << "================ Table des symboles avant exécution : "
                        << interpreteur.getTable();
                cout << endl << "================ Execution de l'arbre" << endl;
                // On exécute le programme si l'arbre n'est pas vide
                if (interpreteur.getArbre() != nullptr)
                    interpreteur.getArbre()->executer();
                // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
                cout << endl << "================ Table des symboles apres exécution : "
                        << interpreteur.getTable();
                return 0;
            }

            // compilation
            else if (argv[1][1] == 'c' && (argc == 4 || argc == 3)) {
                ostream *fichierOut;
                if (argc == 4) {
                    fichierOut = new ofstream(argv[3]);
                } else {
                    fichierOut = &cout;
                }
                // On exécute le programme si l'arbre n'est pas vide
                if (interpreteur.getArbre() != nullptr) {
                    interpreteur.traduitCpp(0, *fichierOut);
                }
                return 0;
            } else {
                cout << "nombre d'arguments invalide : " << endl;
                cout << "Usage (interpretation): " << argv[0] << " -i nom_fichier_source" << endl;
                cout << "Usage (compilation cpp): " << argv[0] << " -c nom_fichier_source"
                        << " [nom_fichier_destination]" << endl;
            }
        } catch (InterpreteurException & e) {
            cout << e.what() << endl;
        }
    } else {
        cout << "argument 2 invalide : " << argv[1] << endl;
        cout << "Usage (interpretation): " << argv[0] << " -i nom_fichier_source" << endl;
        cout << "Usage (compilation cpp): " << argv[0] << " -c nom_fichier_source"
                << " [nom_fichier_destination]" << endl;
    }
    return -2;
}
