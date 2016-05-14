#ifndef SOCKETRESPONSE_H
#define SOCKETRESPONSE_H

#include <QObject>
#include <QVariant>
#include <QVariantMap>
#include <QJsonDocument>
#include <QMetaProperty>

class SocketResponse : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool ok READ ok WRITE setOk)
    Q_PROPERTY(QString errorMessage READ errorMessage WRITE setErrorMessage)

public:
    SocketResponse();
    SocketResponse(const SocketResponse& response);
    ~SocketResponse();

    SocketResponse* ErrorResponse(QString errorMessage);
    SocketResponse* OkResponse(const QVariant& result);

    bool ok() const;
    void setOk(bool ok);

    QString errorMessage() const;
    void setErrorMessage(QString errorMessage);

private:
    bool m_bOk;
    QString m_szErrorMessage;

signals:

public slots:

};

Q_DECLARE_METATYPE(SocketResponse)

#endif // SOCKETRESPONSE_H
