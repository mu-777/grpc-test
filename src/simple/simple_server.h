#ifndef GRPC_TEST_SIMPLE_SERVER_H
#define GRPC_TEST_SIMPLE_SERVER_H

#include "simple.pb.h"
#include "simple.grpc.pb.h"

namespace simple {
class SimpleAddImpl final : public Add::Service {
  ::grpc::Status Add(::grpc::ServerContext *context, const AddRequest *req,
                     AddReply *res) override {
    std::cout << "[Server] ReceivedReq: " << req->str1() << ", " << req->str2() << std::endl;
    res->set_str(req->str1() + req->str2());
    return ::grpc::Status::OK;
  }
};
};

#endif //GRPC_TEST_SIMPLE_SERVER_H
