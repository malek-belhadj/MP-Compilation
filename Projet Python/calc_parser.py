import ply.yacc as yacc
from calc_lexer import tokens
from calc_ast import *
from symbol_table import SymbolTable

# Parsing rules
def p_program(p):
    '''program : statement
               | program statement'''
    if len(p) == 2:
        p[0] = Program([p[1]]) if p[1] is not None else Program([])
    else:
        if p[2] is not None:  # Make sure we don't append None statements
            p[1].statements.append(p[2])
        p[0] = p[1]

def p_statement(p):
    '''statement : assignment
                 | if_statement
                 | for_statement
                 | while_statement
                 | print_statement'''
    p[0] = p[1]

def p_assignment(p):
    'assignment : ID EQUALS expression'
    p[0] = Assign(p[1], p[3])

def p_if_statement(p):
    '''if_statement : IF condition THEN program ENDIF
                    | IF condition THEN program ELSE program ENDIF'''
    if len(p) == 6:
        p[0] = If(p[2], p[4])
    else:
        p[0] = If(p[2], p[4], p[6])

def p_for_statement(p):
    'for_statement : FOR ID EQUALS expression TO expression DO program ENDFOR'
    p[0] = For(p[2], p[4], p[6], p[8])

def p_while_statement(p):
    'while_statement : WHILE condition DO program ENDWHILE'
    p[0] = While(p[2], p[4])

def p_print_statement(p):
    'print_statement : PRINT LPAREN expression RPAREN'
    p[0] = Print(p[3])

def p_condition(p):
    '''condition : expression GT expression
                 | expression LT expression
                 | expression GE expression
                 | expression LE expression
                 | expression EQ expression
                 | expression NE expression'''
    p[0] = Condition(p[1], p[2], p[3])

def p_expression_binop(p):
    '''expression : expression PLUS expression
                  | expression MINUS expression
                  | expression TIMES expression
                  | expression DIVIDE expression'''
    p[0] = BinOp(p[1], p[2], p[3])

def p_expression_group(p):
    'expression : LPAREN expression RPAREN'
    p[0] = p[2]

def p_expression_number(p):
    'expression : NUMBER'
    p[0] = Number(p[1])

def p_expression_string(p):
    'expression : STRING'
    p[0] = String(p[1])

def p_expression_name(p):
    'expression : ID'
    p[0] = Var(p[1])

def p_expression_uminus(p):
    'expression : MINUS expression %prec UMINUS'
    p[0] = UnaryOp('-', p[2])

# Error rule for syntax errors
def p_error(p):
    if p:
        print(f"Syntax error at '{p.value}', line {p.lineno}")
    else:
        print("Syntax error at EOF")

# Precedence rules
precedence = (
    ('left', 'PLUS', 'MINUS'),
    ('left', 'TIMES', 'DIVIDE'),
    ('right', 'UMINUS'),
)

# Build the parser
parser = yacc.yacc()

# Interpreter class
class Interpreter:
    def __init__(self):
        self.symbol_table = SymbolTable()
        
    def visit(self, node):
        if node is None:  # Add a check for None nodes
            return None
            
        method_name = f'visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self.generic_visit)
        return visitor(node)
        
    def generic_visit(self, node):
        raise NotImplementedError(f"No visit_{type(node).__name__} method defined")
        
    def visit_Program(self, node):
        results = []
        for statement in node.statements:
            if statement:  # Make sure we don't visit None statements
                result = self.visit(statement)
                if result is not None:
                    results.append(result)
        return results
        
    def visit_Number(self, node):
        return node.value
        
    def visit_String(self, node):
        return node.value
        
    def visit_BinOp(self, node):
        left = self.visit(node.left)
        right = self.visit(node.right)
        
        if node.op == '+':
            return left + right
        elif node.op == '-':
            return left - right
        elif node.op == '*':
            return left * right
        elif node.op == '/':
            if right == 0:
                raise ZeroDivisionError("Division by zero")
            return left / right
            
    def visit_UnaryOp(self, node):
        value = self.visit(node.expr)
        if node.op == '-':
            return -value
            
    def visit_Assign(self, node):
        value = self.visit(node.value)
        self.symbol_table.set(node.name, value)
        return value
        
    def visit_Var(self, node):
        try:
            return self.symbol_table.get(node.name)
        except NameError as e:
            print(f"Semantic error: {e}")
            return 0  # Return a default value to allow execution to continue
            
    def visit_Condition(self, node):
        left = self.visit(node.left)
        right = self.visit(node.right)
        
        if node.op == '>':
            return left > right
        elif node.op == '<':
            return left < right
        elif node.op == '>=':
            return left >= right
        elif node.op == '<=':
            return left <= right
        elif node.op == '==':
            return left == right
        elif node.op == '!=':
            return left != right
            
    def visit_If(self, node):
        condition = self.visit(node.condition)
        if condition:
            return self.visit(node.then_block)
        elif node.else_block:
            return self.visit(node.else_block)
        return None
            
    def visit_For(self, node):
        start_value = self.visit(node.start_value)
        end_value = self.visit(node.end_value)
        
        results = []
        for i in range(start_value, end_value + 1):
            self.symbol_table.set(node.var_name, i)
            result = self.visit(node.body)
            if result is not None:  # Make sure we don't append None results
                results.append(result)
        return results
        
    def visit_While(self, node):
        results = []
        while self.visit(node.condition):
            result = self.visit(node.body)
            if result is not None:  # Make sure we don't append None results
                results.append(result)
        return results
        
    def visit_Print(self, node):
        value = self.visit(node.expr)
        print(value)
        return value

def parse_and_interpret(text, interpreter=None):
    """Parse and interpret the input text"""
    try:
        ast = parser.parse(text)
        if ast is None:  # If parsing fails completely
            print("Failed to parse input")
            return None

        if interpreter is None:
            interpreter = Interpreter()
        return interpreter.visit(ast)
    except Exception as e:
        print(f"Error during interpretation: {e}")
        import traceback
        traceback.print_exc()  # This will help debugging by showing the full error
        return None