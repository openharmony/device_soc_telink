# device_soc_telink

## 介绍

系统要求：Ubuntu20.04 64位系统版本。

- 运行如下命令，查看Ubuntu版本：

   ```bash
   cat /etc/issue
   ```

编译环境搭建包含如下几步：

1. 工具安装
    1. 安装repo
    2. 安装必要的库和工具
    3. 检查或安装python3
    4. 安装hb
    5. 安装编译工具链
2. 获取源码
3. 确认目录结构
4. 编译流程
5. 烧录

## 1. 工具安装

> 通常系统默认安装samba、vim等常用软件，需要做适当适配以支持Linux服务器与Windows工作台之间的文件共享。

### 1. 安装repo

1. 创建repo安装目录：命令行输入：

    ```bash
    mkdir ~/bin
    ```

2. 下载repo：命令行输入：

    ```bash
    wget https://storage.googleapis.com/git-repo-downloads/repo -P ~/bin/
    ```

3. 改变执行权限：命令行输入：

    ```bash
    chmod a+x ~/bin/repo
    ```

4. 设置环境变量：在~/.bashrc文件的末尾，额外加入以下环境变量：

    ```bash
    # set environment for repo
    export PATH=~/bin:$PATH
    export REPO_URL=https://mirrors.tuna.tsinghua.edu.cn/git/git-repo/
    ```

5. 重启shell或执行以下命令：

    ```bash
    source ~/.bashrc
    ```

### 2. 安装必要的库和工具

> 使用如下apt-get命令安装编译所需的必要的库和工具：

```bash
sudo apt-get install build-essential gcc g++ make zlib* libffi-dev e2fsprogs pkg-config flex bison perl bc openssl libssl-dev libelf-dev libc6-dev-amd64 binutils binutils-dev libdwarf-dev u-boot-tools mtd-utils gcc-arm-linux-gnueabi
```

### 3. 检查或安装Python3

1. 打开Linux编译服务器终端。

