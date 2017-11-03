/**
 * TestInstSi.hpp
 *
 * Early optimization is the root of all evil
 *
 *  Created on: 22 oct. 2017
 *      Author: nihil
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Portability.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>

class TestInstSi : public CPPUNIT_NS::TestFixture
{
// Utilisation de MACROS pour définir en quoi consiste la suite de tests
CPPUNIT_TEST_SUITE (TestInstSi); // déclaration d'une suite de tests
  CPPUNIT_TEST(testSyntaxCorrect); // ajout d'un test à la suite
  CPPUNIT_TEST(testSyntaxIncorrect); // ajout d'un test à la suite
  CPPUNIT_TEST_SUITE_END()
  ; // fin de la suite de tests
public:
  TestInstSi();
  virtual ~TestInstSi();
  void setUp(); // utilisé pour initialiser des données communes aux tests
  void tearDown(); // utilisé pour "nettoyer" les données communes
private:
  std::string fileOk;
  std::string fileError;

  void testSyntaxCorrect();

  void testSyntaxIncorrect();
};
