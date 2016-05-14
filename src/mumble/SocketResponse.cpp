#include "SocketResponse.h"

SocketResponse::SocketResponse()
{
    this->m_bOk = true;
}

SocketResponse::SocketResponse(const SocketResponse &response) {
    this->m_bOk = response.ok();
    this->m_szErrorMessage = response.errorMessage();
}

SocketResponse::~SocketResponse() {

}

bool SocketResponse::ok() const {
    return this->m_bOk;
}

void SocketResponse::setOk(bool ok) {
    this->m_bOk = ok;
}

QString SocketResponse::errorMessage() const {
    return this->m_szErrorMessage;
}

void SocketResponse::setErrorMessage(QString errorMessage) {
    this->m_szErrorMessage = errorMessage;
}

SocketResponse* SocketResponse::ErrorResponse(QString errorMessage) {
    SocketResponse* response = new SocketResponse;
    response->setOk(false);
    response->setErrorMessage(errorMessage);

    return response;
}

SocketResponse* SocketResponse::OkResponse(const QVariant& result) {
    SocketResponse* response = new SocketResponse;
    response->setOk(true);

    return response;
}
