#!/bin/bash

# cmake command to build. 

if [[ -z "${GRPC_CPP_INSTALL}" ]]; then
    echo "need to set GRPC_CPP_INSTALL env var to point to gRPC directory."
    echo "refer to README for more instructions."
    exit
fi

if [ -f "build" ]; then
    echo "there is a file named 'build'."
    exit
elif [ ! -d "build" ]; then
    echo "creating 'build' directory."
    mkdir build
else
    echo "existing 'build' directory found."
fi

cd build
cmake -DCMAKE_PREFIX_PATH=$GRPC_CPP_INSTALL ..
cmake --build .
cd ..
