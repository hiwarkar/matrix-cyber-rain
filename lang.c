#include <stdlib.h>
#include <time.h>
#include "lang.h"

static int g_lang = 0;
static const char *china_chars = "文中汉字测试语句你我他乐";

void lang_init() { srand(time(NULL)+4321); }
void lang_toggle() { g_lang = !g_lang; }
int lang_enabled() { return g_lang; }

char lang_get_char() {
    if (!g_lang) {
        return 33 + rand()%94;
    }
    /* Simplified: return ASCII approximation from a small set when Chinese mode on.
       Full Unicode CJK support would require wide-char handling; this is a placeholder. */
    const char *set = "文中汉字测试";
    return set[rand() % 6];
}
