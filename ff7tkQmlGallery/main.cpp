#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "../src/data/FF7Text.h"
#include "../src/data/FF7Item.h"
#include "../src/data/FF7Materia.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    qmlRegisterType<FF7TEXT>("FF7Text", 1, 0, "FF7Text");
    qmlRegisterType<FF7Item>("FF7Item", 1, 0, "FF7Item");
    qmlRegisterType<FF7Item>("FF7Materia", 1, 0, "FF7Materia");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
