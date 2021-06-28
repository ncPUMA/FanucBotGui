#include "cbottaskdialogfacade.h"

#include <functional>
#include <cassert>
#include <map>

#include "../gui_types.h"

#include "cmovebottaskdialog.h"
#include "cdrillbottaskdialog.h"
#include "cmarkbottaskdialog.h"

//DialogContainer
class CBotTaskDialogFacadePrivate
{
    template <typename T>
    friend class CDialogContainer;

public:
    virtual ~CBotTaskDialogFacadePrivate() { }

    virtual int exec() = 0;
    virtual GUI_TYPES::STaskPoint getTaskPoint() const = 0;

private:
    CBotTaskDialogFacadePrivate() { }
};


template <typename T>
class CDialogContainer : public CBotTaskDialogFacadePrivate
{
public:
    CDialogContainer(T *instance) : dialog(instance) { }
    ~CDialogContainer() { delete dialog; }

    int exec() { return dialog->exec(); }

    GUI_TYPES::STaskPoint getTaskPoint() const { return dialog->getTaskPoint(); }

private:
    T * const dialog;
};


//Dialog Factory method
typedef std::function <CBotTaskDialogFacadePrivate* (QWidget *, GUI_TYPES::STaskPoint)> TDialogConstructor;

template <typename TDialog>
inline static std::pair <GUI_TYPES::TBotTaskType, TDialogConstructor > createDialogPair(GUI_TYPES::TBotTaskType taskType) {
    return std::make_pair(taskType, [](QWidget *parent, const GUI_TYPES::STaskPoint &initData) {
        return new CDialogContainer <TDialog>  (new TDialog(parent, initData));
    });
}

template <GUI_TYPES::TBotTaskType... TaskTypes> struct zip {
    template <typename... TDialogs> struct with {

        CBotTaskDialogFacadePrivate *createDialog(QWidget *parent, const GUI_TYPES::STaskPoint &initData) const {
            auto it = map.find(initData.taskType);
            assert(it != map.cend());
            return it->second(parent, initData);
        }

        const std::map <GUI_TYPES::TBotTaskType, TDialogConstructor> map = {
            createDialogPair <TDialogs> (TaskTypes)...
        };
    };
};
static const zip  <GUI_TYPES::ENBTT_MOVE, GUI_TYPES::ENBTT_DRILL, GUI_TYPES::ENBTT_MARK>::
             with <   CMoveBotTaskDialog,    CDrillBotTaskDialog,    CMarkBotTaskDialog>
dialogFactoryMethod;


//Facade
CBotTaskDialogFacade::CBotTaskDialogFacade(QWidget *parent, const GUI_TYPES::STaskPoint &initData) :
    d_ptr(dialogFactoryMethod.createDialog(parent, initData))
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

GUI_TYPES::STaskPoint CBotTaskDialogFacade::getTaskPoint() const
{
    return d_ptr->getTaskPoint();
}
