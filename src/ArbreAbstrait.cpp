#include "ArbreAbstrait.h"
#include "SymboleValue.h"

#include <limits>
#include <string>
#include <typeinfo>

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
    string red;
    for (unsigned int i = 0; i < vars.size(); ++i) {
        cin >> red;
        red = '"' + red + '"'; // to make a string !!

        // Ignore to the end of line
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        ((SymboleValue*) vars.at(i))->setValeur(red);
    }
    return 0;
}

void NoeudInstLire::ajoute(Noeud* var) {
    vars.push_back(var);
}
