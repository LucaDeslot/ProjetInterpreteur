#ifndef SYMBOLEVALUE_H
#define SYMBOLEVALUE_H

#include <string.h>
#include <iostream>
using namespace std;

#include "Symbole.h"
#include "ArbreAbstrait.h"



class SymboleValue: public Symbole, // Un symbole valué est un symbole qui a une valeur (définie ou pas)
        public Noeud {  //  et c'est aussi une feuille de l'arbre abstrait
public:
    enum class VarType {
        UNDEFINED, STRING, INTEGER
    };
    SymboleValue(Symbole const & s); // Construit un symbole valué à partir d'un symbole existant s
    ~SymboleValue() {
    }
    int executer();         // exécute le SymboleValue (revoie sa valeur !)

    string getString() const;
    void setValeur(int valeur);
    void setValeur(string valeur);
    bool estDefini();
    bool isTypeOf(SymboleValue::VarType type);

    friend ostream & operator <<(ostream & cout, SymboleValue const & symbole); // affiche un symbole value sur cout

private:

    bool m_defini;	// indique si la valeur du symbole est définie
    int m_valeur;	// valeur du symbole si elle est définie, zéro sinon
    string m_string;
    VarType type;
};

inline string SymboleValue::getString() const {
    // we take only the string without quotes
    return m_string.substr(1, m_string.size() - 2);
}

inline void SymboleValue::setValeur(string valeur) {
    this->m_string = valeur;
    m_defini = true;
    type = VarType::STRING;
} // mutateur

inline void SymboleValue::setValeur(int valeur) {
    this->m_valeur = valeur;
    m_defini = true;
    type = VarType::INTEGER;
} // mutateur

inline bool SymboleValue::estDefini() {
    return m_defini;
} // accesseur

inline bool SymboleValue::isTypeOf(SymboleValue::VarType type) {
    return this->type == type;
}

#endif /* SYMBOLEVALUE_H */
