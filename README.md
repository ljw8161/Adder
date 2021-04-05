# Adder
⌨ team project for [2020-1] KNU capstone project2<br>
🖱 mento professor YongTae Kim <br>
🎥 Demo Video: https://youtu.be/1foM8boSv5A

## Introduction
##### A Study & Proposal for a New Low-Power Energy Efficient Adder Algorithm for Reducing Power and Energy Consumption of Artificial Intelligence Processors
##### Evaluation of Proposed Approximate Adder Algorithm
- Assessing algorithmic performance by building SW simulators using c/c++
- Evaluate the accuracy of the operation on multiple criteria(ex.error rate)
- Evaluation by applying algorithms that require additional computation(ex. Video processing)

## Benefit
- Develop a new low-power energy high-efficiency accelerator and apply it to various artificial intelligence algorithms including artificial intelligence processors to build an efficient artificial intelligence platform.
- Additive operations are basic operations, so they are applied to various algorithms such as image/voice processing, communication, and diphrers that require them

## Results
#### 1. Design Low-power Approximate Floating Point Adder
We make accutrate floating point adder with C++ based on IEEE 754. As a result of 5 million tests, the accuracy is 100%.
Then we applied it to LOA and ETA1. And based on these activities and other related studies, we designed low-power approximate floating point adder.
##### Fp.cpp
accutrate floating point adder with C++ based on IEEE 754

#### 2. Design Approximate Adder with increased accuracy
##### FP_ETA1, FP_LOA
ETA1, LOA with FP.cpp
##### AX_adder.cpp
our main goal. we make revising 'normalize'

#### 3. Evaluation
