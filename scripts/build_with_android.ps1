. "$PSScriptRoot/helpers.ps1"

Set-Location $projectDir

$cmake = Find-CMakeCommand
Write-Host "cmake path: $cmake"

if (-not $cmake) {
    Write-Failure "CMake is not installed"
    SafeExit 1
}

function Find-JDKPath {
    # environment?
    $jdkPath = (Get-Command -Name java -ErrorAction SilentlyContinue).Source | `
        Split-Path -Parent -ErrorAction SilentlyContinue | `
        Join-Path -ChildPath ".." -ErrorAction SilentlyContinue
    if ($jdkPath) { return $jdkPath }

    $jdkPath = $env:JAVA_HOME
    if ($jdkPath) { return $jdkPath }

    # location
    return "D:/Program Files/jdk-11.0.18+10"
}

function Find-SDKPath {
    #environment?
    $sdkPath = $env:ANDROID_SDK_ROOT
    if ($sdkPath) { return $sdkPath }

    # location
    return "$HOME/AppData/Local/Android/Sdk"
}

function Find-NDKPath {
    #environment?
    $ndkPath = $env:ANDROID_NDK
    if ($ndkPath) { return $ndkPath }

    # location
    return $(Find-SDKPath | Join-Path -ChildPath "ndk/23.1.7779620")
}

function AndroidCustomization {
    $templatePath = Join-Path $PSScriptRoot -ChildPath "android_customization"
    if (-not (Test-Path $templatePath)) {
        Write-Failure "Template file is missing."
        SafeExit 1
    }

    if (-not (Test-Path $androidPath)) {
        New-Item -Path $androidPath -ItemType Directory
    }
    Copy-Item -Path "$templatePath\*" -Destination $androidPath -Recurse -ErrorAction SilentlyContinue

    $xmlpath = Join-Path $androidPath -ChildPath "AndroidManifest.xml"
    $xml = New-Object Chilkat.Xml
    $success = $xml.LoadXmlFile($xmlpath)
    if ($success -ne $true) {
        $($xml.LastErrorText)
        SafeExit 1
    }

    $success = $xml.UpdateAttribute("package", "org.project.launch")
    $success = $xml.UpdateAttribute("android:versionCode", "1")
    $success = $xml.UpdateAttribute("android:versionName", "1.0")

    # uses-permission
    "INTERNET", "WRITE_EXTERNAL_STORAGE", "ACCESS_NETWORK_STATE" | ForEach-Object {
        $xmlnode = $xml.NewChild("uses-permission", "")
        $success = $xmlnode.AddAttribute("android:name", "android.permission.$($_)")
    }

    # application
    $xmlnode = $xml.FindChild("application")
    $success = $xmlnode.UpdateAttribute("android:label", "launch")
    $success = $xmlnode.AddAttribute("android:icon", "@drawable/icon")

    # activity
    $xmlnode = $xmlnode.FindChild("activity")
    $success = $xmlnode.UpdateAttribute("android:label", "launch")

    $metaData = @{lib_name = 'launch'; arguments = ''; extract_android_style = 'minimal' }
    $metaData.GetEnumerator() | ForEach-Object {
        $metanode = $xmlnode.NewChild("meta-data", "")
        $success = $metanode.AddAttribute("android:name", "android.app.$($_.Name)")
        $success = $metanode.AddAttribute("android:value", "$($_.Value)")
    }

    $success = $xml.SaveXml($xmlpath)
    if ($success -ne $true) {
        $($xml.LastErrorText)
        SafeExit 1
    }
}

function GenerateAndroidIcons {
    $iconFrom = "C:\Users\zjczh\Downloads\stretched-3841-2560-1298954.jpg"
    $iconTo = "$(Join-Path $androidPath -ChildPath "res")"
    python "$(Join-Path $PSScriptRoot -ChildPath "generateicon.py")" $iconFrom $iconTo
}

$buildType = if ($args[0]) { $args[0] } else { "Release" }
$buildPath = $projectDir + "/../cmake-build-ikpas-" + $buildType | Resolve-Path
if (-not (Test-Path $buildPath)) {
    New-Item -Path $buildPath -ItemType Directory -ErrorAction SilentlyContinue
}
Set-Location $buildPath
$androidPath = Join-Path $buildPath -ChildPath "android"

DownloadChilkat $buildPath

# helpers fun
$ninjaPath = Find-NinjaCommand
$npmPath = Find-NpmCommand

# location fun
$jdkPath = Find-JDKPath
if (-not (Test-Path "$jdkPath")) {
    Write-Failure "The JDK directory does not exist, please check whether the JDK is installed and configured correctly."
    SafeExit 1
}

$ndkPath = Find-NDKPath
if (-not (Test-Path "$ndkPath")) {
    Write-Failure "The Android NDK directory does not exist, please check whether the Android JDK is installed and configured correctly."
    SafeExit 1
}

$schema = "qsdf-core"
$domain = "IKPAS"
$domainName = "EPDomain_$($domain)"
$androidABI = "arm64-v8a"
$ndkhost = "windows-x86_64"
$derialnumber = "R9TR502PN7J"

# qt path
$qt6root = "D:/Qt/6.4.2/android_$($androidABI -replace "-", "_")"
$qt6libPath = Join-Path $qt6root -ChildPath "/lib/cmake/Qt6"
$qtHostPath = "D:/Qt/6.4.2/mingw_64"

