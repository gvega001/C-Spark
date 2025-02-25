# 🤖 C-Spark Visual Studio Copilot Instructions

> 📁 **Location:** `.copilot/COPILOT_INSTRUCTIONS.md`
> 🏗 **Template:** .NET MAUI Blazor Hybrid and Web App Solution
> 🛠 **Solution Structure:** CSparkIDE (MAUI), CSparkIDE.Shared (RCL), CSparkIDE.Web (Blazor Server), CSparkIDE.Web.Client (Blazor WASM)

---

## 📘 Project Overview
C-Spark is a cross-platform IDE that uses the **.NET MAUI Blazor Hybrid** template with support for native desktop/mobile apps and web deployment. It leverages a shared **Razor Class Library (RCL)** and supports future WebAssembly (WASM) through the **CSparkIDE.Web.Client**.

### 📊 **Tech Stack:**
- **Frontend (UI):**
  - **.NET MAUI** for native apps (Windows, macOS, iOS, Android)
  - **Blazor Web App** for browser-based IDE
  - **Blazor WASM** (Future) for client-side execution
  - **Razor Class Library (RCL)** for shared components
- **Code Editors:** Monaco or Ace
- **Backend Services:** Lexer, Parser, Transpiler, Execution Engine
- **Database:** SQLite with GitHub Integration
- **Containerization:** Docker + Kubernetes for scalability

---

## 💡 Best Practices for Copilot
1. **Use the Right Project for the Task:**
   - **CSparkIDE** → Native app logic (MAUI)
   - **CSparkIDE.Shared** → Razor components, shared UI
   - **CSparkIDE.Web** → Blazor Web App (Server-Side)
   - **CSparkIDE.Web.Client** → WASM (Future web client)

2. **Leverage Blazor Render Modes:**
   - **Hybrid** for desktop/mobile.
   - **Blazor Server** for web app.
   - **Blazor WASM** for future lightweight web clients.

3. **Use MVVM Architecture in MAUI Projects.**
4. **Utilize Dependency Injection (DI) across all layers.**

---

## 🏗️ Solution Breakdown

### 💻 **1. CSparkIDE (MAUI Hybrid App)**
- **Purpose:** Native App (Windows/macOS/Linux/iOS/Android)
- **Technologies:** .NET MAUI, Blazor Hybrid
- **Features:**
  - Monaco/Ace Code Editor Integration
  - Inline Debugging
  - Syntax Checker using Language Server Protocol (LSP)
  - Local Storage with SQLite

---

### 🌐 **2. CSparkIDE.Web (Blazor Web App)**
- **Purpose:** Browser-based C-Spark IDE
- **Technologies:** Blazor Server
- **Features:**
  - Same Razor components as MAUI (via Shared RCL)
  - SignalR for real-time collaboration
  - Potential future WASM client support

---

### 🧱 **3. CSparkIDE.Shared (Razor Class Library)**
- **Purpose:** Shared UI components across all platforms
- **Technologies:** Blazor Components, MudBlazor, Radzen
- **Features:**
  - Buttons, Modals, Editors, Forms
  - Theming (Dark/Light Mode)
  - Reusable Code Snippets

---

### 🚀 **4. CSparkIDE.Web.Client (Blazor WASM - Future)**
- **Purpose:** Client-Side Blazor App for Browser (WebAssembly)
- **Technologies:** Blazor WASM
- **Planned Features:**
  - Offline-first web support
  - Client-side execution using WebAssembly

---

## 🧪 **Testing Strategy**
- **Unit Testing:** Lexer, Parser, AST Generation (xUnit)
- **Integration Testing:** Transpiler, Execution Engine
- **UI Testing:** Blazor components using **bUnit**
- **Performance Testing:** AST Caching & Real-Time Syntax Checker

---

## 🚀 **Deployment Workflow**
1. **Local Dev:** `.NET 9` SDK → MAUI + Blazor Hybrid
2. **CI/CD:** GitHub Actions → Docker → Kubernetes
3. **Cloud:** Azure/AWS for Web App Hosting + DB
