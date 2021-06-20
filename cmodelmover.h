#ifndef CMODELMOVER_H
#define CMODELMOVER_H

#include "BotSocket/cabstractui.h"

class CModelMoverPrivate;
class MainWindow;

class CModelMover : public CAbstractUi
{
    friend class MainWindow;

public:
    CModelMover();
    ~CModelMover();

    BotSocket::TSocketState socketState() const;

    BotSocket::TDistance getTrX() const;
    BotSocket::TDistance getTrY() const;
    BotSocket::TDistance getTrZ() const;
    BotSocket::TDegree   getRX() const;
    BotSocket::TDegree   getRY() const;
    BotSocket::TDegree   getRZ() const;

    void setGui(MainWindow * const gui);

protected:
    void transformModel(const BotSocket::TDistance trX,
                        const BotSocket::TDistance trY,
                        const BotSocket::TDistance trZ,
                        const BotSocket::TDegree rX,
                        const BotSocket::TDegree rY,
                        const BotSocket::TDegree rZ) final;

    void socketStateChanged(const BotSocket::TSocketState state) final;

private:
    CModelMover(CModelMover &) = delete;
    CModelMover& operator=(const CModelMover &) = delete;

private:
    CModelMoverPrivate * const d_ptr;
};

#endif // CMODELMOVER_H
