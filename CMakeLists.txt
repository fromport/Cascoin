cmake_minimum_required(VERSION 3.20)
project(CascoinCore VERSION 2.0.5 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(ENABLE_WALLET "Enable wallet functionality" ON)
option(ENABLE_QT "Enable Qt GUI" ON)
option(ENABLE_TESTS "Enable tests" ON)
option(ENABLE_BENCH "Enable benchmarks" ON)
option(ENABLE_ZMQ "Enable ZeroMQ notifications" ON)
option(ENABLE_UPNP "Enable UPnP support" ON)

# Platform-specific settings
if(WIN32)
    add_definitions(-DWIN32 -D_WINDOWS -DNOMINMAX -DBOOST_THREAD_USE_LIB)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# Boost
find_package(Boost REQUIRED COMPONENTS 
    system 
    filesystem 
    program_options 
    thread 
    chrono
)

# OpenSSL
find_package(OpenSSL REQUIRED)

# libevent
pkg_check_modules(LIBEVENT REQUIRED libevent)
if(NOT LIBEVENT_FOUND)
    find_package(libevent REQUIRED)
endif()

# Protocol Buffers
find_package(Protobuf REQUIRED)

# ZeroMQ (optional)
if(ENABLE_ZMQ)
    pkg_check_modules(ZMQ libzmq)
    if(ZMQ_FOUND)
        add_definitions(-DENABLE_ZMQ=1)
    else()
        find_package(ZeroMQ QUIET)
        if(ZeroMQ_FOUND)
            set(ZMQ_LIBRARIES ${ZeroMQ_LIBRARIES})
            add_definitions(-DENABLE_ZMQ=1)
        else()
            message(WARNING "ZeroMQ not found, disabling ZMQ support")
            set(ENABLE_ZMQ OFF)
            add_definitions(-DENABLE_ZMQ=0)
        endif()
    endif()
else()
    add_definitions(-DENABLE_ZMQ=0)
endif()

# Qt6 (optional)
if(ENABLE_QT)
    find_package(Qt6 COMPONENTS Core Widgets Network Svg QUIET)
    if(Qt6_FOUND)
        set(QT_FOUND TRUE)
        qt6_standard_project_setup()
    else()
        message(WARNING "Qt6 not found, disabling GUI")
        set(ENABLE_QT OFF)
    endif()
endif()

# Berkeley DB for wallet
if(ENABLE_WALLET)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(BDB QUIET libdb_cxx-4.8)
    if(NOT BDB_FOUND)
        pkg_check_modules(BDB QUIET libdb_cxx)
        if(NOT BDB_FOUND)
            message(WARNING "Berkeley DB not found, wallet functionality may be limited")
        endif()
    endif()
endif()

# MiniUPnPc (optional)
if(ENABLE_UPNP)
    pkg_check_modules(MINIUPNPC miniupnpc)
    if(MINIUPNPC_FOUND)
        add_definitions(-DUSE_UPNP=1)
    else()
        message(WARNING "MiniUPnPc not found, disabling UPnP support")
        set(ENABLE_UPNP OFF)
        add_definitions(-DUSE_UPNP=0)
    endif()
endif()

# QR Code support (optional)
pkg_check_modules(QRENCODE libqrencode)
if(QRENCODE_FOUND)
    add_definitions(-DUSE_QRCODE=1)
else()
    add_definitions(-DUSE_QRCODE=0)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config
    ${CMAKE_CURRENT_SOURCE_DIR}/src/univalue/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/secp256k1/include
)

# Compiler definitions
add_definitions(
    -DHAVE_BUILD_INFO
    -D__STDC_FORMAT_MACROS
    -DCLIENT_VERSION_MAJOR=2
    -DCLIENT_VERSION_MINOR=0
    -DCLIENT_VERSION_REVISION=5
    -DCLIENT_VERSION_BUILD=0
)

if(ENABLE_WALLET)
    add_definitions(-DENABLE_WALLET=1)
