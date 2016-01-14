#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QThread>
#include "RtMidi.h"

//put IP address and API key for Hue bridge here:
const QString api_root = "http://192.168.100.230/api/b225912329de4371bdc4d2e18678263/lights/";

//put light index and number of lights here:
const int lights[] = {4, 12};
const int num_lights = 2;

void setLightOn(bool isOn, int idx, QNetworkAccessManager* man)
{
    QString url_str = api_root + QString::number(idx) + "/state/";
    QUrl url(url_str);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject data_obj;
    data_obj.insert("on", isOn);
    QJsonDocument doc(data_obj);
    QByteArray data = doc.toJson();
    man->put(request, data);
}

void setBrightness(int bri, int idx, QNetworkAccessManager* man)
{
    QString url_str = api_root + QString::number(idx) + "/state/";
    QUrl url(url_str);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject data_obj;
    data_obj.insert("bri", bri);
    QJsonDocument doc(data_obj);
    QByteArray data = doc.toJson();
    man->put(request, data);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QNetworkAccessManager* manager = new QNetworkAccessManager();

    //set up midi

    RtMidiIn* midiIn = new RtMidiIn();
    midiIn->openPort(0);
    qDebug() <<"num ports = " << midiIn->getPortCount();
    qDebug() <<"opened Midi port " << midiIn->getPortName(0).c_str();

    //NOTE:we don't do any checking so make sure
    // the variables at top are set!!
    for (int i=0; i<num_lights; ++i)
    {
        setLightOn(true, lights[i], manager);
        setBrightness(255, lights[i], manager);
    }
    std::vector<uchar> message;

    uint bri0 = 0;
    uint bri1 = 0;
    uint bri = 0;
    uint pre_bri = 0;
    bool isOn = true;
    while (1)
    {
        //do midi object poll
        QThread::msleep(1); //don't use 100% cpu
        double tstamp = midiIn->getMessage(&message);

        if (message.size() == 3)
        {
            qDebug() << message.at(0) <<" "
                        << message.at(1) <<" "
                           << message.at(2);

            bri0 = 127 - message.at(2);
            bri1 = bri0;
            bri = bri0 + bri1; //kind of filter here...
        }
        //int bri = message.at(2);
        if (bri != pre_bri)
        {
            for (int i=0; i<num_lights; ++i)
            {
                setBrightness(bri, lights[i], manager);
            }
            pre_bri = bri;

            if (bri < 10)
            {
                for (int i=0; i<num_lights; ++i)
                {
                    setLightOn(false, lights[i], manager);
                }
                isOn = false;
                qDebug()<<"OFF!";
            }
            else
            {
                if (!isOn)
                {
                    for (int i=0; i<num_lights; ++i)
                    {
                        setLightOn(true, lights[i], manager);
                    }
                    isOn = true;
                    qDebug()<<"ON!";
                }
            }
        }
        //process message loop
        QCoreApplication::processEvents();
    }

    //cleanup
    delete midiIn;
}

