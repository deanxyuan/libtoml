#include "src/common.h"
#include <string.h>

namespace TOML {

namespace internal {
/**
 *	Convert a UCS char to utf8 code, and return it in buf.
 *	Return #bytes used in buf to encode the char, or
 *	-1 on error.
 */
static int ucs_to_utf8(int64_t code, char buf[6]) {
    /* http://stackoverflow.com/questions/6240055/manually-converting-unicode-codepoints-into-utf-8-and-utf-16
     */
    /* The UCS code values 0xd800–0xdfff (UTF-16 surrogates) as well
     * as 0xfffe and 0xffff (UCS noncharacters) should not appear in
     * conforming UTF-8 streams.
     */
    if (0xd800 <= code && code <= 0xdfff) return -1;
    if (0xfffe <= code && code <= 0xffff) return -1;

    /* 0x00000000 - 0x0000007F:
       0xxxxxxx
    */
    if (code < 0) return -1;
    if (code <= 0x7F) {
        buf[0] = (unsigned char)code;
        return 1;
    }

    /* 0x00000080 - 0x000007FF:
       110xxxxx 10xxxxxx
    */
    if (code <= 0x000007FF) {
        buf[0] = (unsigned char)(0xc0 | (code >> 6));
        buf[1] = (unsigned char)(0x80 | (code & 0x3f));
        return 2;
    }

    /* 0x00000800 - 0x0000FFFF:
       1110xxxx 10xxxxxx 10xxxxxx
    */
    if (code <= 0x0000FFFF) {
        buf[0] = (unsigned char)(0xe0 | (code >> 12));
        buf[1] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
        buf[2] = (unsigned char)(0x80 | (code & 0x3f));
        return 3;
    }

    /* 0x00010000 - 0x001FFFFF:
       11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    */
    if (code <= 0x001FFFFF) {
        buf[0] = (unsigned char)(0xf0 | (code >> 18));
        buf[1] = (unsigned char)(0x80 | ((code >> 12) & 0x3f));
        buf[2] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
        buf[3] = (unsigned char)(0x80 | (code & 0x3f));
        return 4;
    }

    /* 0x00200000 - 0x03FFFFFF:
       111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    */
    if (code <= 0x03FFFFFF) {
        buf[0] = (unsigned char)(0xf8 | (code >> 24));
        buf[1] = (unsigned char)(0x80 | ((code >> 18) & 0x3f));
        buf[2] = (unsigned char)(0x80 | ((code >> 12) & 0x3f));
        buf[3] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
        buf[4] = (unsigned char)(0x80 | (code & 0x3f));
        return 5;
    }

    /* 0x04000000 - 0x7FFFFFFF:
       1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    */
    if (code <= 0x7FFFFFFF) {
        buf[0] = (unsigned char)(0xfc | (code >> 30));
        buf[1] = (unsigned char)(0x80 | ((code >> 24) & 0x3f));
        buf[2] = (unsigned char)(0x80 | ((code >> 18) & 0x3f));
        buf[3] = (unsigned char)(0x80 | ((code >> 12) & 0x3f));
        buf[4] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
        buf[5] = (unsigned char)(0x80 | (code & 0x3f));
        return 6;
    }

    return -1;
}
} // namespace internal
bool UCSToUTF8(int64_t code, std::string *output) {
    char buff[8] = {0};

    output->clear();

    int n = internal::ucs_to_utf8(code, buff);
    if (n != -1) {
        *output = std::string(buff, n);
        return true;
    }
    return false;
}

bool IsValidCharForString(bool multiline, uint8_t ch) {
    if ((0 <= ch && ch <= 0x08) || (0x0a <= ch && ch <= 0x1f) || (ch == 0x7f)) {
        if (!(multiline && (ch == '\r' || ch == '\n'))) {
            return false;
        }
    }
    return true;
}

bool IsByteExistsInTarget(const char *target, uint8_t cc) {
    for (; *target != '\0'; ++target) {
        if (cc == static_cast<uint8_t>(*target)) {
            return true;
        }
    }
    return false;
};

int FindNextChar(const uint8_t *buff, int ch, size_t count) {
    auto ptr = reinterpret_cast<const uint8_t *>(memchr(buff, ch, count));
    if (ptr == NULL) return -1;
    return static_cast<int>(ptr - buff);
}
} // namespace TOML
