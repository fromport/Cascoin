Cascoin Core integration/staging tree
===========================================

What is Cascoin?
----------------------

Cascoin is a fork of Litecoin Cassh supporting SHA256 PoW and Hive Mining. For full details, as well as prebuilt binaries for Windows, Mac and Linux, please visit our website at https://cascoin.net.

Cascoin Core is the full node software that makes up the backbone of the CAS network.

License
-------

Cascoin Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Build from source (Linux, Qt6)
--------------------------------

The project now builds against Qt6 and requires a C++17-capable toolchain.

Tested on Ubuntu 22.04/24.04 (Debian derivatives should be similar).

1) Install build dependencies

   On Ubuntu/Debian:

   ```bash
   sudo apt-get update
   sudo apt-get install -y \
     autoconf automake libtool pkg-config build-essential \
     qt6-base-dev qt6-base-dev-tools qt6-tools-dev-tools libqt6svg6-dev \
     libprotobuf-dev protobuf-compiler libevent-dev libboost-all-dev \
     libminiupnpc-dev libssl-dev libzmq3-dev libqrencode-dev \
     libdb++-dev
   ```

   Notes:
   - If your distro packages only newer Berkeley DB versions, configure with `--with-incompatible-bdb` (see step 3).
   - Qt6 helper tools (`moc`, `uic`, `rcc`, `lrelease`) are typically in `/usr/lib/qt6/libexec` and `/usr/lib/qt6/bin`.

2) Ensure Qt6 tools are on PATH

   ```bash
   export PATH=/usr/lib/qt6/libexec:/usr/lib/qt6/bin:$PATH
   ```

3) Generate build system and configure

   ```bash
   ./autogen.sh
   ./configure \
     --with-gui=qt6 \
     --enable-wallet \
     --with-qrencode \
     --enable-zmq \
     --with-incompatible-bdb
   ```

   Common options:
   - Build without GUI: `--with-gui=no`
   - Disable tests/bench (faster build): `--disable-tests --disable-bench`

4) Build

   ```bash
   make -j$(nproc)
   ```

5) Binaries

   - GUI wallet: `src/qt/cascoin-qt`
   - Daemon/CLI (if enabled by your configure flags): `src/cascoind`, `src/cascoin-cli`, `src/cascoin-tx`

Troubleshooting
----------------

- Qt6 tools not found (moc/uic/rcc/lrelease): ensure `qt6-base-dev-tools` and `qt6-tools-dev-tools` are installed and that `/usr/lib/qt6/libexec:/usr/lib/qt6/bin` are on your `PATH`.
- Berkeley DB version mismatch: use system `libdb++-dev` with `--with-incompatible-bdb`, or provide a compatible DB via the `depends/` system.
- Missing features:
  - ZMQ: install `libzmq3-dev` and configure with `--enable-zmq`.
  - QR codes in GUI: install `libqrencode-dev` and configure with `--with-qrencode`.

Other platforms
----------------

For Windows/macOS and distribution-specific notes, see the documents under `doc/`:

- `doc/build-windows.md`
- `doc/build-osx.md`
- `doc/build-unix.md`

Development Process
-------------------

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/Casraw/Cascoin/tags) are created
regularly to indicate new official, stable release versions of Cascoin Core.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).

The developer [mailing list](https://groups.google.com/forum/#!forum/cascoin-dev)
should be used to discuss complicated or controversial changes before working
on a patch set.

Developer IRC can be found on Freenode at #cascoin-dev.

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](src/test/README.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`. Further details on running
and extending unit tests can be found in [/src/test/README.md](/src/test/README.md).

There are also [regression and integration tests](/test), written
in Python, that are run automatically on the build server.
These tests can be run (if the [test dependencies](/test) are installed) with: `test/functional/test_runner.py`

The Travis CI system makes sure that every pull request is built for Windows, Linux, and OS X, and that unit/sanity tests are run automatically.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.

Translations
------------

Any translation corrections or expansions are welcomed as GitHub pull requests.


Links
------------

🌐 Website: [cascoin.net]
⛏ Pool: [mining-pool.io]
💬 Discord: [https://discord.gg/J2NxATBS8z]
📂 GitHub: [https://github.com/Casraw/Cascoin]