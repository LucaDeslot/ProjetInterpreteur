#include "ArbreAbstrait.h"
#include "SymboleValue.h"

#include <limits>
#include <string>
#include <typeinfo>

void indent(unsigned int ind, ostream &out) {
    out << setw(ind * 4) << "";
}

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() :
        m_instructions() {
}

int NoeudSeqInst::executer() {
    for (unsigned int i = 0; i < m_instructions.size(); i++)
        m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::traduitCpp(unsigned int ind, ostream& out) {
    for (unsigned int i = 0; i < m_instructions.size(); i++) {
        indent(ind, out);
        m_instructions[i]->traduitCpp(ind, out); // on ecrit chaque instruction de la séquence
        if (typeid(NoeudAffectation) == typeid(*m_instructions[i])) {
            out << ";";
        }
        out << endl;
    }
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
    if (instruction != nullptr)
        m_instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression) :
        m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
    if (typeid(*m_expression) == typeid(SymboleValue)
            && *((SymboleValue*) m_expression) == "<chaine>") {
        string valeur = ((SymboleValue*) m_expression)->getChaine();
        ((SymboleValue*) m_variable)->setValeur(valeur);
    } else {
        int valeur = m_expression->executer(); // On exécute (évalue) l'expression
        ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
    }
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudAffectation::traduitCpp(unsigned int ind, ostream& out) {
    if (!((SymboleValue*) m_variable)->estDefini()) {
        executer();
        if (((SymboleValue*) m_variable)->isTypeOf(SymboleValue::VarType::STRING)) {
            out << "string ";
        } else {
            out << "int ";
        }
    }
    out << ((SymboleValue*) m_variable)->getChaine();
    out << " = ";
    ((SymboleValue*) m_expression)->traduitCpp(ind, out);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche,
        Noeud* operandeDroit) :
        m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
//    if ((typeid(*m_operandeGauche) == typeid(SymboleValue)
//            && *((SymboleValue*) m_operandeGauche) == "<CHAINE>")) {
//
//    } TODO : add operande to concatenate strings
    int og, od, valeur;
    if (m_operandeGauche != nullptr)
        og = m_operandeGauche->executer(); // On évalue l'opérande gauche
    if (m_operandeDroit != nullptr)
        od = m_operandeDroit->executer(); // On évalue l'opérande droit
    // Et on combine les deux opérandes en fonctions de l'opérateur
    if (this->m_operateur == "+")
        valeur = (og + od);
    else if (this->m_operateur == "-")
        valeur = (og - od);
    else if (this->m_operateur == "*")
        valeur = (og * od);
    else if (this->m_operateur == "==")
        valeur = (og == od);
    else if (this->m_operateur == "!=")
        valeur = (og != od);
    else if (this->m_operateur == "<")
        valeur = (og < od);
    else if (this->m_operateur == ">")
        valeur = (og > od);
    else if (this->m_operateur == "<=")
        valeur = (og <= od);
    else if (this->m_operateur == ">=")
        valeur = (og >= od);
    else if (this->m_operateur == "et")
        valeur = (og && od);
    else if (this->m_operateur == "ou")
        valeur = (og || od);
    else if (this->m_operateur == "non")
        valeur = (!og);
    else if (this->m_operateur == "/") {
        if (od == 0)
            throw DivParZeroException();
        valeur = og / od;
    }
    return valeur; // On retourne la valeur calculée
}

void NoeudOperateurBinaire::traduitCpp(unsigned int ind, ostream& out) {
    if (m_operandeGauche != nullptr)
        m_operandeGauche->traduitCpp(ind, out); // On évalue l'opérande gauche

//    out << setw(ind) << "";
    out << " " << m_operateur.getChaine() << " ";

    if (m_operandeDroit != nullptr)
        m_operandeDroit->traduitCpp(ind, out);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence) {
    ajoute(condition);
    ajoute(sequence);
}

