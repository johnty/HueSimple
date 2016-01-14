#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QCommandLineParser>


void setLightOn(bool isOn, int idx, QNetworkAccessManager* man)
{
    QString url_str = "http://192.168.100.230/api/b225912329de4371bdc4d2e18678263/lights/" + QString::number(idx) + "/state/";
    QUrl url(url_str);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject data_obj;
    data_obj.insert("on", isOn);
    QJsonDocument doc(data_obj);
    QByteArray data = doc.toJson();
    man->put(request, data);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QNetworkAccessManager* manager = new QNetworkAccessManager();

    QTimer::singleShot(2000, &a, SLOT(quit())); //trigger stop



    QCommandLineParser parser;
    parser.setApplicationDescription("Hue Simple Controller");
    parser.addHelpOption();
    parser.addVersionOption();

    //QCommandLineOption onOption("on", QCoreApplication::translate("main", "turn light(s) on"));
    //parser.addOption(onOption);

    parser.addPositionalArgument("index", "index(s) of light(s)");

    parser.process(a);


    const QStringList options = parser.positionalArguments();
    if (options.length() >= 2)
    {
        qDebug()<< "isOn = " << options.at(0);

        bool isOn;
        if (options.at(0) == "on")
            isOn = true;
        else
            isOn = false;

        for (int i=1; i<options.length(); ++i)
        {

            QString idx_str = options.at(i);
            int idx = idx_str.toInt();
            qDebug()<< "idx = " << idx;
            setLightOn(isOn, idx, manager);
        }

    }

    //setLightOn(isOn, i, manager);




        //setLightOn(isOn, index, manager);

    //return 0;

    return a.exec();
}

