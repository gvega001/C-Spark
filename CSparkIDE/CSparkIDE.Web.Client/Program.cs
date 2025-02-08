using CSparkIDE.Shared.Services;
using CSparkIDE.Web.Client.Services;
using Microsoft.AspNetCore.Components.WebAssembly.Hosting;

var builder = WebAssemblyHostBuilder.CreateDefault(args);

// Add device-specific services used by the CSparkIDE.Shared project
builder.Services.AddSingleton<IFormFactor, FormFactor>();

await builder.Build().RunAsync();