int NoeudInstSi::executer() {
    unsigned var = 0;
    for (var = 0; var < conditions.size() && !conditions.at(var)->executer(); ++var) {
    }
    if (var < conditions.size() && conditions.at(var)->executer()) // si une condition a été remplie (et on la retest : useless)
        seqIntructions.at(var)->executer();
    else if (seqIntructions.size() > conditions.size()) { // sinon, on regarde si il y a un "sinon"
        seqIntructions.at(var)->executer();
    }
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstSi::traduitCpp(unsigned int ind, ostream& out) {
    unsigned var = 0;
    for (var = 0; var < conditions.size(); ++var) {
        out << (var == 0 ? "" : "else ") << "if (";
        conditions.at(var)->traduitCpp(ind, out);
        out << ") {" << endl;
        seqIntructions.at(var)->traduitCpp(ind + 1, out);
        indent(ind, out);
        out << "} ";
    }
    if (seqIntructions.size() > conditions.size()) { // sinon, on regarde si il y a un "sinon"
        out << "else {" << endl;
        seqIntructions.at(var)->traduitCpp(ind + 1, out);
        indent(ind, out);
        out << "}" << endl;
    }

}

void NoeudInstSi::ajoute(Noeud* instruction) {
    if (typeid(*instruction) == typeid(NoeudSeqInst)) {
        seqIntructions.push_back(instruction);
    } else {
        conditions.push_back(instruction);
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////
NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* seqInst) :
        condition(condition), seqInst(seqInst) {
}

int NoeudInstTantQue::executer() {
    while (condition->executer()) {
        seqInst->executer();
    }
    return 0;
}

void NoeudInstTantQue::traduitCpp(unsigned int ind, ostream& out) {
    out << "while (";
    condition->traduitCpp(ind, out);
    out << ") {" << endl;
    seqInst->traduitCpp(ind + 1, out);
    indent(ind, out);
    out << "}" << endl;
}
////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////
NoeudInstRepeter::NoeudInstRepeter(Noeud* seqInst, Noeud* condition) :
        NoeudInstTantQue(condition, seqInst) {
}

int NoeudInstRepeter::executer() {
    do {
        seqInst->executer();
    } while (!condition->executer());
    return 0;
}

void NoeudInstRepeter::traduitCpp(unsigned int ind, ostream& out) {
    out << "do {" << endl;
    seqInst->traduitCpp(ind + 1, out);
    indent(ind, out);
    out << "} while (!(";
    condition->traduitCpp(ind, out);
    out << "));" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////
NoeudInstPour::NoeudInstPour(Noeud* seqInst, Noeud* condition, Noeud* affect1, Noeud* affect2) :
        NoeudInstTantQue(seqInst, condition), affect1(affect1), affect2(affect2) {
}

int NoeudInstPour::executer() {
    if (affect1 && affect2) {
        for (affect1->executer(); condition->executer(); affect2->executer()) {
            seqInst->executer();
        }
    } else if (affect1) {
        for (affect1->executer(); condition->executer();) {
            seqInst->executer();
        }
    } else if (affect2) {
        for (; condition->executer(); affect2->executer()) {
            seqInst->executer();
        }
    } else {
        for (; condition->executer();) {
            seqInst->executer();
        }
    }
    return 0;
}

void NoeudInstPour::traduitCpp(unsigned int ind, ostream& out) {
    out << "for (";
    if (affect1)
        affect1->traduitCpp(ind, out);
    out << ";";

    condition->traduitCpp(ind, out);
    out << ";";
    if (affect2)
        affect2->traduitCpp(ind, out);
    out << ") {" << endl;
    seqInst->traduitCpp(ind + 1, out);
    indent(ind, out);
    out << "}" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////
NoeudInstEcrire::NoeudInstEcrire(Noeud* exp) {
    ajoute(exp);
}

int NoeudInstEcrire::executer() {
    for (auto exp : exps) {
        if (typeid(*exp) == typeid(SymboleValue)) {
            if (((SymboleValue*) exp)->isTypeOf(SymboleValue::VarType::STRING)) {
                string str = ((SymboleValue*) exp)->getString();
                for (unsigned int i = 0; i < str.size(); i++) {
                    if (str[i] == '\\') {
                        i++;
                        switch (str[i]) {
                            case 'n':
                                cout << endl;
                                break;
                            case '\\':
                                cout << '\\';
                                break;
                            case 't':
                                cout << '\t';
                                break;
                            case '"':
                                cout << '\"';
                                break;
                            default:
                                throw IndefiniEscapeCharException();
                                break;
                        }
                    } else {
                        cout << str[i];
                    }
                }
            } else {
                cout << exp->executer();
            }
        }
    }
    return 0;
}

void NoeudInstEcrire::traduitCpp(unsigned int ind, ostream& out) {
    out << "cout";
    for (auto exp : exps) {
        out << " << ";
        exp->traduitCpp(ind, out);
    }
    out << ";";
}

void NoeudInstEcrire::ajoute(Noeud* instruction) {
    exps.push_back(instruction);
}
////////////////////////////////////////////////////////////////////////////////
// NoeudInstLire
////////////////////////////////////////////////////////////////////////////////
NoeudInstLire::NoeudInstLire(Noeud* var) {
    ajoute(var);
}

int NoeudInstLire::executer() {
    string sred;
    int ired;
    for (unsigned int i = 0; i < vars.size(); ++i) {
        cin >> sred;
        // Ignore to the end of line
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        Symbole s(sred);

        if (s == "<ENTIER>") {
            ired = atoi(s.getChaine().c_str());
            ((SymboleValue*) vars.at(i))->setValeur(ired);
        } else {
            sred = '"' + sred + '"'; // to make a string !!
            ((SymboleValue*) vars.at(i))->setValeur(sred);
        }

    }
    return 0;
}
void NoeudInstLire::traduitCpp(unsigned int ind, ostream& out) {
    out << "cin";
    for (auto exp : vars) {
        out << " >> ";
        exp->traduitCpp(ind, out);
    }
    out << ";";
}

void NoeudInstLire::ajoute(Noeud* var) {
    vars.push_back(var);
}
