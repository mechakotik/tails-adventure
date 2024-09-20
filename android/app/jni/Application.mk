
# Uncomment this if you're using STL in your project
# You can find more information here:
# https://developer.android.com/ndk/guides/cpp-support
APP_STL := c++_shared

APP_CPPFLAGS := -std=c++20

APP_ABI := armeabi-v7a arm64-v8a x86 x86_64

# Min runtime API level
APP_PLATFORM=android-16

SUPPORT_OGG_STB := true
SUPPORT_GME := true
SUPPORT_WAV := false
SUPPORT_FLAC_DRFLAC := false
SUPPORT_FLAC_LIBFLAC := false
SUPPORT_OGG := false
SUPPORT_MP3_MINIMP3 := false
SUPPORT_MP3_MPG123 := false
SUPPORT_WAVPACK := false
SUPPORT_MOD_XMP := false
SUPPORT_MID_TIMIDITY := false
