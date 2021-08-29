#include "cjsonfilepointssaver.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>

class CJsonFilePointsSaverPrivate
{
    friend class CJsonFilePointsSaver;

    QString fName;
};

CJsonFilePointsSaver::CJsonFilePointsSaver() :
    d_ptr(new CJsonFilePointsSaverPrivate())
{

}

CJsonFilePointsSaver::~CJsonFilePointsSaver()
{
    delete d_ptr;
}

void CJsonFilePointsSaver::setFileName(const char *fName)
{
    d_ptr->fName = fName;
}

bool CJsonFilePointsSaver::savePointsPrivate(const std::vector<GUI_TYPES::STaskPoint> &taskPoints,
                                             const std::vector<GUI_TYPES::SHomePoint> &homePoints)
{
    QJsonObject obj;

    QJsonArray taskArray;
    for(auto pnt : taskPoints)
    {
        QJsonObject task;
        task["taskType"]    = pnt.taskType;
        task["posX"]        = pnt.globalPos.x;
        task["posY"]        = pnt.globalPos.y;
        task["posZ"]        = pnt.globalPos.z;
        task["angleX"]      = pnt.angle.x;
        task["angleY"]      = pnt.angle.y;
        task["angleZ"]      = pnt.angle.z;
        task["normalX"]     = pnt.normal.x;
        task["normalY"]     = pnt.normal.y;
        task["normalZ"]     = pnt.normal.z;
        task["delay"]       = pnt.delay;
        task["zSimmetry"]   = pnt.zSimmetry;
        task["bNeedCalib"]  = pnt.bNeedCalib;
        task["bUseHomePnt"] = pnt.bUseHomePnt;
        taskArray.push_back(task);
    }
    obj["tasks"] = taskArray;

    QJsonArray homeArray;
    for(auto pnt : homePoints)
    {
        QJsonObject homePnt;
        homePnt["posX"]        = pnt.globalPos.x;
        homePnt["posY"]        = pnt.globalPos.y;
        homePnt["posZ"]        = pnt.globalPos.z;
        homePnt["angleX"]      = pnt.angle.x;
        homePnt["angleY"]      = pnt.angle.y;
        homePnt["angleZ"]      = pnt.angle.z;
        homePnt["normalX"]     = pnt.normal.x;
        homePnt["normalY"]     = pnt.normal.y;
        homePnt["normalZ"]     = pnt.normal.z;
        homeArray.push_back(homePnt);
    }
    obj["homes"] = homeArray;

    QJsonDocument doc;
    doc.setObject(obj);
    bool result = false;
    if (!d_ptr->fName.isEmpty())
    {
        QFile file(d_ptr->fName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            file.write(doc.toJson(QJsonDocument::Indented));
            result = true;
            file.close();
        }
    }
    return result;
}

bool CJsonFilePointsSaver::loadPointsPrivate(std::vector<GUI_TYPES::STaskPoint> &taskPoints,
                                             std::vector<GUI_TYPES::SHomePoint> &homePoints)
{
    taskPoints.clear();
    homePoints.clear();
    bool result = false;
    if (!d_ptr->fName.isEmpty())
    {
        QFile file(d_ptr->fName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            const QByteArray data = file.readAll();
            file.close();
            result = !data.isEmpty();
            const QJsonDocument doc = QJsonDocument::fromJson(data);
            const QJsonObject obj = doc.object();

            const QJsonArray taskArray = obj.value("tasks").toArray();
            for(auto task : taskArray)
            {
                const QJsonObject taskObj = task.toObject();
                GUI_TYPES::STaskPoint taskPnt;
                taskPnt.taskType    = taskObj.value("taskType"   ).toInt();
                taskPnt.globalPos.x = taskObj.value("posX"       ).toDouble();
                taskPnt.globalPos.y = taskObj.value("posY"       ).toDouble();
                taskPnt.globalPos.z = taskObj.value("posZ"       ).toDouble();
                taskPnt.angle.x     = taskObj.value("angleX"     ).toDouble();
                taskPnt.angle.y     = taskObj.value("angleY"     ).toDouble();
                taskPnt.angle.z     = taskObj.value("angleZ"     ).toDouble();
                taskPnt.normal.x    = taskObj.value("normalX"    ).toDouble();
                taskPnt.normal.y    = taskObj.value("normalY"    ).toDouble();
                taskPnt.normal.z    = taskObj.value("normalZ"    ).toDouble();
                taskPnt.delay       = taskObj.value("delay"      ).toDouble();
                taskPnt.zSimmetry   = taskObj.value("zSimmetry"  ).toBool();
                taskPnt.bNeedCalib  = taskObj.value("bNeedCalib" ).toBool();
                taskPnt.bUseHomePnt = taskObj.value("bUseHomePnt").toBool();
                taskPoints.push_back(taskPnt);
            }

            const QJsonArray homeArray = obj.value("homes").toArray();
            for(auto home : homeArray)
            {
                const QJsonObject homeObj = home.toObject();
                GUI_TYPES::SHomePoint homePnt;
                homePnt.globalPos.x = homeObj.value("posX"       ).toDouble();
                homePnt.globalPos.y = homeObj.value("posY"       ).toDouble();
                homePnt.globalPos.z = homeObj.value("posZ"       ).toDouble();
                homePnt.angle.x     = homeObj.value("angleX"     ).toDouble();
                homePnt.angle.y     = homeObj.value("angleY"     ).toDouble();
                homePnt.angle.z     = homeObj.value("angleZ"     ).toDouble();
                homePnt.normal.x    = homeObj.value("normalX"    ).toDouble();
                homePnt.normal.y    = homeObj.value("normalY"    ).toDouble();
                homePnt.normal.z    = homeObj.value("normalZ"    ).toDouble();
                homePoints.push_back(homePnt);
            }
        }
    }
    return result;
}
