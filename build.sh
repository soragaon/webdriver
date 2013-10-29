#!/usr/bin/env bash
output_gen=$1
platform=$2
mode=$3
qt_dir=$4

current_dir=`pwd`
if [ -z $output_gen ];
then
  output_gen=`pwd`/out
  platform="desktop"
fi

output_gen=`readlink -m ${output_gen}`
base_output_gen=`dirname ${output_gen}`

if [ -z $platform ];
then
  platforms="desktop"
else
  platforms=$platform
fi
if [ -z $mode ];
then
  modes="release_dbg release"
else
  modes=$mode
fi

OUT_STATIC_LIB_FILES="libchromium_base.a libWebDriver_core.a libWebDriver_extension_qt_base.a libWebDriver_extension_qt_web.a libWebDriver_extension_qt_quick.a libWebDriver_extension_qt_quick_web.a"
OUT_SHARED_LIB_FILES="libchromium_base.so libWebDriver_core.so libWebDriver_extension_qt_base.so libWebDriver_extension_qt_web.so libWebDriver_extension_qt_quick.so libWebDriver_extension_qt_quick_web.so libWebDriver_noWebkit_android.so"
OUT_BIN_FILES="WebDriver WebDriver_noWebkit WebDriver_noWebkit_sharedLibs"
GYP=`which gyp`

#generate wdversion.cc
python generate_wdversion.py

for platform in $platforms
do
  for mode in $modes
  do
    cd $current_dir

    OUTPUT_DIR=${output_gen}/$platform/$mode
    OUTPUT_DIR_OUT=${output_gen}/$platform/$mode/Default
    OUTPUT_BIN_DIR=${output_gen}/bin/$platform/$mode

    python $GYP --depth . -G output_dir=. -D platform=$platform -D mode=$mode -D ROOT_PATH=${base_output_gen} -D QT_DIR=${qt_dir} --generator-output=${output_gen}/$platform/$mode wd.gyp
    [ $? -ne 0 ] && exit 1
    cd $OUTPUT_DIR
    [ $? -ne 0 ] && echo "**** ERROR: Can't access to $OUTPUT_DIR" && exit 1
    make
    [ $? -ne 0 ] && exit 1
    mkdir -p ${output_gen}/bin/$platform/$mode/
    [ $? -ne 0 ] && echo "**** ERROR: Can't create $OUTPUT_BIN_DIR" && exit 1

    # copy libraries
    for file in $OUT_STATIC_LIB_FILES; do cp -f $OUTPUT_DIR_OUT/$file $OUTPUT_BIN_DIR 2>/dev/null; done
    for file in $OUT_SHARED_LIB_FILES; do cp -f $OUTPUT_DIR_OUT/lib.target/$file $OUTPUT_BIN_DIR 2>/dev/null; done

    # copy test binaries
    for file in $OUT_BIN_FILES
    do
      if [ -f $OUTPUT_DIR_OUT/$file ]
      then
        cp -f $OUTPUT_DIR_OUT/$file $OUTPUT_BIN_DIR 2>/dev/null
      fi
    done
    
  done
done
