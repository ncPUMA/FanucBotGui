#include "csnapshotdialog.h"
#include "ui_csnapshotdialog.h"

#include <QFileDialog>

class CSnapshotDialogPrivate
{
    friend class CSnapshotDialog;

    std::string fName;
};



CSnapshotDialog::CSnapshotDialog(QWidget * const parent) :
    QDialog(parent),
    ui(new Ui::CSnapshotDialog),
    d_ptr(new CSnapshotDialogPrivate())
{
    ui->setupUi(this);
    ui->viewport->setScaleWidget(*ui->dsbScale);
    connect(ui->pbSnapshot, &QPushButton::clicked, this, &CSnapshotDialog::slSnapshot);
    connect(ui->pbClose   , &QPushButton::clicked, this, &CSnapshotDialog::reject);
}

CSnapshotDialog::~CSnapshotDialog()
{
    delete d_ptr;
    delete ui;
}

void CSnapshotDialog::setContext(CInteractiveContext &context)
{
    ui->viewport->setContext(context);
}

void CSnapshotDialog::setFileName(const char *fname)
{
    d_ptr->fName = fname;
}

void CSnapshotDialog::init(const double scale, const size_t width, const size_t height)
{
    ui->viewport->setScale(scale);
    ui->sbWidth->setValue(static_cast <int> (width));
    ui->sbHeight->setValue(static_cast <int> (height));
}

double CSnapshotDialog::getScale() const
{
    return ui->viewport->getScale();
}

size_t CSnapshotDialog::getWidth() const
{
    return static_cast <size_t> (ui->sbWidth->value());
}

size_t CSnapshotDialog::getHeight() const
{
    return static_cast <size_t> (ui->sbHeight->value());
}

void CSnapshotDialog::makeSnapshot()
{
    ui->viewport->createSnapshot(d_ptr->fName.c_str(), getWidth(), getHeight());
}

void CSnapshotDialog::slSnapshot()
{
    QString fName = QString::fromStdString(d_ptr->fName);
    fName = QFileDialog::getSaveFileName(this,
                                         "Имя файла для сохранения",
                                         QString(),
                                         tr("BMP (*.bmp)"));
    if (!fName.isEmpty())
    {
        if (!fName.endsWith(".bmp", Qt::CaseInsensitive))
            fName.append(".bmp");
        d_ptr->fName = fName.toStdString();
        makeSnapshot();
    }
}
