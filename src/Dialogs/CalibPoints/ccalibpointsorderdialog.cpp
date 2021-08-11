#include "ccalibpointsorderdialog.h"
#include "ui_ccalibpointsorderdialog.h"

#include <QAbstractTableModel>
#include <QProxyStyle>
#include <QMimeData>
#include <QPainter>
#include <QStyledItemDelegate>

#include "caddcalibpointdialog.h"

namespace {

class CCalibPointsOrderModel : public QAbstractTableModel
{
private:
    enum EN_Columns
    {
        ENC_FIRST = 0,

        ENC_NAME = ENC_FIRST,
        ENC_GLOBAL_POS,
        ENC_BOT_POS,

        ENC_LAST
    };

public:
    CCalibPointsOrderModel(QObject *parent = nullptr) :
        QAbstractTableModel(parent) { }

    void setCalibPoints(const std::vector<GUI_TYPES::SCalibPoint> &calibPnts) {
        beginResetModel();
        points.clear();
        size_t cntr = 0;
        for(const auto &scp : calibPnts) {
            SPoint pnt;
            pnt.name = tr("C%1").arg(++cntr);
            pnt.pnt = scp;
            points.push_back(pnt);
        }
        endResetModel();
    }

    std::vector<GUI_TYPES::SCalibPoint> getCalibPoints() const {
        std::vector<GUI_TYPES::SCalibPoint> result;
        for(const auto &pnt : points)
            result.push_back(pnt.pnt);
        return result;
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const final {
        (void)parent;
        return ENC_LAST;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const final {
        (void)parent;
        return static_cast <int> (points.size());
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final {
        QVariant result;
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            switch(section) {
                case ENC_NAME      : result = tr("Название")            ; break;
                case ENC_GLOBAL_POS: result = tr("Координаты заготовки"); break;
                case ENC_BOT_POS   : result = tr("Координаты робота")   ; break;
                default: break;
            }
        }
        else
            result = QAbstractTableModel::headerData(section, orientation, role);
        return result;
    }

    QString displayRoleData(const QModelIndex &index) const {
        QString result;
        if (index.row() >= 0 && index.row() < rowCount()) {
            const size_t row = static_cast <size_t> (index.row());
            switch(index.column()) {
                case ENC_NAME:
                    result = points[row].name;
                    break;
                case ENC_GLOBAL_POS: {
                    const GUI_TYPES::SVertex &vertex = points[row].pnt.globalPos;
                    result = tr("X:%1 Y:%2 Z:%3")
                            .arg(vertex.x, 12, 'f', 6, QChar('0'))
                            .arg(vertex.y, 12, 'f', 6, QChar('0'))
                            .arg(vertex.z, 12, 'f', 6, QChar('0'));
                    break;
                }
                case ENC_BOT_POS: {
                    const GUI_TYPES::SVertex &vertex = points[row].pnt.botPos;
                    result = tr("X:%1 Y:%2 Z:%3")
                            .arg(vertex.x, 12, 'f', 6, QChar('0'))
                            .arg(vertex.y, 12, 'f', 6, QChar('0'))
                            .arg(vertex.z, 12, 'f', 6, QChar('0'));
                    break;
                }
                default:
                    break;
            }
        }
        return result;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const final {
        QVariant result;
        switch(role) {
            case Qt::DisplayRole: result = displayRoleData(index); break;
            default: break;
        }
        return result;
    }

    Qt::ItemFlags flags(const QModelIndex &) const final {
        const Qt::ItemFlags result =
                Qt::ItemIsSelectable |
                Qt::ItemIsEditable |
                Qt::ItemIsDragEnabled |
                Qt::ItemIsDropEnabled |
                Qt::ItemIsEnabled |
                Qt::ItemNeverHasChildren;
        return result;
    }

    Qt::DropActions supportedDragActions() const final {
        return Qt::MoveAction;
    }

    Qt::DropActions supportedDropActions() const final {
        return Qt::MoveAction;
    }

    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent) final {
        (void)row;
        (void)column;
        // check if the action is supported
        if (!data || !(action == Qt::CopyAction || action == Qt::MoveAction))
            return false;
        // check if the format is supported
        QStringList types = mimeTypes();
        if (types.isEmpty())
            return false;
        QString format = types.at(0);
        if (!data->hasFormat(format))
            return false;
        // decode and insert
        QByteArray encoded = data->data(format);
        QDataStream stream(&encoded, QIODevice::ReadOnly);
        int top = INT_MAX;
        int left = INT_MAX;
        int bottom = 0;
        int right = 0;
        QVector<int> rows, columns;
        QVector<QMap<int, QVariant> > dt;
        while (!stream.atEnd()) {
            int r, c;
            QMap<int, QVariant> v;
            stream >> r >> c >> v;
            rows.append(r);
            columns.append(c);
            dt.append(v);
            top = qMin(r, top);
            left = qMin(c, left);
            bottom = qMax(r, bottom);
            right = qMax(c, right);
        }

        int dstRow = rowCount();
        if (parent.isValid())
            dstRow = parent.row();
        return moveRow(QModelIndex(), top, QModelIndex(), dstRow);
    }

    bool moveRows(const QModelIndex &, int sourceRow, int count,
                  const QModelIndex &, int destinationChild) final {
        (void)count;
        const int rCount = rowCount();
        if (sourceRow < 0 ||
            sourceRow >= rCount ||
            destinationChild > rCount ||
            destinationChild < 0 ||
            sourceRow == destinationChild - 1)
            return false;
        beginMoveRows(QModelIndex(), sourceRow, sourceRow, QModelIndex(), destinationChild);
        const SPoint tmp = points[sourceRow];
        points.erase(points.begin() + sourceRow);
        if (sourceRow <= destinationChild)
            --destinationChild;
        points.insert(points.begin() + destinationChild, tmp);
        endMoveRows();
        return true;
    }

private:
    struct SPoint
    {
        QString name;
        GUI_TYPES::SCalibPoint pnt;
    };
    std::vector <SPoint> points;
};



class CViewStyle: public QProxyStyle
{
public:
    CViewStyle(QStyle* style = 0) : QProxyStyle(style) { }

    void drawPrimitive (QStyle::PrimitiveElement element, const QStyleOption * option,
                         QPainter * painter, const QWidget * widget = 0 ) const final {
        if (element == QStyle::PE_IndicatorItemViewItemDrop && !option->rect.isNull()) {
            QStyleOption opt(*option);
            if (widget)
                opt.rect = QRect(0, opt.rect.top(), widget->width(), 2);
            QBrush br = painter->background();
            br.setColor(opt.palette.color(QPalette::Highlight));
            painter->setBrush(br);
            QProxyStyle::drawPrimitive(element, &opt, painter, widget);
        }
        else
            QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};



class CItemDelegate : public QStyledItemDelegate
{
public:
    CItemDelegate(CCalibPointsOrderModel &model, QObject * const parent) :
        QStyledItemDelegate(parent),
        mdl(model) { }

protected:
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const final {
        (void)option;

        std::vector <GUI_TYPES::SCalibPoint> points = mdl.getCalibPoints();
        const size_t row = static_cast <size_t> (index.row());
        if (row < points.size()) {
            GUI_TYPES::SCalibPoint &pnt = points[row];
            CAddCalibPointDialog dlg(parent, pnt);
            if (dlg.exec() == QDialog::Accepted) {
                pnt = dlg.getCalibPoint();
                mdl.setCalibPoints(points);
            }
        }
        return nullptr;
    }

private:
    CCalibPointsOrderModel &mdl;
};

}



class CCalibPointsOrderDialogPrivate
{
    friend class CCalibPointsOrderDialog;

    CCalibPointsOrderModel mdl;
};



CCalibPointsOrderDialog::CCalibPointsOrderDialog(const std::vector<GUI_TYPES::SCalibPoint> &calibPnts,
                                                 QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CCalibPointsOrderDialog),
    d_ptr(new CCalibPointsOrderDialogPrivate)
{
    ui->setupUi(this);

    d_ptr->mdl.setCalibPoints(calibPnts);
    ui->tableView->setModel(&d_ptr->mdl);
    ui->tableView->resizeColumnsToContents();

    ui->tableView->setStyle(new CViewStyle(ui->tableView->style()));
    ui->tableView->setItemDelegate(new CItemDelegate(d_ptr->mdl, ui->tableView));

    connect(ui->pbOk, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(ui->pbCancel, &QAbstractButton::clicked, this, &QDialog::reject);
}

CCalibPointsOrderDialog::~CCalibPointsOrderDialog()
{
    delete ui;
    delete d_ptr;
}

std::vector<GUI_TYPES::SCalibPoint> CCalibPointsOrderDialog::getCalibPoints() const
{
    return d_ptr->mdl.getCalibPoints();
}
