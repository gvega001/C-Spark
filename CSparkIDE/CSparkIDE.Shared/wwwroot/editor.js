window.initializeMonaco = function () {
    require.config({ paths: { 'vs': 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.40.0/min/vs' } });
    require(["vs/editor/editor.main"], function () {
        window.editor = monaco.editor.create(document.getElementById("monacoEditor"), {
            value: "// Start coding...",
            language: "csharp",
            theme: localStorage.getItem("editorTheme") || "vs-dark",
            fontSize: parseInt(localStorage.getItem("editorFontSize") || "14")
        });
    });
};

window.updateConsoleVisibility = function (isVisible) {
    const consoleDiv = document.querySelector(".console-panel");
    if (consoleDiv) {
        consoleDiv.style.display = isVisible ? "block" : "none";
    }
};