else()
    add_definitions(-DENABLE_WALLET=0)
endif()

# Source files for core library
set(CASCOIN_CORE_SOURCES
    src/addrdb.cpp
    src/addrman.cpp
    src/amount.cpp
    src/arith_uint256.cpp
    src/base58.cpp
    src/bech32.cpp
    src/bloom.cpp
    src/blockencodings.cpp
    src/chain.cpp
    src/chainparams.cpp
    src/chainparamsbase.cpp
    src/chainparamsseeds.h
    src/checkpoints.cpp
    src/clientversion.cpp
    src/coins.cpp
    src/compressor.cpp
    src/consensus/merkle.cpp
    src/core_read.cpp
    src/core_write.cpp
    src/crypter.cpp
    src/dbwrapper.cpp
    src/fees.cpp
    src/hash.cpp
    src/httprpc.cpp
    src/httpserver.cpp
    src/init.cpp
    src/key.cpp
    src/keystore.cpp
    src/main.cpp
    src/merkleblock.cpp
    src/miner.cpp
    src/net.cpp
    src/netaddress.cpp
    src/netbase.cpp
    src/netmessagemaker.cpp
    src/noui.cpp
    src/policy/fees.cpp
    src/policy/policy.cpp
    src/policy/rbf.cpp
    src/pow.cpp
    src/primitives/block.cpp
    src/primitives/transaction.cpp
    src/protocol.cpp
    src/pubkey.cpp
    src/random.cpp
    src/rest.cpp
    src/rpc/blockchain.cpp
    src/rpc/client.cpp
    src/rpc/mining.cpp
    src/rpc/misc.cpp
    src/rpc/net.cpp
    src/rpc/protocol.cpp
    src/rpc/rawtransaction.cpp
    src/rpc/server.cpp
    src/scheduler.cpp
    src/script/interpreter.cpp
    src/script/script.cpp
    src/script/script_error.cpp
    src/script/sigcache.cpp
    src/script/sign.cpp
    src/script/standard.cpp
    src/support/cleanse.cpp
    src/support/events.cpp
    src/support/lockedpool.cpp
    src/sync.cpp
    src/threadsafety.cpp
    src/timedata.cpp
    src/torcontrol.cpp
    src/txdb.cpp
    src/txmempool.cpp
    src/ui_interface.cpp
    src/uint256.cpp
    src/util.cpp
    src/utilmoneystr.cpp
    src/utilstrencodings.cpp
    src/utiltime.cpp
    src/validation.cpp
    src/validationinterface.cpp
    src/versionbits.cpp
    src/warnings.cpp
)

# Wallet sources (conditional)
set(CASCOIN_WALLET_SOURCES
    src/wallet/crypter.cpp
    src/wallet/db.cpp
    src/wallet/feebumper.cpp
    src/wallet/rpcdump.cpp
    src/wallet/rpcwallet.cpp
    src/wallet/wallet.cpp
    src/wallet/walletdb.cpp
    src/wallet/walletutil.cpp
)

# Crypto sources
set(CASCOIN_CRYPTO_SOURCES
    src/crypto/aes.cpp
    src/crypto/chacha20.cpp
    src/crypto/hmac_sha256.cpp
    src/crypto/hmac_sha512.cpp
    src/crypto/ripemd160.cpp
    src/crypto/sha1.cpp
    src/crypto/sha256.cpp
    src/crypto/sha512.cpp
)

# Common sources
set(CASCOIN_COMMON_SOURCES
    src/amount.cpp
    src/base58.cpp
    src/bech32.cpp
    src/chainparams.cpp
    src/coins.cpp
    src/compressor.cpp
    src/core_read.cpp
    src/core_write.cpp
    src/hash.cpp
    src/key.cpp
    src/keystore.cpp
    src/netaddress.cpp
    src/netbase.cpp
    src/policy/feerate.cpp
    src/protocol.cpp
    src/scheduler.cpp
    src/script/sign.cpp
    src/script/standard.cpp
    src/warnings.cpp
)

