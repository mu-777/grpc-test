#ifndef GRPC_TEST_REFLECTION_SERVER_H
#define GRPC_TEST_REFLECTION_SERVER_H

#include "reflection.pb.h"
#include "reflection.grpc.pb.h"

namespace reflection {
class ReflectionImpl final : public Area::Service {

  const float PI = 3.14159265f;

  grpc::Status CalcArea(grpc::ServerContext *context,
                        const reflection::ShapeData *request,
                        reflection::AreaData *response) override {
    std::cout << "Received data" << std::endl;
    auto s = CalcAreaImpl(request);
    std::cout << "Calced: " << s << std::endl;
    response->set_area(s);
    return ::grpc::Status::OK;
  }

  float CalcAreaImpl(const reflection::ShapeData *sd) {
    switch (sd->shape_case()) {
      case ShapeData::ShapeCase::kTriangle:
        return sd->triangle().baseside() * sd->triangle().height() * 0.5f;
      case ShapeData::ShapeCase::kRectangle:
        return sd->rectangle().side1() * sd->rectangle().side2();
      case ShapeData::ShapeCase::kSquare:
        return sd->square().side() * sd->square().side();
      case ShapeData::ShapeCase::kCircle:
        return sd->circle().radius() * sd->circle().radius() * PI;
      case ShapeData::ShapeCase::SHAPE_NOT_SET:
        std::cout << "SHAPE_NOT_SET" << std::endl;
        return 0;
    }
    return 0;
  }
};
};

#endif //GRPC_TEST_REFLECTION_SERVER_H
