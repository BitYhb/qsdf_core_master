$cmakePath="D:\Qt\Tools\CMake_64\bin\cmake.exe"
$buildPath="C:\Users\zjczh\Desktop\BUILD\jzcode-qsdf\cmake_top_android"

$buildArguments="
-DQt6_DIR:PATH=D:\Qt\6.4.2\android_arm64_v8a\lib\cmake\Qt6 -DNpm_ROOT_PATH:PATH=D:\Program Files\node-v18.13.0-win-x64
-DQSDF_BUILD_APP_BUNDLE:BOOL=ON
-DQSDF_DOMAIN_NAME:STRING=okrdp
-DCMAKE_INSTALL_PREFIX:PATH=\"C:\Users\zjczh\Desktop\BUILD\jzcode-android-pack\"
-DCPACK_IFW_ROOT:PATH=\"D:/Qt/Tools/QtInstallerFramework/4.5/bin\"
-DQSDF_BUILD_ANDROID:BOOL=ON
-DANDROID_NKD:PATH=\"C:\Users\zjczh\AppData\Local\Android\Sdk\ndk\25.1.8937393\"
-DJAVA_HOME:PATH=\"D:\Program Files\jdk-11.0.18+10\"
"

&$cmakePath $buildArguments