1.install toolchain and make tools for build kernel

(1)mkdir -p ~/workspace/source; cd ~/workspace/source;

(2)git clone kernel;

(3)mkdir prebuild;
git clone https://android.googlesource.com/platform/prebuilts/build-tools;
git clone https://android.googlesource.com/platform/prebuilts/clang/host/linux-x86;
git clone https://android.googlesource.com/platform/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9

(4) add toolchain path to PATH environment variable

2. export some environment variable and build the kernel and dtb

export SOURCE_ROOT=~/workspace/source
export DEFCONFIG=atoll_defconfig
export MAKE_PATH=${SOURCE_ROOT}/prebuilts/build-tools/linux-x86/bin/
export CROSS_COMPILE=${SOURCE_ROOT}/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-
export KERNEL_ARCH=arm64
export KERNEL_DIR=${SOURCE_ROOT}/msm-4.14
export KERNEL_OUT=${KERNEL_DIR}/../kernel_out
export KERNEL_SRC=${KERNEL_OUT}
export CLANG_TRIPLE=aarch64-linux-gnu-
export OUT_DIR=${KERNEL_OUT}
export ARCH=${KERNEL_ARCH}
export TARGET_INCLUDES=${TARGET_KERNEL_MAKE_CFLAGS}
export TARGET_LINCLUDES=${TARGET_KERNEL_MAKE_LDFLAGS}
export PATH=${SOURCE_ROOT}/prebuilts/clang/host/linux-x86/clang-r383902/bin:$PATH

TARGET_KERNEL_MAKE_ENV+="HOSTCC=${SOURCE_ROOT}/prebuilts/clang/host/linux-x86/clang-r383902/bin/clang "
TARGET_KERNEL_MAKE_ENV+="CC=${SOURCE_ROOT}/prebuilts/clang/host/linux-x86/clang-r383902/bin/clang "
TARGET_KERNEL_MAKE_ENV+="LLVM=${SOURCE_ROOT}/prebuilts/clang/host/linux-x86/clang-r383902/bin/clang"

(cd ${KERNEL_DIR} && \
${MAKE_PATH}make V=1 O=${OUT_DIR} ${TARGET_KERNEL_MAKE_ENV} ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} CFLAGS="${TARGET_INCLUDES}" LDFLAGS="${TARGET_LINCLUDES}" ${DEFCONFIG})


(cd ${OUT_DIR} && \
	${MAKE_PATH}make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} CFLAGS="${TARGET_INCLUDES}" LDFLAGS="${TARGET_LINCLUDES}" O=${OUT_DIR} ${TARGET_KERNEL_MAKE_ENV} -j$(nproc))
