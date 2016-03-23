#ifndef FILTERSPREVIEW_H
#define FILTERSPREVIEW_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>

#include <dscrollarea.h>
#include <dlistwidget.h>
#include <dslider.h>

//#define USE_DLISTWIDGET
using namespace Dtk::Widget;
namespace filter2d {
class FilterObj;
}
class FiltersPreview : public QWidget
{
    Q_OBJECT
public:
    explicit FiltersPreview(QWidget *parent = 0);
    void setImage(const QImage& img);
    qreal indensity() const;
Q_SIGNALS:
    void filterIdSelected(int id);
    void indensityChanged(qreal);

private Q_SLOTS:
    void applyIndensity(int value);

private:
    QVector<QLabel*> m_label;
    QVector<filter2d::FilterObj*> m_filter;
#ifdef USE_DLISTWIDGET
    DListWidget *m_list;
#else
    QListWidget *m_list;
#endif
    DSlider *m_indensity;
    QImage m_image;
};

#endif // FILTERSPREVIEW_H