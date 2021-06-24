#ifndef CBOTTASKDIALOGFACADE_H
#define CBOTTASKDIALOGFACADE_H

#include "../gui_types.h"

class QDialog;
class QWidget;
class gp_Pnt;
class CBotTaskDialogFacadePrivate;

class CBotTaskDialogFacade
{
public:
    CBotTaskDialogFacade(QWidget *parent, const GUI_TYPES::TBotTaskType taskType, const gp_Pnt &pos);
    ~CBotTaskDialogFacade();

    int exec();

    gp_Pnt getPos() const;
    gp_Pnt getAngles() const;

private:
    CBotTaskDialogFacadePrivate * const d_ptr;
};

#endif // CBOTTASKDIALOGFACADE_H
