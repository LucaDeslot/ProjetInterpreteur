#include "SymboleValue.h"
#include "Exceptions.h"

#include <stdlib.h>
#include <algorithm>
#include <iomanip>
#include <iterator>
#include <string>

SymboleValue::SymboleValue(Symbole const & s) :
        Symbole(s.getChaine()) {
    if (s == "<ENTIER>") {
        m_valeur = atoi(s.getChaine().c_str()); // c_str convertit une string en char*
        m_defini = true;
        type = VarType::INTEGER;
    } else if (s == "<chaine>") {
        m_defini = true;
        m_string = s.getChaine();
        type = VarType::STRING;
    } else {
        m_defini = false;
        type = VarType::UNDEFINED;
    }
}

int SymboleValue::executer() {
    if (!m_defini)
        throw IndefiniException(); // on lève une exception si valeur non définie
    if (type != VarType::INTEGER)
        throw IndefiniException(); // on lève une exception si valeur non définie
    return m_valeur;
}

void SymboleValue::traduitCpp(unsigned int ind, ostream& out) {
    if (*this == "<VARIABLE>" && !estDefini()) {
        out << setw(ind * 4) << "";
        if (type == VarType::STRING) {
            out << "string ";
        } else {
            out << "int ";
        }
        out << getChaine() << ";";
    } else {
        out << getChaine();
    }
}

ostream & operator<<(ostream & cout, SymboleValue const & symbole) {
    cout << (Symbole) symbole << "\t\t - Valeur=";
    if (symbole.m_defini) {
        if (symbole == "<CHAINE>") {
            cout << symbole.getString() << " ";
        } else if (symbole == "<VARIABLE>" && symbole.type == SymboleValue::VarType::STRING) {
            cout << symbole.getString() << " ";
        } else {
            cout << symbole.m_valeur << " ";
        }
    } else {
        cout << "indefinie ";
    }
    return cout;
}
