#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>

std::string toRelativePath(std::string absoluteFolder, const std::string& absoluteFile);

namespace tests
{
    int testToRelativePath();
}

#endif //FILESYSTEM_HPP
