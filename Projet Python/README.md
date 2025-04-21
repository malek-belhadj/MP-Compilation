# Simple Calculator Language Interpreter

A simple programming language interpreter built with Python and PLY (Python Lex-Yacc). This project implements a basic calculator language with variables, control structures, and basic operations.

## Project Overview

This interpreter provides a simple programming language with the following features:

- Variable assignment and usage
- Arithmetic operations (addition, subtraction, multiplication, division)
- Comparison operators (>, <, >=, <=, ==, !=)
- Conditional statements (if-then-else)
- Loop structures (for and while loops)
- Print statements
- String support

## Project Structure

- **calc_lexer.py**: Defines the lexical analyzer (tokenizer) using PLY's lex module
- **calc_ast.py**: Contains the Abstract Syntax Tree node classes
- **calc_parser.py**: Implements the parser and interpreter using PLY's yacc module
- **symbol_table.py**: Manages variable storage and retrieval
- **main.py**: Entry point for the interpreter (interactive mode and file execution)
- **examples/**: Directory containing example programs

## Language Syntax

### Variables and Expressions

```
# Variable assignment
x = 10
y = 5

# Arithmetic operations
z = x + y * 2  # Follows standard operator precedence
w = (x + y) * 2  # Parentheses for grouping
```

### Conditional Statements

```
# If-then structure
if x > 5 then
    print("x is greater than 5")
endif

# If-then-else structure
if x > 10 then
    print("x is greater than 10")
else
    print("x is not greater than 10")
endif
```

### Loop Structures

```
# For loop
for i = 1 to 5 do
    print(i)
endfor

# While loop
counter = 3
while counter > 0 do
    print(counter)
    counter = counter - 1
endwhile
```

### Print Statements

```
print("Hello, World!")
print(x + y)
```

## How to Run

### Interactive Mode

Run the interpreter in interactive mode:

```
python main.py
```

This will start a REPL (Read-Eval-Print Loop) where you can enter statements line by line.

### File Execution

Execute a program from a file:

```
python main.py examples/test1.calc
```

## Implementation Details

### Lexical Analysis

The lexer (`calc_lexer.py`) uses PLY's lex module to define tokens for the language, including:
- Numbers and strings
- Operators (+, -, *, /, =, >, <, etc.)
- Keywords (if, then, else, for, while, etc.)
- Identifiers (variable names)

### Parsing and AST

The parser (`calc_parser.py`) uses PLY's yacc module to define grammar rules and build an Abstract Syntax Tree (AST). The AST nodes are defined in `calc_ast.py` and represent different language constructs.

### Interpretation

The interpreter walks the AST using the visitor pattern, executing each node according to its semantics. The `Interpreter` class in `calc_parser.py` contains methods to visit and evaluate each type of AST node.

### Symbol Table

The `SymbolTable` class in `symbol_table.py` manages variable storage and retrieval during program execution.

## Example Program

```
# This is a test program
x = 10
y = 5

# Calculate some values
z = x + y * 2

# Conditional statement
if z > 20 then
    print("z is greater than 20")
else
    print("z is not greater than 20")
endif

# Loop structure
for i = 1 to 5 do
    print(i)
endfor

# While loop example
counter = 3
while counter > 0 do
    print("Countdown: ")
    print(counter)
    counter = counter - 1
endwhile

print("Done!")
```

## Notes

- The language is case-insensitive for keywords
- Make sure to include spaces around operators and '=' signs
- Comments start with '#' and continue to the end of the line