#ifndef ASPECTRATIOWIDGET_H
#define ASPECTRATIOWIDGET_H

#include <QWidget>
#include <QBoxLayout>

class AspectRatioWidget : public QWidget
{
public:
    AspectRatioWidget(QWidget *widget, float width, float height, QWidget *parent = 0);
    explicit AspectRatioWidget(QWidget *parent = nullptr);

    void set_ration(float width, float height);
    void set_layout(QWidget *widget);

    void set_layout_remove(QWidget *widget);

    void delete_layout();

    void resizeEvent(QResizeEvent *event);

private:
    QBoxLayout *layout;
    QSpacerItem *qspaceItem1;
    QSpacerItem *qspaceItem2;
    float arWidth;
    float arHeight;
};

#endif // ASPECTRATIOWIDGET_H
