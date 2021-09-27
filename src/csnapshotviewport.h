#ifndef CSNAPSHOTVIEWPORT_H
#define CSNAPSHOTVIEWPORT_H

#include <QWidget>

class CSnapshotViewportPrivate;
class CInteractiveContext;
class QDoubleSpinBox;

class CSnapshotViewport : public QWidget
{
    Q_OBJECT

public:
    explicit CSnapshotViewport(QWidget *parent = nullptr);
    ~CSnapshotViewport();

    void setContext(CInteractiveContext &context);
    void setScaleWidget(QDoubleSpinBox &box);
    void updatePosition();
    void createSnapshot(const char *fname, const size_t width, const size_t height);

    void setScale(const double scale);
    double getScale() const;

protected:
    QPaintEngine* paintEngine() const final;
    void paintEvent(QPaintEvent *) final;
    void resizeEvent(QResizeEvent*) final;

//    void mousePressEvent(QMouseEvent *event) final;
//    void mouseReleaseEvent(QMouseEvent *event) final;
//    void mouseMoveEvent(QMouseEvent *event) final;
    void wheelEvent(QWheelEvent *event) final;

private slots:
    void slSpinChanged(double value);

private:
    CSnapshotViewportPrivate * const d_ptr;
};

#endif // CSNAPSHOTVIEWPORT_H
