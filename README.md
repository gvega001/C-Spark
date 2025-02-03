# C-Spark

## 🚀 Overview
C-Spark is a structured programming language and IDE designed to introduce students to programming concepts in a fun and accessible way. The IDE supports C-Spark development and provides tools for writing, debugging, and testing code. The project consists of two main components:

- **C-Spark Core** – The transpiler and execution engine for the C-Spark language.
- **C-Spark IDE** – A .NET MAUI-based cross-platform IDE built with Visual Studio Community.

---

## 📌 Features
### **C-Spark Language & Transpiler**
- Converts C-Spark code to C for execution.
- Implements syntax validation, type checking, and AST-based code analysis.
- Supports debugging tools such as variable inspection and step-through execution.

### **C-Spark IDE (Built with .NET MAUI)**
- Cross-platform support (Windows, macOS, Linux, Web).
- Real-time error checking and debugging.
- Gamification system for student engagement.
- Git integration for version control and collaboration.

---

## 🛠 Installation & Setup
### **Requirements**
- **Visual Studio Community (Latest Version)**
- **.NET 9+** (for MAUI development)
- **Clang/LLVM** (for compiling C-Spark code)

### **Setup Steps**
1. **Clone the repository**
   ```sh
   git clone https://github.com/gvega001/C-Spark.git
   cd C-Spark
   ```
2. **Open in Visual Studio**
   - Open `C-Spark.sln` in **Visual Studio Community**.
   - Install required **NuGet packages** if prompted.
3. **Run the IDE**
   - Set `C-Spark.UI` as the startup project.
   - Click **Run** (▶) to launch the IDE.

---

## 🔬 Testing Guidelines
### **C-Spark Core (C Code Testing)**
- All C-Spark transpiler and execution engine tests must pass **Clang-Tidy** static analysis.
- Memory safety must be validated using **AddressSanitizer**.
- Code must be formatted using **Clang-Format (Google style)**.
- Execute unit tests for transpiler logic.

### **C-Spark IDE (MAUI Testing)**
- Only **unit tests** apply for the MAUI application.
- Use **xUnit/MSTest** to validate IDE features.
- UI testing is handled separately and is **not mandatory** for contributors.

---

## 📜 Contributing
We welcome contributions! Please follow the guidelines in [CONTRIBUTING.md](CONTRIBUTING.md) before submitting any changes.

---

## 📞 Support & Issues
- **Report Bugs**: Open an [Issue](https://github.com/gvega001/C-Spark/issues) on GitHub.
- **Discussions**: Start a discussion in the GitHub **Discussions** tab.
- **Community Support**: Join our Discord server (Coming Soon!)

---

## 📜 License
This project is licensed under the **GPL-3.0** license. See [LICENSE.txt](LICENSE.txt) for details.

---

🚀 Happy coding with C-Spark! 🎯


