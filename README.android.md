# Uxn on Android

This is a working port of Uxn to run on Android.

## Building yourself

Easiest way to start is to install JDK first, then [Android
Studio](https://developer.android.com/studio) and make sure NDK is
there to use on your computer.  Your OS package manager might already
have the packages required, if you're willing to go that route.

Make sure environment is properly configured:

	; ls $ANDROID_HOME
	build-tools  emulator  licenses  ndk  patcher  platform-tools  platforms  tools

You should be able to build now:

	; cd uxn; ./build --no-run                 # need boot.rom to be included

	; cd src/android; ./gradlew assembleDebug # now the APK
	............
	BUILD SUCCESSFUL in 111s
	31 actionable tasks: 3 executed, 28 up-to-date

	; find app -name *.apk
	app/build/outputs/apk/debug/app-debug.apk

Refer to Android development documentation for more details.
