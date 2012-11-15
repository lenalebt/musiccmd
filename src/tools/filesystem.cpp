#include "filesystem.hpp"
#include "debug.hpp"
#include "testframework.hpp"
#include <algorithm>

std::string toRelativePath(std::string absoluteFolder, const std::string& absoluteFile)
{
    if (absoluteFolder[absoluteFolder.size()-1] != '/')
        absoluteFolder += "/";
    
    unsigned int minLength = std::min(absoluteFolder.size(), absoluteFile.size());
    int endOfCommonFolderPath=-1;
    for (unsigned int i=0; i<minLength; i++)
    {
        if (absoluteFolder[i] != absoluteFile[i])
            break;
        
        if (absoluteFolder[i] == '/')
            endOfCommonFolderPath = i;
    }
    
    std::string relativePath;
    //set relative path to position of last slash plus one character
    if (endOfCommonFolderPath != -1)
        relativePath = absoluteFile.substr(endOfCommonFolderPath+1, std::string::npos);
    else
        relativePath = absoluteFile;
    
    for (unsigned int i=endOfCommonFolderPath + 1; i < absoluteFolder.size(); i++)
    {
        if (absoluteFolder[i] == '/')
            relativePath = "../" + relativePath;
    }
    
    return relativePath;
}

namespace tests
{
    int testToRelativePath()
    {
        CHECK_EQ(toRelativePath("/home/test/", "/home/test/a.pdf"), "a.pdf");
        CHECK_EQ(toRelativePath("/home/test/lala", "/home/test/a.pdf"), "../a.pdf");
        CHECK_EQ(toRelativePath("/home/test/lala/lala/lalala", "/home/test/a.pdf"), "../../../a.pdf");
        CHECK_EQ(toRelativePath("/home/", "/home/test/a.pdf"), "test/a.pdf");
        CHECK_EQ(toRelativePath("/", "/home/test/a.pdf"), "home/test/a.pdf");
        CHECK_EQ(toRelativePath("/", "/home/test/"), "home/test/");
        CHECK_EQ(toRelativePath("/", "/home/test"), "home/test");
        CHECK_EQ(toRelativePath("/homer/", "/home/test"), "../home/test");
        CHECK_EQ(toRelativePath("/homer", "/home/test"), "../home/test");
        
        return EXIT_SUCCESS;
    }
}
