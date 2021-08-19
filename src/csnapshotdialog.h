#ifndef CSNAPSHOTDIALOG_H
#define CSNAPSHOTDIALOG_H

#include <QDialog>

class CInteractiveContext;

namespace Ui {
class CSnapshotDialog;
}

class CSnapshotDialogPrivate;

class CSnapshotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CSnapshotDialog(QWidget * const parent = nullptr);
    ~CSnapshotDialog();

    void setContext(CInteractiveContext &context);
    void setFileName(const char *fname);
    void setScale(const double scale);
    double getScale() const;

    Q_INVOKABLE void makeSnapshot();

public slots:
    void slSnapshot();

private:
    Ui::CSnapshotDialog *ui;
    CSnapshotDialogPrivate * const d_ptr;
};

#endif // CSNAPSHOTDIALOG_H
