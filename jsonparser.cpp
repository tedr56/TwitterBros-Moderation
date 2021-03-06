#include "jsonparser.h"
#include <QStringList>
#include <QVariant>

JsonParser::JsonParser(QJsonDocument *JsonDoc)
{
    JsonDocument = JsonDoc;
}

JsonParser::~JsonParser()
{

}

QString JsonParser::parseJson(QString Path)
{
    QStringList XPath = Path.split("/");
    QJsonObject root = JsonDocument->object();
    QJsonValue resultValue = QJsonValue(root);

    foreach (QString element, XPath) {
        if (resultValue.isObject())
            resultValue = parseObject(resultValue.toObject(), element);
        else if (resultValue.isArray())
            resultValue = parseArray(resultValue.toArray(), element);
        else if (resultValue.isUndefined())
            break;
    }
    QString resultString;
    resultString = resultValue.toString("");
    return resultString;
}

QJsonValue JsonParser::parseObject(QJsonObject object, QString path)
{
    if (object.contains(path))
    {
        return object.value(path);
    }
    else
        return QJsonValue();
}

QJsonValue JsonParser::parseArray(QJsonArray array, QString path)
{
    QStringList pathSplit = path.split("[");
    QString arrayIndex = pathSplit[1];
    arrayIndex.truncate(1);
    if (array.size() > arrayIndex.toInt())
    {
        return array.at(arrayIndex.toInt());
    }
    else
        return QJsonValue();
}

