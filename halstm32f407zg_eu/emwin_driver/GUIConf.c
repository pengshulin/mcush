#include "GUI.h"

#ifdef GUI_ALLOC_SIZE
    #define GUI_BUF_LEN  GUI_ALLOC_SIZE
#else
    #define GUI_BUF_LEN  3*1024
#endif

char _buffer[GUI_BUF_LEN];

void GUI_X_Config(void)
{
    GUI_ALLOC_AssignMemory( (U32*)_buffer, GUI_BUF_LEN );
    GUI_SetDefaultFont(GUI_DEFAULT_FONT);
}
