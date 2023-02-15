$originDir = Get-Location
$projectDir = Split-Path $PSScriptRoot -Parent

function SafeExit {
    param(
        $exitCode  # exit code
    )
    Set-Location $originDir
    exit $exitCode
}

function Write-Success {
    param (
        $InputObject  # input
    )
    Write-Host "$InputObject" -ForegroundColor Green
}

function Write-Failure {
    param (
        $InputObject  # input
    )
    Write-Host "$InputObject" -ForegroundColor Red
}

function local:vsInstanceRoot {
    return Get-ChildItem "C:\Program Files\Microsoft Visual Studio" -Filter 20?? | Sort-Object -Descending
}

function Find-CMakeCommand {
    # environment?
    $cmakePath = Get-Command -Name cmake -ErrorAction SilentlyContinue
    if ($cmakePath) { return $cmakePath }

    # Visual Studio?
    $vsInstances = vsInstanceRoot
    foreach ($instance in $vsInstances) {
        $cmakePath = Get-ChildItem "$instance\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -ErrorAction SilentlyContinue
        if ($cmakePath) { return $cmakePath }
    }

    # Qt CMake?
    $cmakePath = Get-ChildItem "C:\Qt\Tools\CMake_64\bin\cmake.exe" -ErrorAction SilentlyContinue
    if ($cmakePath) { return $cmakePath }
}

function Find-NinjaCommand {
    # environment?
    $ninjaPath = Get-Command -Name ninja -ErrorAction SilentlyContinue
    if ($ninjaPath) { return $ninjaPath }

    $ninjaPath = Find-CMakeCommand | Split-Path -Parent | Join-Path -ChildPath "..\..\Ninja\ninja.exe" -ErrorAction SilentlyContinue
    if ($ninjaPath) { return $ninjaPath }
}

function Find-NpmCommand {
    # environment?
    $npmPath = Get-Command -Name npm -ErrorAction SilentlyContinue
    if ($npmPath) { return $npmPath }

    # Visual Studio?
    $vsInstances = vsInstanceRoot
    foreach ($instance in $vsInstances) {
        $cmakePath = Get-ChildItem "$instance\Community\MSBuild\Microsoft\VisualStudio\NodeJs\npm.cmd" -ErrorAction SilentlyContinue | `
            Split-Path -Parent
        if ($cmakePath) { return $cmakePath }
    }
}

function DownloadChilkat {
    param (
        $downloadPath
    )

    $donetVersion = "48"
    $version = "9.5.0"
    $chilkatUrl = "https://chilkatdownload.com/9.5.0.93/chilkatdotnet$donetVersion-$version-x64.zip"
    $downloadPath = Join-Path $downloadPath -ChildPath "chilkat"
    if (-not (Test-Path $downloadPath)) {
        New-Item -Path $downloadPath -ItemType Directory
    }
    $DownloadFileName = Split-Path -Path $chilkatUrl -Leaf
    $localFilePath = Join-Path $downloadPath -ChildPath $DownloadFileName

    # download
    if (-not (Test-Path $localFilePath)) {
        Invoke-WebRequest -Uri $chilkatUrl -OutFile $localFilePath
    }

    # unzip
    $dllpath = "$downloadPath\ChilkatDotNet$donetVersion-$version-x64\ChilkatDotNet$donetVersion.dll"
    if (-not (Test-Path $dllpath)) {
        Expand-Archive -Path $localFilePath -DestinationPath $downloadPath
    }
    
    if (-not (Test-Path $dllpath)) {
        Write-Failure "$dllpath not found."
        SafeExit 1
    }
    
    Add-Type -Path $dllpath
}