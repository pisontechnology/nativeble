param (
    [Alias('c')]
    [switch]$clean = $false,
    [validateset('x86','x64')]
    [Alias('a')]
    [string]$arch = "x64" # x86, x64
)

[string]$PROJECT_ROOT = Resolve-Path $($PSScriptRoot + "\..\..\..")
[string]$VSWHERE_PATH = Resolve-Path $($PROJECT_ROOT + "\toolchains\windows\tools\vswhere.exe")
# -products flag allows the system to also search for users who installed Visual Studio Build Tools 2019 (https://github.com/3F/hMSBuild/issues/12)
[string]$MSBUILD_PATH = & $VSWHERE_PATH -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe -products * | select-object -first 1
[string]$TARGET = "Release"

if ($null -eq $MSBUILD_PATH) {
    Write-Error "MSBuild path not found."
    exit -1
}

# Validate the received architecture
switch -regex ($arch) {
  'x86' {
        [string]$OUTPUT_PATH = "windows-x86"
        [string]$WINDOWS_ARCH = "Win32"
    }
  'x64' {
        [string]$OUTPUT_PATH = "windows-x64"
        [string]$WINDOWS_ARCH = "x64"
    }
}


# Clean directories if needed and recreate the necessary folder structure.
if ($clean) {
    Remove-Item -Path "$PROJECT_ROOT\bin\$OUTPUT_PATH" -Force -Recurse -ErrorAction SilentlyContinue
    Remove-Item -Path "$PROJECT_ROOT\build\$OUTPUT_PATH\*" -Force -Recurse -ErrorAction SilentlyContinue
}
New-Item -ItemType Directory -Force -Path "$PROJECT_ROOT\build\$OUTPUT_PATH" | Out-Null
New-Item -ItemType Directory -Force -Path "$PROJECT_ROOT\bin\$OUTPUT_PATH" | Out-Null

# Run CMake to create our build files.
cmake -S "$PROJECT_ROOT" -B "$PROJECT_ROOT\build\$OUTPUT_PATH\" -A $WINDOWS_ARCH

# Run the compiler
& $MSBUILD_PATH "$PROJECT_ROOT\build\$OUTPUT_PATH\NATIVEBLE.sln" -nologo -m:8 /bl:output.binlog /verbosity:minimal /p:Configuration=$TARGET

#Copy all generated files to the bin folder for consistency and remove the output folder.
Copy-item -Force -Recurse "$PROJECT_ROOT\bin\$OUTPUT_PATH\$TARGET\*" -Destination "$PROJECT_ROOT\bin\$OUTPUT_PATH\"
#Remove-Item -Path "$PROJECT_ROOT\bin\$TARGET" -Force -Recurse -ErrorAction SilentlyContinue

Compress-Archive -Path "$PROJECT_ROOT\bin\$OUTPUT_PATH\$TARGET\*" -DestinationPath "$PROJECT_ROOT\bin\$OUTPUT_PATH\$OUTPUT_PATH" -Force