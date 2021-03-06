#include "application.h"

#include "controller/configsetter.h"
#include "controller/dbmanager.h"
#include "controller/exporter.h"
#include "controller/globaleventfilter.h"
#include "controller/importer.h"
#include "controller/signalmanager.h"
#include "controller/wallpapersetter.h"
#include "controller/viewerthememanager.h"
#include "dirwatcher/scanpathsdialog.h"

#include <QDebug>
#include <QTranslator>

namespace {

}  // namespace

Application::Application(int& argc, char** argv)
    : DApplication(argc, argv)
{
    initI18n();
    setOrganizationName("deepin");
    setApplicationName("deepin-image-viewer");
    setApplicationDisplayName(tr("Deepin Image Viewer"));
    setApplicationVersion("1.2");

    installEventFilter(new GlobalEventFilter());


    initChildren();
}

void Application::initChildren()
{
    viewerTheme = ViewerThemeManager::instance();
    setter = ConfigSetter::instance();
    exporter = Exporter::instance();
    signalM = SignalManager::instance();
    dbM = new DBManager();
    importer = new Importer();
    scanDialog = ScanPathsDialog::instance();
    wpSetter = WallpaperSetter::instance();
}

void Application::initI18n()
{
    // install translators
//    QTranslator *translator = new QTranslator;
//    translator->load(APPSHAREDIR"/translations/deepin-image-viewer_"
//                     + QLocale::system().name() + ".qm");
//    installTranslator(translator);
    loadTranslator(QList<QLocale>() << QLocale::system());

}
