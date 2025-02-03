Sure! I can help format the content to make it more readable and visually appealing.

---

# Contributing to C-Spark

Thank you for being so interested in contributing to C-Spark! 🚀 Please follow the guidelines below to ensure smooth collaboration.

## 📌 Branching Strategy

We follow a structured Git Workflow:

- **main**: Stable production-ready branch.
- **dev**: Active development branch.
- **feature/\***: Use for new features (e.g., feature/new-parser).
- **hotfix/\***: For urgent bug fixes.

**Always create a feature branch from `dev` and open a PR for review.**

## 📌 Pull Request Guidelines

- Use a meaningful branch name (e.g., `feature/bug-fix-name`).
- Follow commit message format:
  - `feat`: Add feature XYZ
  - `fix`: Resolve memory leak in parser
  - `docs`: Update README with new setup steps
- Open a pull request (PR) from your feature branch to `dev`.
- At least 1 approval is required before merging.
- CI checks must pass before merging, except when overridden by an admin.

## 📌 Code Quality & Standards

### C-Spark Core (C Code) Requirements:

- Code must pass Clang-Tidy static analysis.
- Memory safety must be validated using AddressSanitizer.
- Code must be formatted using Clang-Format (Google style).
- Execute unit tests for transpiler logic.

### C-Spark IDE (MAUI) Requirements:

- Only unit tests must pass before merging.
- UI testing is not required unless explicitly stated.
- Admins may override test failures if necessary.

## 📌 Local Development Setup

1. **Clone the repository:**
   ```bash
   git clone https://github.com/gvega001/C-Spark.git
   cd C-Spark
   ```

2. **Create a feature branch:**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Run Clang-Tidy checks:**
   ```bash
   clang-tidy --checks=* source.cpp
   ```

4. **Run Sanitizer tests:**
   ```bash
   clang++ -fsanitize=address,undefined -fno-omit-frame-pointer main.cpp
   ./a.out
   ```

5. **Format the code:**
   ```bash
   clang-format -style=Google -i *.cpp
   ```

## 📌 Contact & Issues

- **Found a bug?** Open an Issue in GitHub.
- **Have a question?** Start a Discussion in GitHub.

Happy coding! 🚀
