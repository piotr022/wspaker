## wspaker
paker.hpp - single header lib, for packing binary data into weak singal protocols with universal formatting

### example of packing data into ft4/ft8 CQ frame format
utils/paker_test.cpp

```
piotr:utils/ (main✗) $ ./paker_test                                                                                                           [2:53:01]

paker format: CQ nQSxxx xxNN
frame permutations: 2376275200
frame bitsize ffloor: 31
specific base: C=1 Q=1  =1 n=2 Q=1 S=1 x=26 x=26 x=26  =1 x=26 x=26 N=10 N=10 

test raw data: eaffffff65bdc3fffa230b0c

encoded output: 
frame [0]:  CQ 1Q0UZV NG37
frame [1]:  CQ 1Q1UVI SK38
frame [2]:  CQ 0Q2SSW AL06
frame [3]:  CQ 0Q3AAA AA11

decoded back to: eaffffff65bdc3fffa230b0c
```
