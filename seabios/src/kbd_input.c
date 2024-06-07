#include "bregs.h"  // struct bregs
#include "stacks.h" // call16_int
#include "string.h" // memset
#include "util.h"

/****************************************************************
 * Keyboard calls
 ****************************************************************/

// See if a keystroke is pending in the keyboard buffer.
static int check_for_keystroke(void)
{
    struct bregs br;
    memset(&br, 0, sizeof(br));
    br.flags = F_IF|F_ZF;
    br.ah = 1;
    call16_int(0x16, &br);
    return !(br.flags & F_ZF);
}

// Return a keystroke - waiting forever if necessary.
static int get_raw_keystroke(void)
{
    struct bregs br;
    memset(&br, 0, sizeof(br));
    br.flags = F_IF;
    call16_int(0x16, &br);
    return br.ax;
}

// Read a keystroke - waiting up to 'msec' milliseconds.
// returns both scancode and ascii code.
int get_keystroke_full(int msec)
{
    u32 end = irqtimer_calc(msec);
    for (;;) {
        if (check_for_keystroke())
            return get_raw_keystroke();
        if (irqtimer_check(end))
            return -1;
        yield_toirq();
    }
}

// Read a keystroke - waiting up to 'msec' milliseconds.
// returns scancode only.
int get_keystroke(int msec)
{
    int keystroke = get_keystroke_full(msec);

    if (keystroke < 0)
        return keystroke;
    return keystroke >> 8;
}