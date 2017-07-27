# pyZ3

Vesicle traffic system and directly encoding in the Solver using python Z3 api.
We have initially encoded the VTS system with our encoding using CBMC model checker (the code of the same is placed in the cbmc folder). 

To run the example of the cbmc you need either of two:
1. cbmc <= 5.4 will give you counterexample with detailed output.
2. cbmc > 5.4 need some extra options to print the counterexmaple. 

The method for cbmc v <= 5.4 is shown.

```c
 cbmc filename.c --unwindset main.loopnumber:numberoftimes,main.loopnumber1:numberoftimes,main.loopnumber:numberoftimes
```
loopnumber can be obtained using :

`cbmc filename.c --show-loops`

The code contain some non-static part so we have to provide the number of times the loop needs to be unrolled. This is marked # Dynamic in the code. 

One sample command is :
` cbmc 1F.c --unwindset main.4:3,main.5:3,main.8:3,main.9:3`

Explain how to run an example.

----------------------
Newer encoding with tool MAA.

We have encoded the VTS directly under the solver using pyZ3. The main difference is the reachability encoding that encodes steady state/stability condition of the VTS, and graph connectivity. Thdefining the reachability recusrsive is the main feature.

Explain how to run an example.

If you have installed pyZ3 directly following the directions at https://github.com/Z3Prover/z3 by make file with  --python option :

`./bio.py -V * -M * -N * -Q * -C * `

* : Suitable Natural number.
for example #./bio.py -V 1 -M 4 -N 2 -Q 2 

