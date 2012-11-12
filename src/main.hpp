#ifndef MAIN_HPP
#define MAIN_HPP

#include <string>

/// 
/// @mainpage music - a MUsic SImilarity Classifier
/// 
/// <code>musiccmd</code> is a frontend for <code>libmusic</code>; 
/// <code>libmusic</code> classifies music, based on the files contents rather than their tag infos.
/// 
/// @section mainpage_install Install
/// To install this package, you need to install some other packages:
///  * <code>libmusic</code> which you should get where you got this program
///  * <code>libmusicaccess</code> (dependency of <code>libmusic</code>, same source)
///  * <code>Eigen</code> >= 3.0 (can be obtained from http://eigen.tuxfamily.org, 3.0.6 is known to work)
///  * <code>Boost program_options</code> (see http://www.boost.org)
///  * <code>cmake</code> (see http://www.cmake.org)
///  * <code>doxygen</code> (optional, see http://www.doxygen.org)
/// 
/// @subsection mainpage_normal_install Installing on a normal *nix system
/// 
/// If you want to install the library in a normal, 
/// non-cross-compile-environment, just go to the "build/" directory and
/// type
/// 
/// @code{.sh}
/// cmake -DCMAKE_BUILD_TYPE=Release ..
/// make -j5
/// make install
/// @endcode
/// 
/// The last step might require root privileges. If you want to build 
/// the docs and have doxygen installed, type
/// 
/// @code{.sh}
/// make doc
/// @endcode
/// 
/// in the very same directory. You will find the docs in the api-doc/ 
/// directory. There are two formats available, for the more common HTML
/// version open the file <code>api-doc/html/index.html</code>. The
/// LaTeX version needs to be compiled, which is possible via
/// @code{.sh}
/// cd api-doc/latex/
/// pdflatex refman.tex
/// pdflatex refman.tex
/// @endcode
/// if you have LaTeX installed. Running LaTeX twice is necessary to get
/// cross-references and some citations right.
/// If you don't care about them, a single run is sufficient.
/// You may now open the file <code>refman.pdf</code> with a 
/// <a href="http://pdfreaders.org/">PDF reader of your choice</a>..
/// 
/// @subsection mainpage_crosscompilation_install Installing for Cross-Compilation
/// 
/// If you want to cross-compile and install the library, you need to 
/// build the library with a cmake toolchain file and afterwards copy 
/// the files manually to the right directories. This step highly 
/// depends on the environment you are using.
/// 
/// Example: Blackberry Playbook NDK 2.0.1
/// 
/// Assuming you installed the NDK in <code>/opt/bbndk-2.0.1/</code>, you need to:
/// - build the library:
///     * go to the <code>build/</code> directory. Type:
///         @code{.sh}
///         cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/modules/Toolchain-QNX-6.5.0-armv7.cmake ..
///         make -j4
///         @endcode
///         Sometimes, <code>cmake</code> is not able to find Boost properly. In this
///         case, you may help cmake by typing
///         @code{.sh}
///         cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/modules/Toolchain-QNX-6.5.0-armv7.cmake -DBOOST_LIBRARY_DIR=/opt/bbndk-2.0.1/target/qnx6/armle-v7/usr/lib/boost-1_48/ ..
///         make -j4
///         @endcode
/// 
/// If you have trouble building boost for the Blackberry Playbook, see
/// https://github.com/blackberry/Boost
///
/// @section mainpage_usage Usage
/// <code>music</code> is able to build user-specified categories of music. To
/// create an own category, you need to first add all your files to the database
/// of the program. After having done so, you can add and edit your categories.
/// Every category is built from positive and negative examples, you can have an
/// arbitrary number of examples for every category.
/// 
/// @subsection mainpage_usage_twolines Short introduction
/// This is a short three-line example of <code>musiccmd</code>:
/// @code
/// #add all files in /path/to/files to database (may take a long! time, approx. 2files/min), show progress
/// ./musiccmd -v -i --add-folder /path/to/files
/// 
/// #add new category "test"
/// ./musiccmd --add-category test
/// 
/// #add all files containing "myart" in the artist field (e.g. myartist)
/// #as positive example, all files containing "gold" in the title field as
/// #negative examples, recalculate category memberships, all for category "test".
/// 
/// ./musiccmd -v -i --edit-category test add-positive artist %myart% add-negative title %gold%
/// @endcode
/// 
/// 
/// @subsection mainpage_usage_short A little more explanation
/// To add your files to the database, there are two options: One for adding files,
/// and one for adding all files in a folder. Both options can handle wildcards,
/// multiple parameters, and multiple occurences:
/// @code{.sh}
/// #options for adding folders
/// #adds all files in this folder (no files from subfolders)
/// ./musiccmd --add-folder /path/to/folder/with/files
/// #adds all files in all first-level subfolders of the given path
/// ./musiccmd --add-folder /path/to/folder/with/subfolders/*
/// #adds all files in all first and second-level subfolders
/// ./musiccmd -A /path/* /path/*/*
///
/// #options for adding files
/// #add file /path/to/file.mp3
/// ./musiccmd --add-file /path/to/file.mp3
/// #add all mp3 files beginning with file
/// ./musiccmd --add-file /path/to/file*.mp3
/// #add all mp3 files beginning with file and all ending with end
/// ./musiccmd --add-file /path/to/file*.mp3 -a /path/to/*end.mp3
/// @endcode
/// @bug In this example, Doxygens code highlighting goes wrong.
/// 
/// <code>-A</code> is the short form for <code>--add-folder</code> and
/// <code>-a</code> is the short form for <code>--add-file</code>.
/// 
/// The option <code>--add-category</code> adds a new, empty category to the database.
/// Just call it with a name of the new category (don't forget to enclose the parameter
/// in \"s if it has multiple words, as in <code>--add-category "rock music"</code>!).
/// @bug Empty categories leave the database in a stale state for now. If
///     you want to add a new category, simply add files to it in the same statement:
///     <code>./musiccmd --add-category test --edit-category test add-positive album \%myalbum\%</code>.
/// 
/// Editing a category is easy, too:
/// @code{.sh}
/// ./musiccmd --edit-category test add-positive artist Nightwish
/// @endcode
/// adds all titles from the artist Nightwish to the category "test" as positive examples.
/// You can add and remove multiple recordings at once
/// from a category. A more powerful example:
/// @code{.sh}
/// ./musiccmd --edit-category test add-positive artist %ac_dc% filename %.ogg add-negative artist queen title %bicycle% add-positive queen title %stop_me_now% remove artist %nightwish% album %century_child%
/// @endcode
/// The statement adds all files from AC/DC as positive examples whose filenames
/// end with <code>.ogg</code>. This can be used to filter for file types, if you
/// need this. The statement is case-insensitive, and wildcards are allowed, so
/// \%ac_dc\% fits "AC/DC", "Ac/Dc", ";Ac dc...", but not "mAC and the heaDCones", since
/// _ is the wildcard character for a single character and \% is used for multiple characters.
/// You are allowed to use * and ?, too, but they can be expanded by the shell in
/// a way you do not like it.
/// 
/// The title "Bicycle Race" by Queen will be added as a negative example. There might be other songs
/// fitting the wildcards, but I am not aware of another song containing "bicycle" by Queen,
/// so it fits my needs. But the song "Don't stop me now" will be added as positive example.
/// Since I am not sure how the "don't" could be spelled in the title tag of
/// my files, I'll leave that out.
/// 
/// The titles by Nightwish on the album "Century Child" will be removed
/// from the list of examples, no matter if they
/// were positive or negative examples. You will not get errors if there
/// are no files fitting to your search string, or if they were not added
/// as examples at all.
/// 
/// You can show the best matches of your category with
/// @code{.sh}
/// ./musiccmd --show-category -i 2
/// @endcode
/// 
/// If you want more verbose output, you can add verbosity switches:
/// @code{.sh}
/// ./musiccmd -v 3 -i 2
/// @endcode
/// will raise the verbosity level to 3, and the database verbosity level to 2.
/// The database verbosity level is used when data from the database should be
/// displayed, such as the timbre model of a category (with database verbosity level 3
/// and option <code>--show-category name</code>).
/// 
/// 
/// @subsection mainpage_usage_help Getting help on possible command-line options
/// <code>musiccmd</code> is a command-line tool. It has built-in help functions, call
/// @code{.sh}
/// ./musiccmd --help all
/// @endcode
/// for a complete list of all command-line options.
/// If you leave out <code>all</code>,
/// you will get a condensed version of the help page.
/// 
/// 
/// 

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
