#include "tests.hpp"
#include "testframework.hpp"
#include "stringhelper.hpp"

namespace tests
{
    int testStringHelper()
    {
        CHECK(endsWith("hallo", "o"));
        CHECK(endsWith("hallo", "lo"));
        CHECK(endsWith("hallo", "llo"));
        CHECK(endsWith("hallo", "allo"));
        CHECK(endsWith("hallo", "hallo"));
        CHECK(!endsWith("hallo", "o!"));
        CHECK(endsWith("hallo!", "o!"));
        CHECK(!endsWith("hallo", "ollah"));
        CHECK(!endsWith("hallo", "%"));
        CHECK(!endsWith("hallo", "&"));
        CHECK(!endsWith("hallo", "§"));
        CHECK(!endsWith("hallo", "\""));
        CHECK(!endsWith("hallo", "?"));
        CHECK(!endsWith("hallo", "#"));
        CHECK(!endsWith("hallo", "'"));
        
        std::string str("hAllO'!§$%&/()=?");
        CHECK_EQ(str, "hAllO'!§$%&/()=?");
        toupper(str);
        CHECK_EQ(str, "HALLO'!§$%&/()=?");
        toupper(str);
        CHECK_EQ(str, "HALLO'!§$%&/()=?");
        tolower(str);
        CHECK_EQ(str, "hallo'!§$%&/()=?");
        tolower(str);
        CHECK_EQ(str, "hallo'!§$%&/()=?");
        
        str = ".DS_Store\000\006x`\000";
        CHECK_EQ(str, ".DS_Store\000\006x`\003\000");
        tolower(str);
        CHECK_EQ(str, ".ds_store\000\006x`\003\000");
        
        str = "%";
        CHECK_EQ(str, "%");
        toupper(str);
        CHECK_EQ(str, "%");
        tolower(str);
        CHECK_EQ(str, "%");
        
        return EXIT_SUCCESS;
    }
}
