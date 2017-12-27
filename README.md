# PyZ3

A eukaryotic cell contains multiple membrane-bound compartments. Transport vesicles move cargo between these compartments, just as trucks move cargo between warehouses.

The complete vesicle fusion process is modelled as a contraint over a labeled graph. Where node represents compartments and directed edges are vesicles. The whole network is recyclable [molecule moves only in cycles]. We call this labeled network as Vesicle Traffic System (VTS).

We have modelled the VTS using CBMC [C Bounded Model Checker] and python Z3 [Z3 Theorem Prover].


<!--### CBMC tool Command :
#To run the example of the cbmc you need either of two:
#1. cbmc <= 5.4 will give you counterexample with detailed output.
#2. cbmc > 5.4 need some extra options to print the counterexmaple. 
#
#The method for cbmc v <= 5.4 is shown.
#
#`cbmc filename.c --unwindset main.loopnumber:numberoftimes,main.loopnumber1:numberoftimes,main.loopnumber:numberoftimes`
#loopnumber can be obtained using :
#
#`cbmc filename.c --show-loops`
#
#The code contain some nongg-static part so we have to provide the number of times the loop needs to be unrolled. This is marked # Dynamic in the code. 
#
#One sample command is :
#` cbmc 1F.c --unwindset main.4:3,main.5:3,main.8:3,main.9:3`
#
#Explain how to run an example.
#
### MAA tool run command:
#
#Newer encoding with tool MAA.
#
#We have encoded the VTS directly under the solver using pyZ3. The main difference is the reachability encoding that encodes steady state/stability condition of the VTS, and graph connectivity. Thdefining the reachability recursive is the main feature.
#
#If you have installed pyZ3 directly following the directions at https://github.com/Z3Prover/z3 by make file with  --python option :
#
#`./bio.py -V * -M * -N * -Q * -C * `
#
#* : Suitable Natural number.
#for example `./bio.py -V 1 -M 4 -N 2 -Q 2 `
#We have initially modelled the VTS encoded the VTS system with our encoding using CBMC model checker (the code of the same is placed in the cbmc folder). 
-->
