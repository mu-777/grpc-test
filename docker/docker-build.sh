cd $(dirname $0)
docker build -t ryosuke/grpc-cpp -f ./Dockerfile.grpc .
docker build -t ryosuke/grpc-cpp-dev -f ./Dockerfile.grpc-dev .