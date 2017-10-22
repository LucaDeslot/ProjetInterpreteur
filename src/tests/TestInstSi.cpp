/**
 * TestInstSi.cpp
 *
 * Early optimization is the root of all evil
 *
 *  Created on: 22 oct. 2017
 *      Author: nihil
 */

#include "TestInstSi.hpp"

#include <cppunit/extensions/AutoRegisterSuite.h>
#include <fstream>
#include <string>

#include "../Exceptions.h"
#include "../Interpreteur.h"
#include "../SymboleValue.h"
#include "../TableSymboles.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TestInstSi);

TestInstSi::TestInstSi() :
        fileOk("testSi.txt"), fileError("testSiError.txt") {
}

TestInstSi::~TestInstSi() {
}

void TestInstSi::setUp() {
    // To take the file motsCles.txt to perform correctly the execution
    fstream motCDist("../motsCles.txt", ios::in);
    fstream motC("motsCles.txt");
    if (!motCDist.good() && !motC.good()) {
        throw FichierException("motsCles.txt");
    } else if (motCDist.good()) {
        motC.open("motsCles.txt", ios::out);
        motC << motCDist.rdbuf();
    }
    motC.close();
    motCDist.close();
    //-----------------------
}

void TestInstSi::tearDown() {
}

void TestInstSi::testSyntaxCorrect() {
    int const IVAL = 12;
    int const JVAL = 2;
    string const KVAL = "\"testOk\"";

    ifstream file(fileOk);
    CPPUNIT_ASSERT_NO_THROW_MESSAGE("Instantiation de l'interpreteur ",
            Interpreteur interpreteur(file));
    file.close();
    file.open(fileOk);
    Interpreteur interpreteurEffect(file);

    //------------------------------------------------------
    // ------------------Analyse et execution
    CPPUNIT_ASSERT_NO_THROW_MESSAGE("Analyse de la syntaxe", interpreteurEffect.analyse());

    CPPUNIT_ASSERT_NO_THROW_MESSAGE("Execution de l'arbre",
            interpreteurEffect.getArbre()->executer());

    //------------------------------------------------------
    // ------------------Resultats
    TableSymboles tb = const_cast<TableSymboles&>(interpreteurEffect.getTable());
    SymboleValue* symb = nullptr;

    //------------------------------------------------------
    // ------------------variable i
    symb = tb.cherche(Symbole(to_string(IVAL)));
    CPPUNIT_ASSERT_MESSAGE("IVAL existe ?", symb);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("i==12 ?", IVAL, symb->executer());

    //------------------------------------------------------
    // ------------------si avec variable j
    symb = tb.cherche(Symbole(to_string(JVAL)));
    CPPUNIT_ASSERT_MESSAGE("JVAL existe ?", symb);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("j==2 ?", JVAL, symb->executer());

    //------------------------------------------------------
    // ------------------si avec variable k (string)
    symb = tb.cherche(Symbole(KVAL));
    CPPUNIT_ASSERT_MESSAGE("KVAL existe ?", symb);

    CPPUNIT_ASSERT_ASSERTION_PASS_MESSAGE("k==testOk ?", KVAL == symb->getString());

    file.close();
}

void TestInstSi::testSyntaxIncorrect() {
    ifstream file(fileError);
    CPPUNIT_ASSERT_NO_THROW_MESSAGE("Instantiation de l'interpreteur ",
            Interpreteur interpreteur(file));
    file.close();
    file.open(fileError);
    Interpreteur interpreteurEffect(file);

    //------------------------------------------------------
    // ------------------Analyse et execution
    try {
        interpreteurEffect.analyse();
    } catch (exception e) {
    }
    CPPUNIT_ASSERT_MESSAGE("Analyse de la syntaxe erreur",
            interpreteurEffect.getArbre() == nullptr);
}
