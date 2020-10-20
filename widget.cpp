#include "widget.h"
#include <cmath>
#include <iomanip>
#include <random>
#include <string>
#include <vector>

constexpr auto kSeconds = 1000 * 1;
constexpr auto kMinutes = 60 * kSeconds;
// constexpr auto kHours = 60 * kMinutes;
constexpr auto kDelay = 3 * kSeconds;
constexpr auto kShowInterval = 2 * kSeconds;
static const std::vector<std::string> kMessages = {
    {"休息一下吧"},     {"该休息了"},
    {"起来走走吧"},     {"活动活动脖子吧"},
    {"不要猝死啦"},     {"命不要了吗"},
    {"女朋友不要了吗"}, {"不累吗，赶紧起来走走"},
};

Widget::Widget(QWidget* parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    this->resize(300, 100);
    InitLabel();
    UpdateView();
    InitTimer();
    InitSystemTray();
}

void Widget::InitLabel()
{
    model_ = new QStandardItemModel(this);
    view_ = new QListView(this);
    view_->setModel(model_);
    view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view_->setItemDelegate(new ColorDelegate(this));    // 设置自定义delegate
    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(view_);
    setLayout(layout);
}

void Widget::InitSystemTray()
{
    system_ = new QSystemTrayIcon(this);
    menu_ = new QMenu(this);
    quit_action_ = new QAction(this);
    quit_action_->setText("Quit");
    menu_->addAction(quit_action_);
    connect(quit_action_, &QAction::triggered, [this]() { close(); });
    auto icon_index = QStyle::StandardPixmap::SP_DialogCloseButton;
    auto icon = QApplication::style()->standardIcon(icon_index);

    system_->setIcon(icon);
    system_->setToolTip("Quit Clock Tip");
    system_->setContextMenu(menu_);
    connect(system_, &QSystemTrayIcon::activated, [this]() { close(); });
    system_->show();
}
void Widget::InitTimer()
{
    show_ = new QTimer(this);
    hide_ = new QTimer(this);
    connect(show_, &QTimer::timeout, [this]() {
        MoveWidget();
        UpdateText();
    });
    connect(hide_, &QTimer::timeout, [this]() { HideWidget(); });

    show_->start(kDelay);
}
QPoint Widget::GetWidgetPosition()
{
    setWindowFlags(windowFlags() | Qt::BypassWindowManagerHint);
    QList<QScreen*> list_screen = QGuiApplication::screens();
    QRect rect = list_screen.at(0)->geometry();
    return {rect.width() - this->width(), rect.height() - this->height()};
}

void Widget::MoveWidget()
{
    auto pos = GetWidgetPosition();
    this->move(pos);
}

void Widget::HideWidget()
{
    hide_->stop();
    show_->start();
    if (this->isHidden())
    {
        return;
    }
    this->hide();
}

void Widget::UpdateText()
{
    show_->stop();
    hide_->start(kShowInterval);

    this->hide();
    UpdateView();
    this->show();
}

static int RandomMessageIndex()
{
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(1, kMessages.size());
    return uniform_dist(e1);
}

void Widget::UpdateView()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("hh:mm:ss");
    auto empty_item = [] { return new QStandardItem{""}; };
    auto time_item = new QStandardItem{current_date};
    int index = RandomMessageIndex() - 1;
    auto msg_item = new QStandardItem{kMessages.at(index).data()};

    QList<QStandardItem*> items;
    items << empty_item() << time_item << empty_item() << msg_item
          << empty_item();
    model_->clear();
    for (const auto& view : items)
    {
        model_->appendRow(view);
    }
}
