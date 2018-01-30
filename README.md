# ECTR Foundations Library

**E**merging **C**omputing **T**echnology **L**aboratory foundations library
is a library designed in Mordern C++ to support research on EDA and designs.

## Philosophy

### Correctness is prime concern.

For a very long time the research community lacks easy tools accelerate
the research process. Since the prime purpose of this library is to support
EDA research, correctness is our prime conern. A research that is not reproducable
is nonesence. We aim to use mordern language constructs to facilitate correctness.
This includes:

- Use type systems to differentiate different representations of the same network,
make sure that algorithms are run only on the correct representations. Make clear
the cost related to transforming different representations.

- Use design patterns to facilitate reuse and up-to-date attributes.

- Include as much of compile time checks as possible. Use const if possible, make
sure the algorithm does not accidentally change anything. Help detect errors as 
soon as possible. 

### Expressiveness is essential.
The second goal of this library to utilize modern to tools to make the user
code readable. We believe a research product easily understandable by most is 
more likely to be accepted. It will also help develop further on others' work.
We believe expressiveness is important because we would like the researcher
to focus on expressing their ideas, other than spend tons of tons of hours to
fight pointers and other issues. We also this library will help researchers to
quickly prototype their ideas, and help new comers to focus on the bigger pictures
other than trying to struggle through a large code base.

### Performance as an option.

> Premature optimization is the root of all evils - Donald Knuth

We strongly believe in this very famous quote. Our library does not aim for 
extreme options. Instead, whenever expressiveness or ease of use is in conlict
with better performance (which is very often), our design philosophy almost
always take the expressiveness side. However, we do care about performance. We
choose to design the library in a way that you are almost always able to replace
preformance critical part with your own code. Essentially, we believe **performance
is an right that you can exercise on demand**. 


## To Do 
- [x] Extract struct BnetNode, BnetNetwork from nanotrav
- [x] Remove unneeded functions and dependencies of bnet
- [x] Organize bnet in a lib/include structure
- [ ] Simplify the headers for bnet library
- [ ] Create C++ interface for bnet library
- [ ] To be continued
