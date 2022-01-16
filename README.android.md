# Uxn on Android

This is a working port of Uxn to run on Android.

## Guide

To get a proper on-screen keyboard, install
[Hacker's Keyboard](https://play.google.com/store/apps/details?id=org.pocketworkstation.pckeyboard).
Use it naturally, as you would use a physical keyboard,
including pressing and holding modifiers (Ctrl, Alt, etc) with letters
if a ROM requires it (like Noodle, to load or save an image).

Another alternative keyboard is [Multiling O](https://play.google.com/store/apps/details?id=kl.ime.oh).

Tapping anywhere outside of ROM screen area will toggle the keyboard.

`F4` will invoke "boot" ROM, which displays all the ROMs available for loading,
including those you once downloaded with a browser and then opened using Uxn.

## Building yourself

Easiest way to start is to install JDK first, then [Android
Studio](https://developer.android.com/studio) and make sure NDK is
there to use on your computer.  Your OS package manager might already
have the packages required, if you're willing to go that route.

Make sure environment is properly configured:

	; ls $ANDROID_HOME
	build-tools  emulator  licenses  ndk  patcher  platform-tools  platforms  tools

You should be able to build now:

	; ./prepare-android.sh
	; cd src/android; ./gradlew assembleDebug
	; find app -name *.apk
	app/build/outputs/apk/debug/app-debug.apk

Refer to Android development documentation for more details.
