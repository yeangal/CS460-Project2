# CS460-Project2
CPU Scheduling

- FIFO
  - [1] First come, first served: Processes are queued in order that they arrive in the ready queue
  - [2] How-to-run:
    - ./main -alg FIFO -input [fileInput]
- SJF
  - [1] Shortest Job First: Processes are selected by the shortest execution time
  - [2] How-to-run:
    - ./main -alg SJF -input [fileInput]
- PR
  - [1] Priority: Processes are executed in order of highest priority
  - [2] How-to-run:
    - ./main -alg PR -input [fileInput]
- RR
  - [1] Round Robin: Processes are assigned a fixed time slot
  - [2] How-to-run:
    - ./main -alg RR -quantum [time(ms)] -input [fileInput]
