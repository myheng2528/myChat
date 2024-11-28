#include "global.h"
void repolish(QWidget *w)
{
    w->style()->unpolish(w);
    w->style()->polish(w);
}
