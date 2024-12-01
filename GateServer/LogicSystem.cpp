//
// Created by myheng on 24-11-29.
//

#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VarifyGrpcClient.h"
void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
    _get_handlers.insert(make_pair(url, handler));
}

LogicSystem::LogicSystem() {
    RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->_response.body()) << "receive get_test req " << std::endl;
        int i = 0;
        for (auto& elem : connection->_get_params) {
            i++;
            beast::ostream(connection->_response.body()) << "param" << i << " key is " << elem.first;
            beast::ostream(connection->_response.body()) << ", " <<  " value is " << elem.second << std::endl;
        }
    });

    RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        bool parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        auto email = src_root["email"].asString();

        GetVarifyRsp rsp = VarifyGrpcClient::GetInstance()->GetVarifyCode(email);
        std::cout << "email is " << email << std::endl;
        root["error"] = rsp.error();
        root["email"] = src_root["email"];
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
    });


}
LogicSystem::~LogicSystem()
{

}
bool LogicSystem::HandleGet(std::string url, std::shared_ptr<HttpConnection> con) {
    if (_get_handlers.find(url) == _get_handlers.end()) {
        return false;
    }

    _get_handlers[url](con);
    return true;
}

bool LogicSystem::HandlePost(std::string url, std::shared_ptr<HttpConnection> con) {

    if (_post_handlers.find(url) == _post_handlers.end()) {
        return false;
    }

    _post_handlers[url](con);
    return true;
}

void LogicSystem::RegPost(std::string url, HttpHandler handler) {
    _post_handlers.insert(make_pair(url, handler));
}