2. 输入如下命令，查看python版本：

    ```bash
    python3 --version
    ```

    如果不低于3.8，可以忽略下列步骤，跳转到下一节开始[安装ohos-build](#4-安装hb)。

3. 如果低于python3.7版本，不建议直接升级，请按照如下步骤重新安装。

    以python3.8为例，按照以下步骤安装python。

    ```bash
    sudo apt-get install python3.8
    ```

4. 设置python和python3软链接为python3.8。

   ```bash
   sudo update-alternatives --install /usr/bin/python python /usr/bin/python3.8 1
   sudo update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.8 1
   ```

5. 安装并升级Python包管理工具（pip3），任选如下一种方式。

   - **命令行方式：**

     ```bash
     sudo apt-get install python3-setuptools python3-pip -y
     sudo pip3 install --upgrade pip
     ```

   - **安装包方式：**

     ```bash
     curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
     python get-pip.py
     ```

### 4. 安装hb

#### 前提条件

请先安装Python 3.7.4及以上版本，请[安装Python3](#3-检查或安装python3)。

#### 安装方法

1. 运行如下命令安装ohos-build:

   ```bash
   python3 -m pip install --user ohos-build
   ```

2. 执行"hb -h"，有打印以下信息即表示安装成功：

   ```bash
    usage: hb [-h] [-v] {build,set,env,clean} ...

    OHOS Build System version 0.4.6

    positional arguments:
    {build,set,env,clean}
        build               Build source code
        set                 OHOS build settings
        env                 Show OHOS build env
        clean               Clean output

    optional arguments:
    -h, --help            show this help message and exit
    -v, --version         show program's version number and exit
   ```

### 5. 安装编译工具链

1. 编译链工具推荐使用RISC-V GNU compiler toolchain。

    从[Telink官方网站](http://wiki.telink-semi.cn/wiki/IDE-and-Tools/BDT_for_TLSR9_Series_in_Linux/)可以下载该编译链工具，并解压到用户自行指定的位置。
    [点击下载Toolchain](http://wiki.telink-semi.cn/tools_and_sdk/Tools/IDE/telink_riscv_linux_toolchain.zip)或可以使用下列命令中的工具。

    ```bash
    wget http://wiki.telink-semi.cn/tools_and_sdk/Tools/IDE/telink_riscv_linux_toolchain.zip
    unzip telink_riscv_linux_toolchain.zip
    ```

2. 设置依赖项：

    ```bash
    sudo dpkg --add-architecture i386
    sudo apt-get update
    sudo apt-get install -y libc6:i386 libncurses5:i386 libstdc++6:i386
    ```

3. 在~/.bashrc文件的末尾，根据解压后所存放的路径，将该toolchain加入环境变量：

    ```bash
    # set environment path for toolchain
    RISCV=telink_riscv_linux_toolchain/nds32le-elf-mculib-v5f
    export PATH=~/$RISCV/bin:$PATH
    ```

4. 保存后，执行以下命令使环境变量生效。

    ```bash
    source ~/.bashrc
    ```

## 2. 获取代码流程

1. 新建代码存放目录(用户可以自行指定为其他目录)，并进入：

    ```bash
    mkdir ~/openharmony
    cd ~/openharmony
    ```

2. 打包下载所有文件，此时默认的Harmony版本为Master：

    ```bash
    repo init -u https://gitee.com/openharmony/manifest --no-repo-verify
    ```

3. 下载好仓库后，下载当前分支的代码。

    ```bash
    repo sync -c
    ```

    这里下载的过程的耗时可能会很长，请耐心等待。

4. 下载好代码后，下载部分大容量二进制文件。

    ```bash
    repo forall -c 'git lfs pull'
    ```

## 3. 确认目录结构

在device文件夹下，确保device/soc目录结构如下

```shell
user:~/openharmony/device/soc$ tree -L 3
.
└── telink
    ├── b91                                     # b91 soc适配目录
    │   ├── adapter                                 # 外设驱动、BLE、文件系统适配
    │   ├── b91m_ble_sdk                            # liteos b91 SDK适配
    │   ├── BUILD.gn                                # GN构建脚本
    │   ├── hcs                                     # 设备描述文件
    │   ├── hdf                                     # HDF设备驱动框架目录
    │   ├── Kconfig.liteos_m.defconfig.b91          # b91 Kconfig默认配置
    │   ├── Kconfig.liteos_m.defconfig.series       # series Kconfig默认配置
    │   ├── Kconfig.liteos_m.series                 # series Kconfig配置项
    │   ├── Kconfig.liteos_m.soc                    # soc Kconfig配置项
    │   ├── liteos.ld                               # 链接脚本
    │   └── liteos_m                                # liteos_m 各模块实现
    ├── BUILD.gn                                # GN构建脚本
    ├── Kconfig.liteos_m.defconfig              # liteos_m Kconfig默认配置
    ├── Kconfig.liteos_m.series                 # liteos_m series配置项
    ├── Kconfig.liteos_m.soc                    # liteos_m soc配置项
    └── util                                    # 工具项目录
        └── util.gni                               #  固件生成脚本
```

在device文件夹下，确保device/board目录结构如下

```shell
user:~/openharmony/device/board$ tree -L 3
.
└── telink
    ├── b91_devkit                                # B91 Generic Starter Kit开发板配置目录
    │   ├── b91_devkit_defconfig                    # B91 Generic Starter Kit Kconfig默认配置
    │   ├── BUILD.gn                                # GN构建脚本
    │   ├── Kconfig.liteos_m.board                  # Board liteos_m Kconfig配置项
    │   ├── Kconfig.liteos_m.defconfig.board        # Board liteos_m Kconfig默认配置
    │   └── liteos_m                                # 构建脚本目录
    ├── BUILD.gn                                  # GN构建脚本
    ├── drivers                                   # 板级驱动存放目录
    │   └── BUILD.gn                                # GN构建脚本
    ├── hcs                                       # hcs硬件描述配置目录
    │   ├── b91_devkit.hcs                          # B91 Generic Starter Kit hcs硬件描述脚本
    │   └── BUILD.gn                                # GN构建脚本
    ├── Kconfig.liteos_m.boards                   # Board liteos_m Kconfig配置项
    └── Kconfig.liteos_m.defconfig.boards         # Board liteos_m Kconfig默认配置
```

在vendor文件夹下，确保vendor文件夹目录结构如下

```shell
user:~/openharmony/vendor$ tree -L 3
.
└── telink
    ├── b91_devkit_ble_demo                    # BLE应用示例工程
    │   ├── b91_gatt_sample                       # BLE示例代码
    │   ├── BUILD.gn                              # GN构建脚本
    │   ├── config.json                           # 子系统裁配置裁剪脚本
    │   ├── hals                                  # 产品参数配置
    │   ├── hdf_config                            # HDF硬件描述配置
    │   ├── kernel_configs                        # Kconfig配置输出
    │   └── ohos.build                            # 子系统模块单
    ├── b91_devkit_led_demo                    # LED测试示例工程
    │   ├── BUILD.gn                              # GN构建脚本
    │   ├── config.json                           # 子系统裁配置裁剪脚本
    │   ├── hals                                  # 产品参数配置
    │   ├── hdf_config                            # HDF硬件描述配置
    │   ├── kernel_configs                        # Kconfig配置输出
    │   ├── led_demo                              # LED示例代码
    │   └── ohos.build                            # 子系统模块单
    └── b91_devkit_xts_demo                    # XTS测试示例工程
        ├── BUILD.gn                              # GN构建脚本
        ├── config.json                           # 子系统裁配置裁剪脚本
        ├── hals                                  # 产品参数配置
        ├── hdf_config                            # HDF硬件描述配置
        ├── kernel_configs                        # Kconfig配置输出
        └── ohos.build                            # 子系统模块单
```

## 4. 编译工程

1. 设置工程根目录

    进入源码根目录编译工程，输入以下命令可以选择当前源码根目录为工程根目录：

    ```bash
    hb set -root .
    ```

    此时会自动进入产品选择界面。

    > 如果hb set命令提示报错，请先执行以下命令去构建并安装ohos-build

    ```bash
    python3 -m pip install --user build/lite
    ```

2. 选择产品：

    产品选择界面如下：

    ```bash
        telink
            >b91_devkit_xts_demo
             b91_devkit_ble_demo
             b91_devkit_led_demo
             ...
    ```

    也可使用以下命令进行修改：

    ```bash
    hb set -p {PRODUCT_NAME}
    ```

    {PRODUCT_NAME}为产品选择界面中列举出来的名称，如b91_devkit_xts_demo。

3. 编译

    后续修改了本产品的文件，只需要执行以下命令进行编译，不需要重复设置产品的根目录。

    ```bash
    hb build -f
    ```

    > 注意：若编译XTS项目，请在上述命令行中添加参数如下：

    ```bash
    hb build --gn-args 'build_xts=true' -f
    ```

    生成的固件位于工程根目录下：

    ```bash
    out/{BOARD_NAME}/{PRODUCT_NAME}/bin/{PRODUCT_NAME}.bin
    ```

    {BOARD_NAME}为主板名，为b91_devkit。

## 5. 固件烧录

### 1. 软件安装

Burning and Debugging Tools(BDT)仅支持在Windows平台下安装使用，其可执行安装程序为Telink BDT.exe。
[点击下载BDT](http://wiki.telink-semi.cn/tools_and_sdk/Tools/BDT/BDT.zip)

### 2. 硬件连接

请按照下面的图示进行硬件连接。图中的紫线表示USB线，其他两根线表示杜邦线。

![硬件连接示意图](images/flahsing_hw_connections_1.jpg)

> 注意：务必采用默认的跳帽配置。

下图是完成所有硬件连接后状态：

![硬件连接实例](images/flahsing_hw_connections_2.jpg)

### 3. 烧录步骤

1. 用USB线连接烧录器到电脑的USB口。

2. 下载BDT烧录软件，解压到本地文件夹，双击可执行文件"Telink BDT.exe"。
如果一切正常，可以看到如下的窗口显示，在系统标题栏中可以看到已被连接的烧录器的设备信息（见图中红色框）。

    ![BDT正常启动后的界面](images/bdt_connection.png)

3. 点击工具栏中的"SWS"按钮，如果看到下图中的消息，则标明所有的硬件连接都没有问题。

    ![按下SWS按钮后的界面显示](images/bdt_connection_2.png)

4. 设置Flash擦除的区域大小。点击工具栏中的"Setting"按钮，在弹出的“Setting"窗口中可以看到默认的Flash擦除的区域大小是512kB。

    ![默认的Flash擦除的区域大小](images/bdt_earse_size_512.png)

    将Flash擦除的区域大小设置为“2040”，如下图所示：

    ![改写Flash擦除的区域大小](images/bdt_earse_size_2040.png)

    > 注意：对于外挂2MB Flash的TLSR9518开发板，Flash最后的8kB空间预留用于保存重要的SoC信息，因此最多可以擦除2040kB的Flash区域。

5. 点击工具栏中的"Erase"按钮，等待Flash擦除操作完成。

    ![Flash擦除操作](images/bdt_earse_done.png)

6. 选择需要烧录的BIN文件。点击"File"菜单里面的"Open"子菜单，在弹出的文件选择对话框中选中需要烧录的BIN文件。选中后的BIN文件将显示在底部的状态栏中。

    ![加载BIN文件](images/bdt_file_open.png)

7. 点击工具栏中"Download"按钮，等待Flash烧录完成。

    ![Flash烧录操作](images/bdt_flash_done.png)

### 4. 烧录工具的平台和驱动

烧录工具不需要安装额外的驱动程序。

如果烧录器插入电脑后，可以被Windows的设备管理器正确识别，但是烧录工具软件没有识别到，即在系统标题栏中看不到烧录器的设备信息。

请检查电脑是否用了AMD平台的处理器，如果是的话，请换一台Intel平台处理器的电脑重新试一下。

### 5. 烧录工具常见的问题和解决方法

最常见的问题是：在点击工具栏中的"SWS"按钮后，出现下图中的错误信息。

![按下SWS按钮后的错误信息](images/bdt_sws_error.png)

主要有两种原因。

- 第一种原因是硬件连接不正确。

请参照前面的说明仔细核对所有的硬件连接，确认没有遗漏的连接。

- 在完全排除硬件连接问题之后，考虑第二种原因，即烧录器的固件版本太低。

请按照下面步骤查看烧录器固件的版本。

1. 点击Help菜单下的Upgrade子菜单。

2. 在弹出的Upgrade EVK窗口中，点击"Read FW Version"按钮。在旁边的"Firmware Version"区域将会显示烧录器的固件版本号，例如下图。如果固件版本号低于V3.4，可以确认是由于固件版本太低导致了通讯错误。请继续下面的步骤去完成固件升级。

    ![查看烧录器固件版本](images/bdt_firmware_version.png)

3. 点击窗口中的"Load..."按钮，在BDT工具所在目录下的config目录下的fw子目录找到最新的烧录器固件，如下图中的Firmware_v3.5.bin文件。

    ![升级烧录器固件](images/bdt_firmware_upgrade.png)

4. 点击Upgrade按钮完成烧录器固件升级。

5. 插拔烧录器的USB线，使烧录器重新上电。

**提示：** 更多BDT的使用说明，请参考[Burning and Debugging Tools for all Series](http://wiki.telink-semi.cn/wiki/IDE-and-Tools/Burning-and-Debugging-Tools-for-all-Series/)

## 相关仓库

[vendor_telink](https://gitee.com/openharmony-sig/vendor_telink)

[device_board_telink](https://gitee.com/openharmony-sig/device_board_telink)
