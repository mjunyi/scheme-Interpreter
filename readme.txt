Our interpreter runs according to specifications.  We also implemented all
optional material listed below.  Our interpreter has a couple of unique
implementations.  We created a method to handle procedures that can take
either ints or doubles.  Our tokenizer has a shorter comparison because it uses
arrays.

proj5: Eval
Handle (if test consequent) variant that is missing alternate.

proj6: Apply
Support multiple expressions in the body of lambda.
Support multiple expressions in the body of let.

proj7: Primitives
Handles + correct return types

proj8: Interpreter
* has correct behaviour for int and doubles
/ has correct behaviour - can take multiple arguments
- has correct behaviour for int and doubles
loads functions in lists.scm before running (currently map and not).
(load filename*) loads the filename with the first string given, and executes it silently.
