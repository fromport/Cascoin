Mac OS X Build Instructions and Notes
====================================
The commands in this guide should be executed in a Terminal application.
The built-in one is located in `/Applications/Utilities/Terminal.app`.

Preparation
-----------
Install the OS X command line tools:

`xcode-select --install`

When the popup appears, click `Install`.

Then install [Homebrew](https://brew.sh).

Dependencies
----------------------

    brew install automake berkeley-db4 libtool boost@1.65.1 miniupnpc openssl pkg-config protobuf python3 qt@6 libevent

Alternatively, for an easier build experience and to ensure all dependencies are the correct versions, you can use the `depends` system described in the generic [build-unix.md](build-unix.md) guide. This is especially recommended for Qt (which will be built as 6.5.0) and Boost. If you choose this path, you may not need to install all of the above packages manually via Homebrew.

See [dependencies.md](dependencies.md) for a complete overview.

If you want to build the disk image with `make deploy` (.dmg / optional), you need RSVG

    brew install librsvg

If you want to build with ZeroMQ support
    
    brew install zeromq

**Note on Qt**: This project now uses Qt6. The recommended way to build the GUI is using the `depends` system, which will build Qt 6.5.0. If you prefer to use system Qt from Homebrew, ensure you have a compatible Qt6 version installed (e.g., `brew install qt@6` or `brew install qt6`).

**Note on Boost**: Boost 1.65.1 is required. If `brew install boost@1.65.1` does not work, consider using the `depends` system or installing Boost 1.65.1 manually.

**Note on Compiler**: A C++17 compatible compiler is required (usually Clang, provided by the latest Xcode command line tools).

Berkeley DB
-----------
It is recommended to use Berkeley DB 4.8. If you have to build it yourself,
you can use [the installation script included in contrib/](/contrib/install_db4.sh)
like so

```shell
./contrib/install_db4.sh .
```

from the root of the repository.

**Note**: You only need Berkeley DB if the wallet is enabled (see the section *Disable-Wallet mode* below).

Build Cascoin Core
------------------------

1. Clone the cascoin source code and cd into `cascoin`

        git clone https://github.com/cascoin-project/cascoin
        cd cascoin

2.  Build cascoin-core:

    Configure and build the headless cascoin binaries as well as the GUI (if Qt is found).

    You can disable the GUI build by passing `--without-gui` to configure.

        ./autogen.sh
        ./configure
        make

3.  It is recommended to build and run the unit tests:

        make check

4.  You can also create a .dmg that contains the .app bundle (optional):

        make deploy

5.  Installation into user directories (optional):

        make install

    or

        cd ~/cascoin/src
        cp cascoind /usr/local/bin/
        cp cascoin-cli /usr/local/bin/

Running
-------

Cascoin Core is now available at `./src/cascoind`

Before running, it's recommended you create an RPC configuration file.

    echo -e "rpcuser=cascoinrpc\nrpcpassword=$(xxd -l 16 -p /dev/urandom)" > "/Users/${USER}/Library/Application Support/Cascoin/cascoin.conf"

    chmod 600 "/Users/${USER}/Library/Application Support/Cascoin/cascoin.conf"

The first time you run cascoind, it will start downloading the blockchain. This process could take several hours.

You can monitor the download process by looking at the debug.log file:

    tail -f $HOME/Library/Application\ Support/Cascoin/debug.log

Other commands:
-------

    ./src/cascoind -daemon # Starts the cascoin daemon.
    ./src/cascoin-cli --help # Outputs a list of command-line options.
    ./src/cascoin-cli help # Outputs a list of RPC commands when the daemon is running.

Using Qt Creator as IDE
------------------------
You can use Qt Creator as an IDE, for cascoin development.
Download and install the community edition of [Qt Creator](https://www.qt.io/download/).
Uncheck everything except Qt Creator during the installation process.

1. Make sure you installed everything through Homebrew mentioned above
2. Do a proper ./configure --enable-debug
3. In Qt Creator do "New Project" -> Import Project -> Import Existing Project
4. Enter "cascoin-qt" as project name, enter src/qt as location
5. Leave the file selection as it is
6. Confirm the "summary page"
7. In the "Projects" tab select "Manage Kits..."
8. Select the default "Desktop" kit and select "Clang (x86 64bit in /usr/bin)" as compiler
9. Select LLDB as debugger (you might need to set the path to your installation)
10. Start debugging with Qt Creator

Notes
-----

* Tested on OS X 10.8 through 10.13 on 64-bit Intel processors only.

* Building with downloaded Qt binaries is not officially supported. See the notes in [#7714](https://github.com/bitcoin/bitcoin/issues/7714)
