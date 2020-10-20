#include "widget.h"
#include <cmath>
#include <iomanip>
#include <random>
#include <string>
#include <vector>
static const std::vector<std::string> kMessages = {
    {"休息一下吧"},     {"该休息了"},
    {"起来走走吧"},     {"活动活动脖子吧"},
    {"不要猝死啦"},     {"命不要了吗"},
    {"女朋友不要了吗"}, {"不累吗，赶紧起来走走"},
};

static int GetNextDelaySecond()
{
    constexpr auto kSeconds = 1000 * 1;
    static const std::vector<int> kDelaySeconds = {10, 2, 2, 3, 5, 2};
    static std::size_t index = 0;
    int delay = kDelaySeconds.at(index);
    index = ++index == kDelaySeconds.size() ? 0 : index;
    return delay * kSeconds;
}

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
    refresh_ = new QTimer(this);
    connect(refresh_, &QTimer::timeout, [this]() {
        MoveWidget();
        UpdateText();
    });
    refresh_->start(GetNextDelaySecond());
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

void Widget::UpdateText()
{
    refresh_->stop();

    if (this->isHidden())
    {
        UpdateView();
        this->show();
    }
    else
    {
        this->hide();
    }
    refresh_->start(GetNextDelaySecond());
}

static int RandomMessageIndex(int max)
{
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(0, max - 1);
    return uniform_dist(e1);
}

void Widget::UpdateView()
{
    auto empty = [] { return new QStandardItem{""}; };
    //
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("hh:mm:ss");
    auto time = new QStandardItem{current_date};
    //
    int index = RandomMessageIndex(kMessages.size());
    auto msg = new QStandardItem(kMessages.at(index).data());

    model_->clear();
    QList<QStandardItem*> items;
    items << empty() << time << empty() << msg << empty();
    //
    for (const auto& view : items)
    {
        model_->appendRow(view);
    }
}
