#ifndef CDEPTHMAPVIEWPORT_H
#define CDEPTHMAPVIEWPORT_H

#include <QWidget>

class CDepthMapViewportPrivate;
class CInteractiveContext;

class CDepthMapViewport : public QWidget
{
public:
    explicit CDepthMapViewport(QWidget *parent = nullptr);
    ~CDepthMapViewport();

    void setContext(CInteractiveContext &context);
    void updatePosition();
    void createSnapshot(const char *fname, const size_t width, const size_t height);

    void setScale(const double scale);

protected:
    QPaintEngine* paintEngine() const final;
    void paintEvent(QPaintEvent *) final;
    void resizeEvent(QResizeEvent*) final;

private:
    CDepthMapViewportPrivate * const d_ptr;
};

#endif // CDEPTHMAPVIEWPORT_H
