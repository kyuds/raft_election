#!/bin/bash

# cmake command to build. 

if [[ -z "${GRPC_CPP_INSTALL}" ]]; then
  echo "need to set GRPC_CPP_INSTALL env var to point to gRPC installation directory."
  exit
fi

mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=$GRPC_CPP_INSTALL ..
cmake --build .
cd ..
