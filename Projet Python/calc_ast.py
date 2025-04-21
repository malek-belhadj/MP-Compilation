class Node:
    pass

class Number(Node):
    def __init__(self, value):
        self.value = value
        
    def __repr__(self):
        return f"Number({self.value})"

class BinOp(Node):
    def __init__(self, left, op, right):
        self.left = left
        self.op = op
        self.right = right
        
    def __repr__(self):
        return f"BinOp({self.left}, {self.op}, {self.right})"

class UnaryOp(Node):
    def __init__(self, op, expr):
        self.op = op
        self.expr = expr
        
    def __repr__(self):
        return f"UnaryOp({self.op}, {self.expr})"

class Assign(Node):
    def __init__(self, name, value):
        self.name = name
        self.value = value
        
    def __repr__(self):
        return f"Assign({self.name}, {self.value})"

class Var(Node):
    def __init__(self, name):
        self.name = name
        
    def __repr__(self):
        return f"Var({self.name})"

class Condition(Node):
    def __init__(self, left, op, right):
        self.left = left
        self.op = op
        self.right = right
        
    def __repr__(self):
        return f"Condition({self.left}, {self.op}, {self.right})"

class If(Node):
    def __init__(self, condition, then_block, else_block=None):
        self.condition = condition
        self.then_block = then_block
        self.else_block = else_block
        
    def __repr__(self):
        return f"If({self.condition}, {self.then_block}, {self.else_block})"

class For(Node):
    def __init__(self, var_name, start_value, end_value, body):
        self.var_name = var_name
        self.start_value = start_value
        self.end_value = end_value
        self.body = body
        
    def __repr__(self):
        return f"For({self.var_name}, {self.start_value}, {self.end_value}, {self.body})"

class While(Node):
    def __init__(self, condition, body):
        self.condition = condition
        self.body = body
        
    def __repr__(self):
        return f"While({self.condition}, {self.body})"

class Print(Node):
    def __init__(self, expr):
        self.expr = expr
        
    def __repr__(self):
        return f"Print({self.expr})"

class String(Node):
    def __init__(self, value):
        self.value = value
        
    def __repr__(self):
        return f"String({self.value})"

class Program(Node):
    def __init__(self, statements):
        self.statements = statements
        
    def __repr__(self):
        return f"Program({self.statements})"