& $cmake `
    -G Ninja -S "$projectDir" `
    -DCMAKE_CONFIGURATION_TYPES:STRING="$buildType" `
    -DCMAKE_BUILD_TYPE:STRING="$buildType" `
    -DQSDF_DOMAIN_NAME:STRING="$domain" `
    -DCMAKE_MAKE_PROGRAM:FILEPATH="$ninjaPath" `
    -DQSDF_BUILD_ANDROID:BOOL=ON `
    -DNpm_ROOT_PATH:PATH="$npmPath" `
    -DJAVA_HOME:PATH="$jdkPath" `
    -DANDROID_SDK_ROOT:PATH="$(Find-SDKPath)" `
    -DANDROID_NDK:PATH="$ndkPath" `
    -DANDROID_ABI:STRING="$androidABI" `
    -DQt6_DIR:PATH="$qt6libPath"

$location = Get-Location
& $cmake --build $location --target all -j 14

# android customization
AndroidCustomization

# generate icon
GenerateAndroidIcons

$json = New-Object Chilkat.JsonObject
$json.EmitCompact = 0
$success = $json.AddStringAt(-1, "description", "This file is generated by cmake to be read by androiddeployqt and should not be modified by hand.")
$success = $json.AddObjectAt(-1, "qt")
$qt = $json.ObjectAt($json.Size - 1)
$success = $qt.AddStringAt(-1, $androidABI, $qt6root)
$success = $json.AddStringAt(-1, "sdk", $(Join-Path $ndkPath -ChildPath "../.." -Resolve))
$success = $json.AddStringAt(-1, "sdkBuildToolsRevision", "33.0.1")
$success = $json.AddStringAt(-1, "ndk", $ndkPath)
$success = $json.AddStringAt(-1, "toolchain-prefix", "llvm")
$success = $json.AddStringAt(-1, "tool-prefix", "llvm")
$success = $json.AddBoolAt(-1, "useLLVM", 1)
$success = $json.AddStringAt(-1, "toolchain-version", "clang")
$success = $json.AddStringAt(-1, "ndk-host", $ndkhost)
$success = $json.AddObjectAt(-1, "architectures")
$architectures = $json.ObjectAt($json.Size - 1)
$success = $architectures.AddStringAt(-1, $androidABI, "aarch64-linux-android")
$success = $json.AddStringAt(-1, "application-binary", "launch")
$paths = $buildPath, $(Join-Path $buildPath -ChildPath "$($domainName)-prefix/src/$($domainName)-build/$($domainName)")
$success = $json.AddStringAt(-1, "qml-import-paths", $($paths -join ","))
$st = New-Object Chilkat.StringTable
$success = $st.Append($(Join-Path $buildPath -ChildPath "$($domainName)-prefix/src/$($domainName)/src/qml"))
$success = $json.AppendStringArray("qml-root-path", $st)
$success = $json.AddStringAt(-1, "qml-importscanner-binary", $(Join-Path $qtHostPath -ChildPath "bin/qmlimportscanner.exe"))
$success = $json.AddStringAt(-1, "rcc-binary", $(Join-Path $qtHostPath -ChildPath "bin/rcc.exe"))
$st = New-Object Chilkat.StringTable
$success = $st.Append($(Join-Path $ndkPath -ChildPath "toolchains/llvm/prebuilt/$($ndkhost)/sysroot"))
$success = $json.AppendStringArray("extraPrefixDirs", $st)
$st = New-Object Chilkat.StringTable
$success = $st.Append($(Join-Path $buildPath -ChildPath "$($domainName)-prefix/src/$($domainName)-build"))
$success = $st.Append($(Join-Path $buildPath -ChildPath "$($domainName)-prefix/src/$($domainName)-build/$($domainName)"))
$success = $json.AppendStringArray("extraLibraryDirs", $st)
$success = $json.AddBoolAt(-1, "zstdCompression", 0)
$success = $json.AddStringAt(-1, "stdcpp-path", $(Join-Path $ndkPath -ChildPath "toolchains/llvm/prebuilt/$($ndkhost)/sysroot/usr/lib/"))
#$success = $json.AddStringAt(-1, "android-extra-plugins", "")
#$success = $json.AddStringAt(-1, "android-extra-libs", "")
$jsonPath = Join-Path $buildPath -ChildPath "android-$($domain)-deployment-settings.json"
$success = $json.WriteFile($jsonPath)

# android deploy qt
$env:JAVA_HOME = $jdkPath

# Build Android APK
$androiddeployqt = Join-Path $qtHostPath -ChildPath "bin/androiddeployqt.exe"
& $androiddeployqt `
    --input $($jsonPath) `
    --output $($androidPath) `
    --android-platform android-33 --jdk $($jdkPath) --gradle

# Install to device
& $androiddeployqt `
   --verbose --output $($androidPath) `
   --no-build --input $($jsonPath) `
   --gradle --reinstall --device $($derialnumber)

$adb = "$(Find-SDKPath)/platform-tools/adb.exe"
& $adb -s $($derialnumber) pull /system/bin/app_process $buildPath/app_process
& $adb -s $($derialnumber) pull /system/bin/app_process32 $buildPath/app_process
& $adb -s $($derialnumber) pull /system/bin/linker $buildPath/linker
& $adb -s $($derialnumber) pull /system/lib/libc.so $buildPath/libc.so

if ($?) {
    Write-Success "Script executed successfully."
}
else {
    Write-Failure "Script execution error."
}

# finished, exit
SafeExit 0