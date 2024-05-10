#include "utils.h"

namespace ST {
    string operator+(const string& str, int num) {
        int digits = 1;
        int numcopy = num;
        while(numcopy / 10 > 0) {
            numcopy /= 10;
            digits++;
        }

        char* charnum = new char[digits];

        for(int i = 0; i < digits; i++) {
            charnum[digits - i - 1] = '0' + (num % 10);
            num /= 10;
        }

        ST::char_buffer cat;
        cat.allocate(str.size() + digits);
        std::char_traits<char>::copy(&cat[0], str.c_str(), str.size());
        std::char_traits<char>::copy(&cat[str.size()], charnum, digits);

        delete[] charnum;
        return string::from_validated(std::move(cat));
    }
}

namespace bakermaker {

}