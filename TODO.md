## WARNING These are very important notes, you you must not violate these directions

VSCode terminal crashes when using bash commands - avoid executing bash commands directly
Use the runme script in src directory for any terminal commands instead
Keep TODO.md updated with current progress so work is not lost when it crashes
Wait for user's explicit instruction before continuing the task
After updating runme with commands, ask user to run it

## Current Task Status
Implementing function support in BeebAsm. Need to:

---------------------------------------------------------------------------------------

## Current task

1. Fix duplicate code in expression.cpp - DONE
2. Implement ExecuteFunctionCall() method - DONE  
3. Build and test - IN PROGRESS

---------------------------------------------------------------------------------------

# BeebAsm Function Feature Implementation TODO

## Overview
Implement function support in BeebAsm with the following syntax:

FUNCTION name <param>
  RETURN value or string etc
ENDFUNCTION

## Features
- Functions can take zero or more parameters after their name
- No assembly or other memory changing is allowed inside the function
- Functions can be nested
- Functions can return numeric values or strings

## Calling Syntax Examples
startNumber=prng()
biggest=max(20,15)
total=sum(10,10,20)
final=sum(sum(5,5,5), sum(sum(1,1,sum(1,1,1)),5,6),2,1)

## Implementation Tasks

### Phase 1: Core Infrastructure
- [x] Add FUNCTION and ENDFUNCTION to token table (commands.cpp)
- [x] Add RETURN to token table (commands.cpp)
- [x] Create handler methods in LineParser
- [x] Add function symbol table management

### Phase 2: Function Definition Parsing
- [x] Parse FUNCTION token and extract function name
- [x] Parse function parameters (zero or more)
- [x] Store function body for later execution

### Phase 3: Function Call Evaluation (IN PROGRESS)
- [x] Detect function calls in expressions
- [x] Parse function arguments using ParseFunctionArguments()
- [x] Implement nested function call support
- [x] Implement RETURN statement evaluation

### Phase 4: Value Types
- [x] Support returning numeric values
- [x] Support returning string values

## Files Modified
- src/expression.cpp - implemented ParseFunctionArguments()
- src/lineparser.h - added ParseFunctionArguments() declaration

## Current Status
Fixed the original "Mismatched parentheses" error. 

CURRENT BUG: Functions return 0 instead of correct values.

## Issue Details
The function call sum(1, 2, 3) returns 0 instead of 6.
The function max(20, 15) returns 0 instead of 20.

ROOT CAUSE: In ExecuteFunctionCall(), when processing function body with Process(m_line), the assignment statements are not working correctly.

## Next Fix Needed
Need to fix ExecuteFunctionCall() to properly handle assignment statements like "result = a + b + c" inside the function body.
