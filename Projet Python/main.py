from calc_parser import parse_and_interpret
import sys

def run_file(filename):
    try:
        with open(filename, 'r') as f:
            text = f.read()
        parse_and_interpret(text)
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found")
    except Exception as e:
        print(f"Error: {e}")

def run_interactive():
    print("Simple Calculator Language Interpreter")
    print("Type 'exit' to quit")
    print("Note: Make sure to include spaces around operators and '=' signs")
    
    buffer = ""
    interpreter = None  # <-- Add this line

    while True:
        try:
            text = input('calc> ')
            if text.lower() == 'exit':
                break

            buffer += text + "\n"

            # Use persistent interpreter
            if interpreter is None:
                from calc_parser import Interpreter
                interpreter = Interpreter()
            result = parse_and_interpret(buffer, interpreter=interpreter)

            if result is not None:
                buffer = ""
        except EOFError:
            break
        except KeyboardInterrupt:
            print("\nInterrupted")
            buffer = ""
        except Exception as e:
            print(f"Error: {e}")
            buffer = ""  # Clear buffer on error to avoid cascading errors

if __name__ == '__main__':
    if len(sys.argv) > 1:
        run_file(sys.argv[1])
    else:
        run_interactive()