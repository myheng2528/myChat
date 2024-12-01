//
// Created by myheng on 24-12-1.
//
#include "VarifyGrpcClient.h"
GetVarifyRsp VarifyGrpcClient::GetVarifyCode(std::string email) {
    ClientContext context;
    GetVarifyRsp reply;
    GetVarifyReq request;
    request.set_email(email);

    Status status = stub_->GetVarifyCode(&context, request, &reply);

    if (status.ok()) {

        return reply;
    }
    else {
        reply.set_error(ErrorCodes::RPCFailed);
        return reply;
    }
}