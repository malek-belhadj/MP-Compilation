import ply.lex as lex

# List of token names
tokens = (
    'NUMBER',
    'PLUS', 'MINUS', 'TIMES', 'DIVIDE',
    'LPAREN', 'RPAREN',
    'ID',  # Variable names
    'EQUALS',  # Assignment
    'IF', 'THEN', 'ELSE', 'ENDIF',
    'FOR', 'TO', 'DO', 'ENDFOR',
    'WHILE', 'ENDWHILE',
    'PRINT',
    'GT', 'LT', 'GE', 'LE', 'EQ', 'NE',  # Comparisons
    'COMMA',
    'STRING',
)

# Regular expression rules for simple tokens
t_PLUS = r'\+'
t_MINUS = r'-'
t_TIMES = r'\*'
t_DIVIDE = r'/'
t_LPAREN = r'\('
t_RPAREN = r'\)'
t_EQUALS = r'='
t_GT = r'>'
t_LT = r'<'
t_GE = r'>='
t_LE = r'<='
t_EQ = r'=='
t_NE = r'!='
t_COMMA = r','

# Reserved words
reserved = {
    'if': 'IF',
    'then': 'THEN',
    'else': 'ELSE',
    'endif': 'ENDIF',
    'for': 'FOR',
    'to': 'TO',
    'do': 'DO',
    'endfor': 'ENDFOR',
    'while': 'WHILE',
    'endwhile': 'ENDWHILE',
    'print': 'PRINT',
}

# Rule for numbers
def t_NUMBER(t):
    r'\d+'
    t.value = int(t.value)
    return t

# Rule for identifiers and reserved words
def t_ID(t):
    r'[a-zA-Z_][a-zA-Z0-9_]*'
    t.type = reserved.get(t.value.lower(), 'ID')
    return t

# Rule for string literals
def t_STRING(t):
    r'"[^"]*"'
    t.value = t.value[1:-1]  # Remove the quotes
    return t

# Ignored characters (whitespace)
t_ignore = ' \t'

# Define a rule so we can track line numbers
def t_newline(t):
    r'\n+'
    t.lexer.lineno += len(t.value)

# Rule for comments
def t_COMMENT(t):
    r'\#.*'
    pass  # No return value, token is discarded

# Error handling rule
def t_error(t):
    print(f"Lexical error: Illegal character '{t.value[0]}' at line {t.lexer.lineno}")
    t.lexer.skip(1)

# Build the lexer
lexer = lex.lex()

def get_tokens(data):
    """Test function to show the tokens"""
    lexer.input(data)
    tokens = []
    while True:
        tok = lexer.token()
        if not tok:
            break
        tokens.append(tok)
    return tokens