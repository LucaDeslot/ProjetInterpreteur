#ifndef INTERPRETEUR_H
#define INTERPRETEUR_H

#include "Symbole.h"
#include "Lecteur.h"
#include "Exceptions.h"
#include "TableSymboles.h"
#include "ArbreAbstrait.h"

class Interpreteur {
public:
    Interpreteur(ifstream & fichier);   // Construit un interpréteur pour interpreter
                                        //  le programme dans  fichier

    void analyse();                     // Si le contenu du fichier est conforme à la grammaire,
                                        //   cette méthode se termine normalement et affiche un message "Syntaxe correcte".
                                        //   la table des symboles (ts) et l'arbre abstrait (arbre) auront été construits
                                        // Sinon, une exception sera levée
    void traduitCpp(unsigned int ind, ostream& out);

    inline TableSymboles const & getTable() const {
        return m_table;
    } // accesseur
    inline Noeud* getArbre() const {
        return m_arbre;
    }                    // accesseur

private:
    Lecteur m_lecteur;  // Le lecteur de symboles utilisé pour analyser le fichier
    TableSymboles m_table;    // La table des symboles valués
    Noeud* m_arbre;    // L'arbre abstrait

    // Implémentation de la grammaire
    Noeud* programme();   //   <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    Noeud* seqInst();	   //     <seqInst> ::= <inst> { <inst> }
    Noeud* inst();	       //        <inst> ::= <affectation> ; | <instSi>
    Noeud* affectation(); // <affectation> ::= <variable> = <expression>

    Noeud* expression();  // <terme> { + <terme> | - <terme> }
    Noeud* terme(); // <facteur> { * <facteur> | / <facteur> }
    Noeud* facteur(); // <entier> | <variable> | - <expBool> | non <expBool> | ( <expBool> )
    Noeud* expBool(); // <relationET> { ou <relationEt> }
    Noeud* relationEt(); // <relation> { et <relation> }
    Noeud* relation();  // <expression> { <opRel> <expression> }
    Noeud* opRel();  // == | != | < | <= | > | >=


    Noeud* instSi();      //      <instSi> ::= si ( <expression> ) <seqInst> finsi

    /**
     * <instTantQue> ::= tantque ( <expression> ) <seqInst> fintantque
     * @return the node of tantQue
     */
    Noeud* instTantQue();

    /**
     * <instRepeter> ::= repeter <seqInst> jusqua ( <expression> )
     * @return
     */

    Noeud* instRepeter();
    /**
     *<instPour> ::= pour ( [ <affectation> ] ; <expression> ; [ <affectation> ] ) <seqInst> finpour
     * @return
     */

    Noeud* instPour();
    /**
     * <instEcrire> ::= ecrire ( <expression> | <chaine> { , <expression> | <chaine> } )
     * @return
     */

    Noeud* instEcrire();
    /**
     * <instLire>::= lire ( <variable> { , <variable> } )
     *
     * @return
     */

    Noeud* instLire();

    // outils pour simplifier l'analyse syntaxique
    void tester(string const & symboleAttendu) const noexcept(false); // Si symbole courant != symboleAttendu, on lève une exception
    void testerEtAvancer(string const & symboleAttendu) noexcept(false); // Si symbole courant != symboleAttendu, on lève une exception, sinon on avance
    void erreur(string const & mess) const noexcept(false);
    // Lève une exception "contenant" le message mess

    bool isInst(Symbole symb) const;

};

#endif /* INTERPRETEUR_H */
