class SymbolTable:
    def __init__(self):
        self.symbols = {}
        
    def get(self, name):
        if name in self.symbols:
            return self.symbols[name]
        else:
            raise NameError(f"Variable '{name}' is not defined")
            
    def set(self, name, value):
        self.symbols[name] = value
        return value
        
    def __repr__(self):
        return str(self.symbols)