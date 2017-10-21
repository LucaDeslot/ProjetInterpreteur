#include "Interpreteur.h"
#include "SymboleValue.h"

#include <cstdio>
#include <iostream>
#include <string>

using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
        m_lecteur(fichier), m_table(), m_arbre(nullptr) {
}

void Interpreteur::analyse() {
    m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::tester(string const & symboleAttendu) const throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
    static char messageWhat[256];
    if (m_lecteur.getSymbole() != symboleAttendu) {
        sprintf(messageWhat,
                "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
                m_lecteur.getLigne(), m_lecteur.getColonne(), symboleAttendu.c_str(),
                m_lecteur.getSymbole().getChaine().c_str());
        throw SyntaxeException(messageWhat);
    }
}

void Interpreteur::testerEtAvancer(string const & symboleAttendu) throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
    tester(symboleAttendu);
    m_lecteur.avancer();
}

void Interpreteur::erreur(string const & message) const throw (SyntaxeException) {
    // Lève une exception contenant le message et le symbole courant trouvé
    // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
    static char messageWhat[256];
    sprintf(messageWhat, "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(),
            m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
}

bool Interpreteur::isInst(Symbole symb) {
    return symb == "<VARIABLE>" || symb == "si" || symb == "tantque" || symb == "repeter"
            || symb == "pour" || symb == "ecrire" || symb == "lire";
}

Noeud* Interpreteur::programme() {
    // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    testerEtAvancer("procedure");
    testerEtAvancer("principale");
    testerEtAvancer("(");
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finproc");
    tester("<FINDEFICHIER>");
    return sequence;
}

Noeud* Interpreteur::seqInst() {
    // <seqInst> ::= <inst> { <inst> }
    NoeudSeqInst* sequence = new NoeudSeqInst();
    do {
        sequence->ajoute(inst());
    } while (isInst(m_lecteur.getSymbole()));
    // Tant que le symbole courant est un début possible d'instruction...
    // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
    return sequence;
}

Noeud* Interpreteur::inst() {
    // <inst> ::= <affectation>  ; | <instSi>
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        Noeud *affect = affectation();
        testerEtAvancer(";");
        return affect;
    } else if (m_lecteur.getSymbole() == "si") {
        return instSi();
    } else if (m_lecteur.getSymbole() == "tantque") {
        return instTantQue();
    } else if (m_lecteur.getSymbole() == "repeter") {
        return instRepeter();
    } else if (m_lecteur.getSymbole() == "pour") {
        return instPour();
    } else if (m_lecteur.getSymbole() == "ecrire") {
        return instEcrire();
    } else if (m_lecteur.getSymbole() == "lire") {
        return instLire();
    } else {
        erreur("Instruction incorrecte");
    }
    return nullptr;
}

Noeud* Interpreteur::affectation() {
    // <affectation> ::= <variable> = <expression>
    tester("<VARIABLE>");
    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
    m_lecteur.avancer();
    testerEtAvancer("=");
    Noeud* exp = expression();             // On mémorise l'expression trouvée
    return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
    // <expression> ::= <facteur> { <opBinaire> <facteur> }
    //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
    Noeud* fact = facteur();
    while (m_lecteur.getSymbole() == "+" || m_lecteur.getSymbole() == "-"
            || m_lecteur.getSymbole() == "*" || m_lecteur.getSymbole() == "/"
            || m_lecteur.getSymbole() == "<" || m_lecteur.getSymbole() == "<="
            || m_lecteur.getSymbole() == ">" || m_lecteur.getSymbole() == ">="
            || m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!="
            || m_lecteur.getSymbole() == "et" || m_lecteur.getSymbole() == "ou") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* factDroit = facteur(); // On mémorise l'opérande droit
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construit un noeud opérateur binaire
    }
    return fact; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::facteur() {
    // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
    Noeud* fact = nullptr;
    if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>"
            || m_lecteur.getSymbole() == "<chaine>") {
        fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier ou le string à la table
        m_lecteur.avancer();
    } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")),
                facteur());
    } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
    } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
        m_lecteur.avancer();
        fact = expression();
        testerEtAvancer(")");
    } else
        erreur("Facteur incorrect");
    return fact;
}

Noeud* Interpreteur::instSi() {
    // <instSi> ::= si ( <expression> ) <seqInst> finsi
    testerEtAvancer("si");
    testerEtAvancer("(");
    Noeud* condition = expression(); // On mémorise la condition
    testerEtAvancer(")");
    Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
    Noeud *noeudSi = new NoeudInstSi(condition, sequence); //create the node for if (basic)

    // le sinonsi sequence
    while (m_lecteur.getSymbole() == "sinonsi") {
        testerEtAvancer("sinonsi");
        testerEtAvancer("(");
        noeudSi->ajoute(expression()); // On mémorise la condition
        testerEtAvancer(")");
        noeudSi->ajoute(seqInst());     // On mémorise la séquence d'instruction
    }
    // le sinon
    if (m_lecteur.getSymbole() == "sinon") {
        testerEtAvancer("sinon");
        noeudSi->ajoute(seqInst());     // On mémorise la séquence d'instruction
    }
    testerEtAvancer("finsi");
    return noeudSi; // Et on renvoie un noeud Instruction Si
}

Noeud* Interpreteur::instTantQue() {
// <instTantQue> ::= tantque ( <expression> ) <seqInst> fintantque
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expression(); // On mémorise la condition
    testerEtAvancer(")");
    Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition, sequence);
}

Noeud* Interpreteur::instRepeter() {
    testerEtAvancer("repeter");
    Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expression(); // On mémorise la condition
    testerEtAvancer(")");
    return new NoeudInstRepeter(sequence, condition);
}

Noeud* Interpreteur::instPour() {
    Noeud* affect = nullptr;
    Noeud* affect2 = nullptr;
    testerEtAvancer("pour");
    testerEtAvancer("(");
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        affect = affectation();
    }
    testerEtAvancer(";");
    Noeud* condition = expression();
    testerEtAvancer(";");
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        affect2 = affectation();
    }

    testerEtAvancer(")");
    Noeud* inst = seqInst();
    testerEtAvancer("finpour");
    return new NoeudInstPour(condition, inst, affect, affect2);
}

Noeud* Interpreteur::instEcrire() {
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    Noeud* instEcrire = new NoeudInstEcrire(expression());
    while (m_lecteur.getSymbole() == ",") {
        testerEtAvancer(",");
        instEcrire->ajoute(expression());
    }
    testerEtAvancer(")");
    testerEtAvancer(";");
    return instEcrire;
}

/**
 * this methods allow reading on standard input as cin (take only the first word of the line !)
 * @return
 */
Noeud* Interpreteur::instLire() {
    testerEtAvancer("lire");
    testerEtAvancer("(");
    tester("<VARIABLE>");
    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole());
    m_lecteur.avancer();
    Noeud* instLire = new NoeudInstLire(var);

    while (m_lecteur.getSymbole() == ",") {
        testerEtAvancer(",");
        tester("<VARIABLE>");
        instLire->ajoute(m_table.chercheAjoute(m_lecteur.getSymbole()));
        m_lecteur.avancer();
    }
    testerEtAvancer(")");
    testerEtAvancer(";");
    return instLire;
}