# ZMQ sources (conditional)
if(ENABLE_ZMQ)
    set(CASCOIN_ZMQ_SOURCES
        src/zmq/zmqabstractnotifier.cpp
        src/zmq/zmqnotificationinterface.cpp
        src/zmq/zmqpublishnotifier.cpp
    )
endif()

# Create core library
add_library(cascoin_core STATIC
    ${CASCOIN_CORE_SOURCES}
    ${CASCOIN_CRYPTO_SOURCES}
    ${CASCOIN_COMMON_SOURCES}
)

if(ENABLE_WALLET)
    target_sources(cascoin_core PRIVATE ${CASCOIN_WALLET_SOURCES})
endif()

if(ENABLE_ZMQ)
    target_sources(cascoin_core PRIVATE ${CASCOIN_ZMQ_SOURCES})
endif()

# Link libraries to core
target_link_libraries(cascoin_core
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${LIBEVENT_LIBRARIES}
    ${Protobuf_LIBRARIES}
    Threads::Threads
)

if(ENABLE_ZMQ AND ZMQ_FOUND)
    target_link_libraries(cascoin_core ${ZMQ_LIBRARIES})
endif()

if(BDB_FOUND)
    target_link_libraries(cascoin_core ${BDB_LIBRARIES})
endif()

if(MINIUPNPC_FOUND)
    target_link_libraries(cascoin_core ${MINIUPNPC_LIBRARIES})
endif()

if(QRENCODE_FOUND)
    target_link_libraries(cascoin_core ${QRENCODE_LIBRARIES})
endif()

# Platform-specific libraries
if(WIN32)
    target_link_libraries(cascoin_core
        ws2_32
        shlwapi
        mswsock
        ole32
        oleaut32
        uuid
        rpcrt4
        advapi32
        shell32
        winmm
        winspool
        comdlg32
        gdi32
        user32
        kernel32
        crypt32
        iphlpapi
    )
endif()

# Executables
# cascoind (daemon)
add_executable(cascoind src/bitcoind.cpp)
target_link_libraries(cascoind cascoin_core)

# cascoin-cli
add_executable(cascoin-cli src/bitcoin-cli.cpp)
target_link_libraries(cascoin-cli cascoin_core)

# cascoin-tx
add_executable(cascoin-tx src/bitcoin-tx.cpp)
target_link_libraries(cascoin-tx cascoin_core)

