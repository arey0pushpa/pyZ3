# pyZ3

Vesicle traffic system and directly encoding in the Solver using python Z3 api.
We have initially encoded the VTS system with our encoding using CBMC model checker (the code of the same is placed in the cbmc folder). 

To run the example of the cbmc you need either of two:
1. cbmc <= 5.4 will give you counterexample with detailed output.
2. cbmc > 5.4 need some extra options to print the counterexmaple. 

The method for cbmc v <= 5.4 is shown.

Explain how to run an example.

----------------------
Newer encoding with tool MAA.

We have encoded the VTS directly under the solver using pyZ3. The main difference is the reachability encoding that encodes steady state/stability condition of the VTS, and graph connectivity. Thdefining the reachability recusrsive is the main feature.

Explain how to run an example.
......
