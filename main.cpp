#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include "./include/OpenGLItem.h"

int main(int argc, char *argv[])
{
    cv::cuda::setDevice(0);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/icon/icon/application.ico"));

    QQmlApplicationEngine engine;

    qmlRegisterType<OpenGLItem>("OpenGLItem",1,0,"OpenGLItem");

    const QUrl url(QStringLiteral("qrc:/ui/ui/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
