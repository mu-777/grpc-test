#ifndef GRPC_TEST_REFLECTION_SERVER_H
#define GRPC_TEST_REFLECTION_SERVER_H

#include "reflection.pb.h"
#include "reflection.grpc.pb.h"

namespace reflection {
class ReflectionImpl final : public Area::Service {
  grpc::Status CalcArea(grpc::ServerContext *context,
                        const reflection::ShapeData *request,
                        reflection::AreaData *response) override {
    std::cout << "Received data" << std::endl;
    response->set_area(1.0f);
    return ::grpc::Status::OK;
  }

};
};

#endif //GRPC_TEST_REFLECTION_SERVER_H
