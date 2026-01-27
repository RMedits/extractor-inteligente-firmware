
def check_braces(filepath):
    with open(filepath, 'r') as f:
        lines = f.readlines()

    stack = []
    for i, line in enumerate(lines):
        for char in line:
            if char == '{':
                stack.append(i + 1)
            elif char == '}':
                if not stack:
                    print(f"Error: Unmatched '}}' at line {i + 1}")
                    return
                stack.pop()

    if stack:
        print(f"Error: Unmatched '{{' at lines: {stack}")
    else:
        print("Braces are balanced.")

if __name__ == "__main__":
    check_braces("src/main.cpp")
