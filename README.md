# raft
[WIP] [Raft](https://raft.github.io/raft.pdf) Distributed Consensus

## Installation
```
git clone https://github.com/kyuds/raft.git
cd raft && ./install.sh
```

### Installing gRPC
This project uses gRPC to communicate messages between each node. In order to install gRPC, refer to this [link](https://grpc.io/docs/languages/cpp/quickstart/).

## Development Plan
- [ ] v1: std:string based log replication, fixed quorum
- [ ] v2: single value state machine replication
- [ ] v3: changing quorum, single value smr
- [ ] v4: interface for generate state machine
- [ ] v5: KV store example?
