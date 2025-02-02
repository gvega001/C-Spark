C-Spark: A Beginner-Friendly, High-Powered Programming Language 🚀

Introduction

C-Spark is a modern programming language designed to make coding accessible without sacrificing rigor. It bridges the gap between beginner-friendly syntax and real-world programming concepts, making it an ideal tool for students, educators, and developers who want to learn the fundamentals of programming while working with a language that translates into C.

Features

🔹 Lexer & Parser

Efficient tokenization and abstract syntax tree (AST) generation.

Supports variables, loops, conditionals, and functions.

🔹 Transpiler

Converts C-Spark code into C for execution.

Optimized for efficiency and scalability.

🔹 Error Handling & Debugging

Inline hints provide real-time error messages and suggestions.

Helps students understand common syntax mistakes.

🔹 Gamification & Achievements

Interactive learning through a built-in achievements system.

Encourages students to progress through coding challenges.

🔹 Scalability & Future Expansion

Designed with modular components to support future extensions.

Planned support for Python transpilation.

Installation & Setup

Prerequisites

GCC or Clang (for compiling C output).

Git (to clone the repository).

Visual Studio Code or any C-supported IDE.

Installation Steps

Clone the repository:

git clone https://github.com/gvega001/C-Spark.git
cd C-Spark

Build the project:

make

Run the C-Spark Interpreter:

./c-spark <your_script.cspark>

Usage

Example Code

Here’s a simple C-Spark program:

let x = 10;
let y = 20;
print(x + y);

After transpilation, it generates the following C output:

#include <stdio.h>
int main() {
    int x = 10;
    int y = 20;
    printf("%d\n", x + y);
    return 0;
}

Contributing

🚀 Contributions are welcome! Follow these steps:

Fork the repository.

Create a new branch: git checkout -b feature-branch

Commit your changes: git commit -m "Added a new feature"

Push to GitHub: git push origin feature-branch

Submit a Pull Request 🚀

Roadmap & Future Plans

✅ Finalizing IDE integration
✅ Expanding transpiler support to Python
✅ Building a visual debugger
✅ Enhancing educational debugging aids
✅ Debugging

License

This project is open-source under the MIT License.