# Qt GUI (conditional)
if(ENABLE_QT AND QT_FOUND)
    # Qt sources
    set(CASCOIN_QT_SOURCES
        src/qt/addressbookpage.cpp
        src/qt/addresstablemodel.cpp
        src/qt/askpassphrasedialog.cpp
        src/qt/bantablemodel.cpp
        src/qt/bitcoin.cpp
        src/qt/bitcoinaddressvalidator.cpp
        src/qt/bitcoinamountfield.cpp
        src/qt/bitcoingui.cpp
        src/qt/bitcoinunits.cpp
        src/qt/blockexplorer.cpp
        src/qt/clientmodel.cpp
        src/qt/coincontroldialog.cpp
        src/qt/coincontroltreewidget.cpp
        src/qt/csvmodelwriter.cpp
        src/qt/editaddressdialog.cpp
        src/qt/guiutil.cpp
        src/qt/intro.cpp
        src/qt/modaloverlay.cpp
        src/qt/networkstyle.cpp
        src/qt/notificator.cpp
        src/qt/optionsdialog.cpp
        src/qt/optionsmodel.cpp
        src/qt/overviewpage.cpp
        src/qt/paymentserver.cpp
        src/qt/peertablemodel.cpp
        src/qt/platformstyle.cpp
        src/qt/qvalidatedlineedit.cpp
        src/qt/qvaluecombobox.cpp
        src/qt/receivecoinsdialog.cpp
        src/qt/receiverequestdialog.cpp
        src/qt/recentrequeststablemodel.cpp
        src/qt/rpcconsole.cpp
        src/qt/sendcoinsdialog.cpp
        src/qt/sendcoinsentry.cpp
        src/qt/signverifymessagedialog.cpp
        src/qt/splashscreen.cpp
        src/qt/trafficgraphwidget.cpp
        src/qt/transactiondesc.cpp
        src/qt/transactiondescdialog.cpp
        src/qt/transactionfilterproxy.cpp
        src/qt/transactionrecord.cpp
        src/qt/transactiontablemodel.cpp
        src/qt/transactionview.cpp
        src/qt/utilitydialog.cpp
        src/qt/walletframe.cpp
        src/qt/walletmodel.cpp
        src/qt/walletmodeltransaction.cpp
        src/qt/walletview.cpp
        src/qt/winshutdownmonitor.cpp
    )

    # Qt UI files
    set(CASCOIN_QT_FORMS
        src/qt/forms/addressbookpage.ui
        src/qt/forms/askpassphrasedialog.ui
        src/qt/forms/coincontroldialog.ui
        src/qt/forms/editaddressdialog.ui
        src/qt/forms/helpmessagedialog.ui
        src/qt/forms/intro.ui
        src/qt/forms/modaloverlay.ui
        src/qt/forms/openuridialog.ui
        src/qt/forms/optionsdialog.ui
        src/qt/forms/overviewpage.ui
        src/qt/forms/receivecoinsdialog.ui
        src/qt/forms/receiverequestdialog.ui
        src/qt/forms/debugwindow.ui
        src/qt/forms/sendcoinsdialog.ui
        src/qt/forms/sendcoinsentry.ui
        src/qt/forms/signverifymessagedialog.ui
        src/qt/forms/transactiondescdialog.ui
    )

    # Qt resources
    set(CASCOIN_QT_RESOURCES
        src/qt/bitcoin.qrc
    )

    # Process Qt files
    qt6_add_resources(CASCOIN_QT_RC_SOURCES ${CASCOIN_QT_RESOURCES})
    qt6_wrap_ui(CASCOIN_QT_UI_SOURCES ${CASCOIN_QT_FORMS})

    # Create Qt executable
    add_executable(cascoin-qt
        ${CASCOIN_QT_SOURCES}
        ${CASCOIN_QT_RC_SOURCES}
        ${CASCOIN_QT_UI_SOURCES}
    )

    target_link_libraries(cascoin-qt
        cascoin_core
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
        Qt6::Svg
    )

    # Platform-specific Qt settings
    if(WIN32)
        set_target_properties(cascoin-qt PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
    endif()
endif()

# Tests (conditional)
if(ENABLE_TESTS)
    enable_testing()
    find_package(Boost REQUIRED COMPONENTS unit_test_framework)
    
    # Add test sources here
    # add_subdirectory(src/test)
endif()

# Benchmarks (conditional)
if(ENABLE_BENCH)
    # Add benchmark sources here
    # add_subdirectory(src/bench)
endif()

# Installation
install(TARGETS cascoind cascoin-cli cascoin-tx
    RUNTIME DESTINATION bin
)

if(ENABLE_QT AND QT_FOUND)
    install(TARGETS cascoin-qt
        RUNTIME DESTINATION bin
    )
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "Cascoin Core ${PROJECT_VERSION} Configuration Summary:")
message(STATUS "  Wallet support: ${ENABLE_WALLET}")
message(STATUS "  GUI support: ${ENABLE_QT}")
message(STATUS "  ZeroMQ support: ${ENABLE_ZMQ}")
message(STATUS "  UPnP support: ${ENABLE_UPNP}")
message(STATUS "  QR code support: ${QRENCODE_FOUND}")
message(STATUS "  Tests: ${ENABLE_TESTS}")
message(STATUS "  Benchmarks: ${ENABLE_BENCH}")
message(STATUS "")
