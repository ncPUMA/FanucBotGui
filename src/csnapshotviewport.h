#ifndef CSNAPSHOTVIEWPORT_H
#define CSNAPSHOTVIEWPORT_H

#include <QWidget>

namespace Ui {
class CSnapshotViewport;
}

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
    void createSnapshot(const char *fname);

    void setScale(const double scale);
    double getScale() const;

protected:
    QPaintEngine* paintEngine() const final;
    void paintEvent(QPaintEvent *) final;
    void resizeEvent(QResizeEvent*) final;

    void mousePressEvent(QMouseEvent *event) final;
    void mouseReleaseEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;
    void wheelEvent(QWheelEvent *event) final;

private slots:
    void slSpinChanged(double value);

private:
    Ui::CSnapshotViewport *ui;
    CSnapshotViewportPrivate * const d_ptr;
};

#endif // CSNAPSHOTVIEWPORT_H
