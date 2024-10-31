#ifndef CONF_H
#define CONF_H

#include "settings/settings.h"

#include <qobject.h>
#include <qtmetamacros.h>
#include <qtypes.h>


class Conf : public QObject
{
    Q_OBJECT

public:
    static const Conf& instance()
    {
        static const Conf conf;
        return conf;
    }

private:
    Conf() : QObject(nullptr) 
    {
        using namespace lzl::qt_utils;
        Settings::registerSignal("app/fontSize", this, &Conf::onFontSizeChanged);
    }

signals:
    void onFontSizeChanged(qreal size);
};

#endif // CONF_H
