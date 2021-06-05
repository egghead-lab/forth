# Forth machine with C/C++ API
This is a FORTH interpreter with API to C/C++ language. It was developed in the project of 
training AI to use a Turing-complete language for programming; there are two reasons for 
this choice:
- postfix notation of algebraic expressions, which seems genuine to AI that's free from the 
common sense of humans
- simple syntax for definition of new words, which commonly leads to bottom-to-top programming
style - something that correlates with the approach of training combinatorial AI from simple 
to complex.

Discontinued because of lack of specification in the original FORTH, especially in the priorities 
of words used for branching, looping and ';' that closes words definitions.

You can try any string as a FORTH program calling that from C/C++ code (see the unit test for 
examples). To have some fun with the interpreter in manual mode, type WORDS from the console.

### author
(a) pavelmalyshkin
