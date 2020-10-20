#ifndef WIDGET_H
#define WIDGET_H

#include "QApplication"
#include "QDesktopWidget"
#include <QAction>
#include <QApplication>
#include <QBrush>
#include <QDateTime>
#include <QDebug>
#include <QLinearGradient>
#include <QList>
#include <QListView>
#include <QMenu>
#include <QModelIndex>
#include <QPainter>
#include <QPalette>
#include <QRect>
#include <QScreen>
#include <QStandardItemModel>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

class ColorDelegate : public QStyledItemDelegate
{
    Q_OBJECT
   public:
    ColorDelegate(QObject* parent) : QStyledItemDelegate(parent) {}
    ColorDelegate() {}
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override
    {
        auto text = index.data().toString();
        auto opt = option;
        opt.displayAlignment = Qt::AlignCenter;
        auto width = opt.fontMetrics.width(text);
        auto height = opt.fontMetrics.height();
        auto w = (opt.rect.width() - width) / 2.0;
        auto h = (opt.rect.height() - height) / 2.0;
        auto r = QRectF(opt.rect.x() + w, opt.rect.y() + h, width, height);
        QLinearGradient l(r.topLeft(), r.bottomRight());
        l.setColorAt(1.0 / 6, Qt::red);
        l.setColorAt(3.0 / 6, Qt::green);    // 二分之一
        l.setColorAt(5.0 / 6, Qt::blue);
        l.setColorAt(1, QColor(255, 0, 255));
        auto role = QPalette::Text;
        opt.palette.setBrush(role, QBrush(l));
        QApplication::style()->drawItemText(painter, opt.rect,
                                            opt.displayAlignment, opt.palette,
                                            true, text, role);
    }

    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override
    {
        auto text = index.data().toString();
        auto width = option.fontMetrics.width(text) + 20;
        return QSize{width, option.fontMetrics.height()};
    }
};

class Widget : public QWidget
{
    Q_OBJECT

   public:
    Widget(QWidget* parent = nullptr);
    ~Widget() = default;

   private:
    void InitTimer();
    void MoveWidget();
    void InitLabel();
    void UpdateText();
    void UpdateView();
    void InitSystemTray();

   private:
    QPoint GetWidgetPosition();

   private:
    QMenu* menu_ = nullptr;
    QListView* view_ = nullptr;
    QTimer* refresh_ = nullptr;
    QAction* quit_action_ = nullptr;
    QSystemTrayIcon* system_ = nullptr;
    QStandardItemModel* model_ = nullptr;
};
#endif    // WIDGET_H
