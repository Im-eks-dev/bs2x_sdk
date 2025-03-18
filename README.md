# 简介

此仓库是 Hi2821 的半开源 sdk，且适配了 xfusion。

此 sdk 基于：`BS2X_1.0.11`。

sdk 中的 `application` 以源码的方式提供；其余文件夹以 `.a` 静态库 + 头文件的方式提供。

关于 xfusion 的更多信息见：《[xfusion](https://github.com/x-eks-fusion/xfusion)》。

## 注意：此分支现已过时，请使用最新的默认分支！

## Hi2821 简介

Hi2821V100 系列芯片是一款高度集成 2.4GHz BLE、SLE 多功能 SoC，Hi2821V100 芯片还拥有最大工作频率 64MHz 的高性能 32bit 微处理器，带有 FPU，内嵌 SRAM 160KB，Flash 高达 512KB。

无线通信方面，Hi2821V100 系列芯片 BLE 支持 1MHz/2MHz/4MHz 频宽和最高 BLE5.3 协议；星闪 SLE 兼容 SLE1.0 协议。最高空口速率为 12Mbps。

外设方面包括 3 个 SPI、3 个 UART、2 个 I2C、8 路 PWM、19 个 GPIO、1 个 USB2.0、1 个 NFC Tag、2 个 PDM、 1 个 I2S/PCM、1 个 QDEC、1 个 KEYSCAN 键盘扫描电路，32 个 GPIO、支持 8 路 13bit 分辨率 ADC、ADC 支持对接音频 AMIC。

## 快速入门

1.  fork 此仓库，并将 fork 的仓库克隆至 `xfusion/sdks`。

    如不需要提交代码，可忽略 fork 步骤，直接克隆此仓库。

1.  xfusion 相关配置

    xfusion 拉取最新代码。

    ```bash
    cd xfusion/
    . ./export.sh bs21
    ```

    首次使用 bs2x_sdk 时需要为 xf 的 python 虚拟环境激活后需要安装 bs21 sdk 的依赖。

    ```bash
    # 在 xf 的 python 虚拟环境安装 sdk 所需环境。
    # python3.8 以上
    sudo apt-get install python3 -y
    # Python包管理工具，需要sudo权限
    sudo apt-get install python3-setuptools python3-pip -y
    # Kconfiglib 14.1.0+
    sudo pip3 install kconfiglib
    # pycparser-2.21
    pip install pycparser==2.21
    ```

    修改 plugins/bs21/build.py 中的 SDK_PATH 为你克隆此仓库到本地的路径，通常克隆到 `xfusion/sdks`.

    ```python
    SDK_PATH: Path = api.XF_ROOT / "sdks" / "bs2x_sdk"
    ```

    运行示例。

    ```bash
    cd examples/get_started/hello_world

    # 打开 xf 的 menuconfig 修改配置
    xf menuconfig

    xf build
    ```

    之后拿到 `<bs2x_sdk的实际路径>/output/bs21e/fwpkg/bs21e-sle-ble-slp-central-peripheral/bs21e_all_in_one.fwpkg` 烧录。
