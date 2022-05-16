# Programming Assignment II

[TOC]

##Program Testing

In order to measure the time *$T$* taken to run the program with different changes/constants in the variables for file size *$F$* and error probability *$p$*, the client program was temporarily reworked to exit after the successful download of a file. **Please note that the time reported includes the 3 sec timeout*

###With Fixed Error Probability $(p=75)$

| File Size ($F$) | Time (*$T$*) |
| --------------- | ------------ |
| 71 bytes        | 0m3.137s     |
| 674 bytes       | 0m3.138s     |
| 2,104 bytes     | 0m3.144s     |

#### Best-Fit Equation

$y = 3.136942 + 7.285249e-7*x + 1.248121e-9*x^2$

![graphone](/Users/willwalker/Desktop/graphone.png)

###With Fixed File Size $(F=2104 \text{ bytes})$

| Probability $(p)$ | Time (*$T$*) |
| ----------------- | ------------ |
| 70%               | 0m3.140s     |
| 80%               | 0m3.169s     |
| 90%               | 0m3.184s     |

#### Best-Fit Equation

$y = 2.545 + 0.0134*x - 0.00007*x^2$

![graphtwo](/Users/willwalker/Desktop/graphtwo.png)