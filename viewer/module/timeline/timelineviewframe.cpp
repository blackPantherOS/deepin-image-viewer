#include "timelineviewframe.h"
#include "application.h"
#include "controller/wallpapersetter.h"
#include "utils/baseutils.h"
#include "utils/imageutils.h"
#include "widgets/thumbnaildelegate.h"
#include <QBuffer>
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPainter>
#include <QPixmapCache>
#include <QResizeEvent>

namespace {

const QString FAVORITES_ALBUM_NAME = "My favorites";
const QString SHORTCUT_SPLIT_FLAG = "@-_-@";

}  //namespace

TimelineViewFrame::TimelineViewFrame(const QString &timeline,
                                     QWidget *parent)
    : QFrame(parent),
      m_timeline(timeline)
{
    using namespace utils::base;

    initListView();

    m_separator = new QLabel();
    m_separator->setFixedWidth(this->width());
    m_separator->setObjectName("TimelineSeparator");
    m_separator->setFixedHeight(1);


    m_title = new QLabel(timeline);
    m_title->setFixedWidth(this->width());
    // Translate time
    const QDateTime tt = stringToDateTime(timeline);
    if (tt.isValid()) {
        m_title->setText(tt.toString(tr("dd MMMM yyyy")));
    }
    m_title->setFixedHeight(stringHeight(m_title->font(), m_title->text()));
    m_title->setAlignment(Qt::AlignLeft);
    m_title->setObjectName("TimelineFrameTitle");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addSpacing(4);
    layout->addWidget(m_title, 0, Qt::AlignHCenter);
    layout->addSpacing(0);
    layout->addWidget(m_separator, 0, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(m_view);
    layout->addSpacing(15);
}

TimelineViewFrame::~TimelineViewFrame()
{

}

void TimelineViewFrame::initListView()
{
    m_view = new ThumbnailListView(this);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_view, &ThumbnailListView::clicked,
            this, &TimelineViewFrame::clicked);
    connect(m_view, &ThumbnailListView::singleClicked,
            this, &TimelineViewFrame::singleClicked);
    connect(m_view, &ThumbnailListView::doubleClicked,
            this, [=] (const QModelIndex & index) {
        const QString path =
                m_view->itemInfo(index).path.toUtf8().toPercentEncoding("/");
        emit viewImage(path, dApp->dbM->getAllPaths());
    });
    connect(m_view, &ThumbnailListView::customContextMenuRequested,
            this, [=] (const QPoint &pos) {
        if (m_view->indexAt(pos).isValid()) {
            emit showMenuRequested();
        }
    });
}

void TimelineViewFrame::updateThumbnails()
{
    m_view->updateThumbnails();
}

void TimelineViewFrame::selectAll()
{
    m_view->selectAll();
}

bool TimelineViewFrame::posInSelected(const QPoint &pos)
{
    QModelIndexList list = m_view->selectionModel()->selectedIndexes();
    return list.indexOf(m_view->indexAt(pos)) != -1;
}

void TimelineViewFrame::setIconSize(const QSize &iconSize)
{
    m_view->setIconSize(iconSize);
}

void TimelineViewFrame::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);
    m_title->setFixedWidth(width() + m_view->hOffset() * 2 - 6);
    m_separator->setFixedWidth(width() + m_view->hOffset() * 2 - 6);
}

void TimelineViewFrame::insertItem(const DBImgInfo &info)
{
    using namespace utils::image;
    ThumbnailListView::ItemInfo vi;
    vi.name = QByteArray::fromPercentEncoding(info.fileName.toUtf8());
    vi.path = QByteArray::fromPercentEncoding(info.filePath.toUtf8());
    vi.thumb = cutSquareImage(getThumbnail(vi.path, true));

    m_view->insertItem(vi);
}

void TimelineViewFrame::insertItems(const DBImgInfoList &infos)
{
    using namespace utils::image;
    for (auto info : infos) {
        ThumbnailListView::ItemInfo vi;
        vi.name = QByteArray::fromPercentEncoding(info.fileName.toUtf8());
        vi.path = QByteArray::fromPercentEncoding(info.filePath.toUtf8());
        vi.thumb = cutSquareImage(getThumbnail(vi.path, true));

        m_view->insertItem(vi);
    }
}

void TimelineViewFrame::removeItems(const QStringList &paths)
{
    m_view->removeItems(paths);
}

void TimelineViewFrame::clearSelection() const
{
    m_view->selectionModel()->clearSelection();
}

/*!
    \fn QMap<QString, QString> TimelineViewFrame::selectedImages() const

    Return the name-path map of all selected items.
*/
QMap<QString, QString> TimelineViewFrame::selectedImages() const
{
    QMap<QString, QString> images;
    const QList<ThumbnailListView::ItemInfo> infos = m_view->selectedItemInfos();
    for (ThumbnailListView::ItemInfo info : infos) {
        images.insert(info.name, info.path);
    }

    return images;
}

QString TimelineViewFrame::timeline() const
{
    return m_timeline;
}

bool TimelineViewFrame::isEmpty() const
{
    return m_view->count() == 0;
}

int TimelineViewFrame::hOffset() const
{
    return m_view->hOffset();
}
