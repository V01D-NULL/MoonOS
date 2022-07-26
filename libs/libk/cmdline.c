#include "cmdline.h"
#include "kstring.h"
#include <devices/serial/serial.h>

bool boot_cmdline_find_tag(string_view tag, string_view cmdline)
{
    size_t len = strlen(cmdline);
    if (!tag || !cmdline)
        return false;

    // Todo: Copy tag into here
    size_t tag_len = strlen(tag);
    char *unmodified_tag = (char *)tag;
    char buff[tag_len];
    int buff_i = 0;

    for (size_t i = 0; i < len; i++, *cmdline++, *tag++)
    {
        if (*cmdline == ',')
        {
            if (strncmp(buff, unmodified_tag, buff_i) == 0)
            {
                return true;
            }

            memset((char *)buff, 0, tag_len);
            buff_i = 0;

            (void)*cmdline++;
        }

        buff[buff_i++] = *cmdline;
    }

    return !strncmp(buff, unmodified_tag, tag_len);
}