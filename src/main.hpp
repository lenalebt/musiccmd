#ifndef MAIN_HPP
#define MAIN_HPP

#include <string>

/**
 * @mainpage music - a MUsic SImilarity Classifier
 * 
 * music classifies music, based on the files contents rather than their tag infos.
 */

/**
 * @brief our main program.
 */
int main(int argc, char *argv[]);

/**
 * @brief Call this function to start program tests
 * @param testname The name of the test
 * @return <code>EXIT_SUCCESS</code> if the test succeeded,
 *      <code>EXIT_FAILURE</code> otherwise.
 */
int runTest(std::string testname);

#endif //MAIN_HPP
