# raft

I am no longer working on this, as I didn't have enough time after implementing log storage and raft leader elections. Leader election works though. 

[Raft](https://raft.github.io/raft.pdf) Distributed Consensus in C++

Features:
- [X] Leader Election
- [ ] Log Storage
- [ ] State Machine

Requirements:
- Fixed Peer Group (nodes are not added or removed)
- GRPC installed on system (via env var GRPC_CPP_INSTALL)
