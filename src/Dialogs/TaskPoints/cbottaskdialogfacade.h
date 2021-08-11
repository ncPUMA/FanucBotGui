#ifndef CBOTTASKDIALOGFACADE_H
#define CBOTTASKDIALOGFACADE_H

namespace GUI_TYPES {
struct STaskPoint;
}

class QDialog;
class QWidget;
class CBotTaskDialogFacadePrivate;

class CBotTaskDialogFacade
{
public:
    CBotTaskDialogFacade(QWidget *parent, const GUI_TYPES::STaskPoint &initData);
    ~CBotTaskDialogFacade();

    int exec();

    GUI_TYPES::STaskPoint getTaskPoint() const;

private:
    CBotTaskDialogFacadePrivate * const d_ptr;
};

#endif // CBOTTASKDIALOGFACADE_H
