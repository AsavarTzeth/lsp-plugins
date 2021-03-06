/*
 * tk.cpp
 *
 *  Created on: 19 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        static const char *color_names[] =
        {
            "background",
            "hole",
            "glass",

            "red",
            "green",
            "blue",
            "yellow",
            "cyan",
            "magenta",
            "black",
            "white",

            "label_text",
            "button_face",
            "button_text",

            "knob_cap",
            "knob_scale",

            "logo_face",
            "logo_text",

            "graph_axis",
            "graph_mesh",
            "graph_marker",
            "graph_text",
            "graph_line",
            "left_channel",
            "right_channel",
            "middle_channel",
            "side_channel",

            "hlink_text",
            "hlink_hover",

            "status_ok",
            "status_warn",
            "status_error",

            NULL
        };

        color_t color_id(const char *name)
        {
            size_t id = 0;
            for (const char **c = color_names; *c != NULL; ++c, ++id)
                if (!strcmp(*c, name))
                    return color_t(id);
            return C_UNKNOWN;
        }

        const char *color_name(color_t color)
        {
            for (ssize_t id=0; id <= color; ++id)
            {
                if (color_names[id] == NULL)
                    return NULL;
            }

            return color_names[color];
        }
    }
}


