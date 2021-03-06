#ifndef ARBREABSTRAIT_H
#define ARBREABSTRAIT_H

// Contient toutes les déclarations de classes nécessaires
//  pour représenter l'arbre abstrait

#include <vector>
#include <iostream>
#include <iomanip>
#include "Symbole.h"
#include "Exceptions.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
class Noeud {
// Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
// Remarque : la classe ne contient aucun constructeur
public:
    virtual int executer() =0; // Méthode pure (non implémentée) qui rend la classe abstraite
    virtual void traduitCpp(unsigned int ind, ostream& out = cout) = 0;
    virtual void ajoute(Noeud* instruction) {
        throw OperationInterditeException();
    }
    virtual ~Noeud() {
    } // Présence d'un destructeur virtuel conseillée dans les classes abstraites
};

////////////////////////////////////////////////////////////////////////////////
class NoeudSeqInst: public Noeud {
// Classe pour représenter un noeud "sequence d'instruction"
//  qui a autant de fils que d'instructions dans la séquence
public:
    NoeudSeqInst();   // Construit une séquence d'instruction vide
    ~NoeudSeqInst() {
    } // A cause du destructeur virtuel de la classe Noeud
	int executer() override;    // Exécute chaque instruction de la séquence
    void traduitCpp(unsigned int ind, ostream& out = cout) override;
	void ajoute(Noeud* instruction) override;  // Ajoute une instruction à la séquence

private:
  vector<Noeud *> m_instructions; // pour stocker les instructions de la séquenceArbreAbstrait
};

////////////////////////////////////////////////////////////////////////////////
class NoeudAffectation: public Noeud {
// Classe pour représenter un noeud "affectation"
//  composé de 2 fils : la variable et l'expression qu'on lui affecte
public:
    NoeudAffectation(Noeud* variable, Noeud* expression); // construit une affectation
    ~NoeudAffectation() {
    } // A cause du destructeur virtuel de la classe Noeud
	int executer() override; // Exécute (évalue) l'expression et affecte sa valeur à la variable
    void traduitCpp(unsigned int ind, ostream& out = cout) override;
private:
    Noeud* m_variable;
    Noeud* m_expression;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudOperateurBinaire: public Noeud {
// Classe pour représenter un noeud "opération binaire" composé d'un opérateur
//  et de 2 fils : l'opérande gauche et l'opérande droit
public:
    NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit);
    // Construit une opération binaire : operandeGauche operateur OperandeDroit
    ~NoeudOperateurBinaire() {
    } // A cause du destructeur virtuel de la classe Noeud
	int executer() override;            // Exécute (évalue) l'opération binaire)
    void traduitCpp(unsigned int ind, ostream& out = cout) override;
private:
    Symbole m_operateur;
    Noeud* m_operandeGauche;
    Noeud* m_operandeDroit;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstSi: public Noeud {
// Classe pour représenter un noeud "instruction si"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
public:
  NoeudInstSi() = default;
    NoeudInstSi(Noeud* condition, Noeud* sequence);
    // Construit une "instruction si" avec sa condition et sa séquence d'instruction
    ~NoeudInstSi() {
    } // A cause du destructeur virtuel de la classe Noeud
    void ajoute(Noeud* instruction) override;
	int executer() override; // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduitCpp(unsigned int ind, ostream& out = cout) override;
private:
    /**
     * all conditions of if/elseif : match with the seqInstructions
     * @note conditions.at(i) match with seqInstructions.at(i)
     * @details if the size of seqInstructions is bigger by 1 than the conditions size the last seqInstruction is for else
     */
    vector<Noeud*> conditions;

    /** \ref conditions
     * all conditions of if/elseif : match with the conditions
     * @note conditions.at(i) match with seqInstructions.at(i)
     * @details if the size of seqInstructions is bigger by 1 than the conditions size the last seqInstruction is for else
     */
    vector<Noeud*> seqIntructions;
};
////////////////////////////////////////////////////////////////////////////////
class NoeudInstTantQue: public Noeud {
public:
    NoeudInstTantQue(Noeud* condition, Noeud* seqInst);
    ~NoeudInstTantQue() {
    }
    virtual int executer() override;
    virtual void traduitCpp(unsigned int ind, ostream& out = cout) override;
protected:
    Noeud* condition;
    Noeud* seqInst;
};
////////////////////////////////////////////////////////////////////////////////
class NoeudInstRepeter: public NoeudInstTantQue {
public:
    NoeudInstRepeter(Noeud* seqInst, Noeud* condition);
    ~NoeudInstRepeter() {
    }
    int executer() override;
    void traduitCpp(unsigned int ind, ostream& out = cout) override;
private:
};
////////////////////////////////////////////////////////////////////////////////
class NoeudInstPour: public NoeudInstTantQue {
public:
    NoeudInstPour(Noeud* condition, Noeud* seqInst, Noeud* affect1 = nullptr, Noeud* affect2 =
            nullptr);
    ~NoeudInstPour() {
    }
    int executer() override;
    void traduitCpp(unsigned int ind, ostream& out = cout) override;
private:
    Noeud* affect1;
    Noeud* affect2;
};
////////////////////////////////////////////////////////////////////////////////
class NoeudInstEcrire: public Noeud {
public:
    NoeudInstEcrire(Noeud* exp);
    ~NoeudInstEcrire() {
    }
    int executer() override;
    void traduitCpp(unsigned int ind, ostream& out = cout) override;
    void ajoute(Noeud* instruction) override;
private:
    vector<Noeud*> exps;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstLire: public Noeud {
public:
    NoeudInstLire(Noeud* var);
    ~NoeudInstLire() {
    }
    int executer() override;
    void traduitCpp(unsigned int ind, ostream& out = cout) override;
    void ajoute(Noeud* var) override;
private:
    vector<Noeud*> vars;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstSelon : public NoeudInstSi
{
  // on pourrait redefinir le traduitCpp pour faire un case et pas un if
};
#endif /* ARBREABSTRAIT_H */
