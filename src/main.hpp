#ifndef MAIN_HPP
#define MAIN_HPP

/**
 * @mainpage music - a MUsic SImilarity Classifier
 * 
 * music classifies music, based on the files contents rather than their tag infos.
 */

/**
 * @brief Parses the program options from the command line.
 * 
 * @param argc The argument count
 * @param argv The argument values
 * 
 * @return <code>0</code> if everything went fine, <code>ERROR_CODE</code> to return from
 *      <code>int main()</code> otherwise.
 */
int parseProgramOptions(int argc, char* argv[]);

/**
 * @brief our main program.
 */
int main(int argc, char *argv[]);


#endif //MAIN_HPP
