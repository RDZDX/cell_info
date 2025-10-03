#include "console.h"
#include "Profont6x11.h"

#include <vmgraph.h>
#include <string.h>

const int char_width = 6;
const int char_height = 11;

static int scr_width = 0;
static int scr_height = 0;

static int c_w = 0; // simbolių vienoje eilutėje
static int c_x = 0; // simbolių skaičius dabartinėje eilutėje
static int c_y = 0; // dabartinė eilutė

static int max_lines = 0;
static int scroll_offset = 0;


static VMUINT16* scr_buf = 0;

int console_get_c_w() {
    return c_w;
}

void console_init(int _scr_w, int _scr_h, unsigned short* _scr_buf) {
    scr_width = _scr_w;
    scr_height = _scr_h;
    scr_buf = _scr_buf;
    c_w = scr_width / char_width;
    max_lines = scr_height / char_height;
    c_x = 0;
    c_y = 0;
    scroll_offset = 0;
}

static void scroll_down() {
    int line_size = scr_width * char_height;
    // stumiam viską ŽEMYN, kad atlaisvinti vietą viršuje
    memmove(scr_buf + line_size, scr_buf, (scr_width * scr_height - line_size) * 2);
    memset(scr_buf, 0, line_size * 2);
}

static void scroll_up() {
    int line_size = scr_width;  // viena pikselių eilutė
    int block_size = scr_width * char_height; // viena simbolių eilutė (11 px aukščio)
    int i;

    unsigned short* dst = scr_buf;
    unsigned short* src = scr_buf + block_size;

    // Perstumiam visą ekraną aukštyn po eilutę
    for (i = 0; i < (scr_height - char_height) * scr_width; i++) {
        *dst++ = *src++;
    }

    // Išvalom apačią
    memset(scr_buf + (scr_height - char_height) * scr_width, 0, block_size * 2);
}

static void draw_xy_char(int x, int y, char c) {
    const unsigned char* font_ch = ProFont6x11 + 5 + 12 * c + 1;
    const unsigned short textcolor = 0xFFFF, backcolor = 0;
    int i;
    int j;

    for (i = 0; i < char_height; ++i) {
        unsigned short* scr_buf_line = scr_buf + x + (y + i) * scr_width;
        for (j = 0; j < char_width; ++j)
            scr_buf_line[j] = ((((*font_ch) >> j) & 1) ? textcolor : backcolor);
        ++font_ch;
    }
}

static inline int real_y(int logical_line) {
    int idx = (scroll_offset + logical_line) % max_lines;
    return idx * char_height;
}

void console_put_char(char c) {
    if (c == '\n') {
        c_x = 0;
        c_y++;
        if ((c_y + 1) * char_height > scr_height) {
            scroll_up();
            c_y = (scr_height / char_height) - 1;
        }
        flush_layer();
        return;
    }

    if (c_x >= c_w) {
        c_x = 0;
        c_y++;
        if ((c_y + 1) * char_height > scr_height) {
            scroll_up();
            c_y = (scr_height / char_height) - 1;
        }
    }

    draw_xy_char(c_x * char_width, c_y * char_height, c);
    c_x++;
}

void console_put_str(const char* str) {
    while (*str) {
        console_put_char(*str);
        ++str;
    }
}

static char buf[1024];

#include <stdarg.h>
#include <vmstdlib.h>

int cprintf(char const* const format, ...) {
    va_list aptr;

    va_start(aptr, format);
    int ret = vm_vsprintf(buf, format, aptr);
    va_end(aptr);

    console_put_str(buf);
    return ret;
}
