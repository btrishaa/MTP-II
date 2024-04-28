# MTP-II
Real-Time Scheduling of Periodic Tasks and Messages on Heterogeneous Processors and FlexRay Bus Systems

In this thesis, we address the challenge of scheduling Periodic Tasks and Messages on heterogeneous processors and the FlexRay bus system, crucial for real-time systems. With the growing demand for higher performance and efficiency, heterogeneous platforms are increasingly prevalent. We propose a scheduling algorithm that aims to meet real-time task timing constraints while minimizing make span length. Building upon existing strategies for homogeneous multiprocessor systems, we extend the scope to heterogeneous environments. Our approach considers scheduling real-time systems modeled as Precedence-constrained Task Graphs (PTGs) on fully-connected heterogeneous systems. Additionally, we delve into the scheduling intricacies of the FlexRay protocol’s static segment. Periodic signals must be packed into messages of equal size to comply with FlexRay protocol restrictions while optimizing band- width utilization. We formulate a nonlinear integer programming (NIP) problem to maximize bandwidth usage efficiently. By leveraging the constraints of the FlexRay protocol, we decompose the NIP and compute an optimal message set effectively. Our contributions offer insights and solutions for effectively managing real-time tasks and messages in complex, heterogeneous environments like FlexRay-based systems.
