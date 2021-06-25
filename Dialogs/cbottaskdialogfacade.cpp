#include "cbottaskdialogfacade.h"

#include <gp_Pnt.hxx>

#include "cmovebottaskdialog.h"
#include "cdrillbottaskdialog.h"
#include "cmarkbottaskdialog.h"

class CBotTaskDialogFacadePrivate
{
public:
    CBotTaskDialogFacadePrivate() { }
    virtual ~CBotTaskDialogFacadePrivate() { }

    virtual int exec() = 0;

    virtual gp_Pnt getPos() const = 0;
    virtual gp_Pnt getAngles() const = 0;
};

template <typename T>
class CDialogContainer : public CBotTaskDialogFacadePrivate
{
    friend CBotTaskDialogFacadePrivate* create(QWidget *parent, const GUI_TYPES::TBotTaskType taskType, const gp_Pnt &pos);

    CDialogContainer(T *instance) : dialog(instance) { }
    ~CDialogContainer() { delete dialog; }

    int exec() { return dialog->exec(); }

    gp_Pnt getPos() const { return dialog->getPos(); }
    gp_Pnt getAngles() const { return dialog->getAngles(); }

    T *dialog;
};

inline static CBotTaskDialogFacadePrivate* create(QWidget *parent, const GUI_TYPES::TBotTaskType taskType, const gp_Pnt &pos)
{
    CBotTaskDialogFacadePrivate *result = nullptr;
    switch(taskType)
    {
        using namespace GUI_TYPES;

        case ENBTT_MOVE : result = new CDialogContainer <CMoveBotTaskDialog>  (new CMoveBotTaskDialog (parent, pos)); break;
        case ENBTT_DRILL: result = new CDialogContainer <CDrillBotTaskDialog> (new CDrillBotTaskDialog(parent, pos)); break;
        case ENBTT_MARK : result = new CDialogContainer <CMarkBotTaskDialog>  (new CMarkBotTaskDialog (parent, pos)); break;
    }
    assert(result != nullptr);
    return result;
}

CBotTaskDialogFacade::CBotTaskDialogFacade(QWidget *parent, const GUI_TYPES::TBotTaskType taskType, const gp_Pnt &pos) :
    d_ptr(create(parent, taskType, pos))
{

}

CBotTaskDialogFacade::~CBotTaskDialogFacade()
{
    delete d_ptr;
}

int CBotTaskDialogFacade::exec()
{
    return d_ptr->exec();
}

gp_Pnt CBotTaskDialogFacade::getPos() const
{
    return d_ptr->getPos();
}

gp_Pnt CBotTaskDialogFacade::getAngles() const
{
    return d_ptr->getAngles();
}
