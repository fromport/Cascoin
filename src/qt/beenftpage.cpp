// Copyright (c) 2024 The Cascoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/beenftpage.h>
#include <qt/walletmodel.h>
#include <qt/platformstyle.h>
#include <qt/guiutil.h>
#include <qt/bitcoinunits.h>
#include <qt/rpcconsole.h>

#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QGridLayout>
#include <QSortFilterProxyModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDateTime>
#include <QTimer>
#include <thread>

BeeNFTPage::BeeNFTPage(const PlatformStyle *_platformStyle, QWidget *parent) :
    QWidget(parent),
    walletModel(0),
    beeNFTModel(0),
    platformStyle(_platformStyle),
    bctDatabase(new BCTDatabase())
{
    setupUI();
}

BeeNFTPage::~BeeNFTPage()
{
    delete bctDatabase;
}

void BeeNFTPage::setModel(WalletModel *_walletModel)
{
    this->walletModel = _walletModel;
    if (_walletModel) {
        // TODO: Initialize bee NFT model
        // beeNFTModel = _walletModel->getBeeNFTTableModel();
        // beeNFTView->setModel(beeNFTModel);
        
        updateBeeNFTCombo();
        refreshBeeNFTs();
        
        // Load mice asynchronously to not block GUI startup
        QTimer::singleShot(100, this, SLOT(loadAvailableMice()));
    }
}

void BeeNFTPage::setupUI()
{
    setWindowTitle(tr("Mice NFTs"));
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    tabWidget = new QTabWidget(this);
    
    // === Bee NFT List Tab ===
    listTab = new QWidget();
    QVBoxLayout *listLayout = new QVBoxLayout(listTab);
    
    // Controls row
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    
    showExpiredCheckBox = new QCheckBox(tr("Show expired mice NFTs"));
    refreshButton = new QPushButton(tr("Refresh"));
    detailsButton = new QPushButton(tr("Details"));
    
    controlsLayout->addWidget(showExpiredCheckBox);
    controlsLayout->addStretch();
    controlsLayout->addWidget(refreshButton);
    controlsLayout->addWidget(detailsButton);
    
    listLayout->addLayout(controlsLayout);
    
    // Bee NFT table
    beeNFTView = new QTableView();
    beeNFTView->setSelectionBehavior(QAbstractItemView::SelectRows);
    beeNFTView->setSelectionMode(QAbstractItemView::SingleSelection);
    beeNFTView->setAlternatingRowColors(true);
    beeNFTView->setContextMenuPolicy(Qt::CustomContextMenu);
    beeNFTView->setSortingEnabled(true);
    
    listLayout->addWidget(beeNFTView);
    
    tabWidget->addTab(listTab, tr("My Mice NFTs"));
    
    // === Tokenize Tab ===
    tokenizeTab = new QWidget();
    QVBoxLayout *tokenizeLayout = new QVBoxLayout(tokenizeTab);
    
    QGroupBox *tokenizeGroup = new QGroupBox(tr("Tokenize Mouse"));
    QGridLayout *tokenizeGridLayout = new QGridLayout(tokenizeGroup);
    
    // Available Mice Selection
    tokenizeGridLayout->addWidget(new QLabel(tr("Select Mouse:")), 0, 0);
    mouseSelectionCombo = new QComboBox();
    mouseSelectionCombo->setMinimumWidth(400);
    tokenizeGridLayout->addWidget(mouseSelectionCombo, 0, 1, 1, 2);
    
    // Refresh button for mouse list
    refreshMiceButton = new QPushButton(tr("Refresh Available Mice"));
    tokenizeGridLayout->addWidget(refreshMiceButton, 0, 3);
    
    // Owner Address
    tokenizeGridLayout->addWidget(new QLabel(tr("Owner Address:")), 2, 0);
    ownerAddressEdit = new QLineEdit();
    ownerAddressEdit->setPlaceholderText(tr("Address to receive the mice NFT"));
    generateAddressButton = new QPushButton(tr("Generate New"));
    generateAddressButton->setToolTip(tr("Generate a new address for the mice NFT"));
    tokenizeGridLayout->addWidget(ownerAddressEdit, 2, 1);
    tokenizeGridLayout->addWidget(generateAddressButton, 2, 2);
    
    // Tokenize Button
    tokenizeButton = new QPushButton(tr("Tokenize Mouse"));
    if (platformStyle->getImagesOnButtons()) {
        tokenizeButton->setIcon(platformStyle->SingleColorIcon(":/icons/send"));
    }
    tokenizeGridLayout->addWidget(tokenizeButton, 3, 1, 1, 2);
    
    tokenizeLayout->addWidget(tokenizeGroup);
    tokenizeLayout->addStretch();
    
    tabWidget->addTab(tokenizeTab, tr("Tokenize"));
    
    // === Transfer Tab ===
    transferTab = new QWidget();
    QVBoxLayout *transferLayout = new QVBoxLayout(transferTab);
    
    QGroupBox *transferGroup = new QGroupBox(tr("Transfer Mice NFT"));
    QGridLayout *transferGridLayout = new QGridLayout(transferGroup);
    
    // Mice NFT Selection
    transferGridLayout->addWidget(new QLabel(tr("Select Mice NFT:")), 0, 0);
    beeNFTCombo = new QComboBox();
    beeNFTCombo->setToolTip(tr("Select the mice NFT to transfer"));
    transferGridLayout->addWidget(beeNFTCombo, 0, 1, 1, 2);
    
    // Recipient Address
    transferGridLayout->addWidget(new QLabel(tr("Recipient Address:")), 1, 0);
    recipientAddressEdit = new QLineEdit();
    recipientAddressEdit->setPlaceholderText(tr("Enter recipient's address"));
    transferGridLayout->addWidget(recipientAddressEdit, 1, 1, 1, 2);
    
    // Transfer Button
    transferButton = new QPushButton(tr("Transfer Mice NFT"));
    if (platformStyle->getImagesOnButtons()) {
        transferButton->setIcon(platformStyle->SingleColorIcon(":/icons/send"));
    }
    transferGridLayout->addWidget(transferButton, 2, 1, 1, 2);
    
    transferLayout->addWidget(transferGroup);
    transferLayout->addStretch();
    
    tabWidget->addTab(transferTab, tr("Transfer"));
    
    mainLayout->addWidget(tabWidget);
    
    // Connect signals
    connect(refreshButton, SIGNAL(clicked()), this, SLOT(refreshBeeNFTs()));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(showBeeNFTDetails()));
    connect(refreshMiceButton, SIGNAL(clicked()), this, SLOT(loadAvailableMice()));
    connect(tokenizeButton, SIGNAL(clicked()), this, SLOT(tokenizeBee()));
    connect(transferButton, SIGNAL(clicked()), this, SLOT(transferBeeNFT()));
    connect(generateAddressButton, SIGNAL(clicked()), this, SLOT(generateNewAddress()));
    connect(showExpiredCheckBox, SIGNAL(toggled(bool)), this, SLOT(refreshBeeNFTs()));
    
    // Enable/disable buttons based on selection
    detailsButton->setEnabled(false);
    transferButton->setEnabled(false);
    
    connect(beeNFTView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(onBeeNFTSelectionChanged()));
    
    // Defer heavy loading until the user visits the Tokenize tab
    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int idx){
        QWidget* w = tabWidget->widget(idx);
        if (w == tokenizeTab) {
            static bool loadedOnce = false;
            if (!loadedOnce) {
                loadedOnce = true;
                QTimer::singleShot(50, this, SLOT(loadAvailableMice()));
            }
        }
    });
}

void BeeNFTPage::loadAvailableMice()
{
    if (!walletModel) {
        return;
    }
    
    mouseSelectionCombo->clear();
    mouseSelectionCombo->addItem(tr("Loading BCT overview..."));
    mouseSelectionCombo->setEnabled(false);

    // Run RPC in background to avoid blocking UI
    std::thread([this]() {
        // Inform splash that mice DB init starts
        uiInterface.ShowProgress("Mice DB initialisieren", 1, false);
        std::string rpcResult;
        std::string rpcCommand = "miceavailable";

        bool rpcOk = RPCConsole::RPCExecuteCommandLine(rpcResult, rpcCommand);

        if (!rpcOk) {
            // Fallback to local DB on UI thread
            QMetaObject::invokeMethod(this, "loadAvailableMiceFromWallet", Qt::QueuedConnection);
            QMetaObject::invokeMethod(this, [this]() {
                mouseSelectionCombo->setEnabled(true);
            }, Qt::QueuedConnection);
            uiInterface.ShowProgress("Mice DB initialisieren", 100, false);
            return;
        }

        // Parse and populate on UI thread
        QMetaObject::invokeMethod(this, [this, rpcResult]() {
            QString jsonString = QString::fromStdString(rpcResult);
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);

            mouseSelectionCombo->clear();
            mouseSelectionCombo->addItem(tr("Select a BCT (mice will be selectable next)"), "");

            if (error.error == QJsonParseError::NoError && doc.isArray()) {
                QJsonArray bctArray = doc.array();
                int totalAvailableMice = 0;
                int totalBCTs = 0;

                int idx = 0;
                for (const QJsonValue& bctValue : bctArray) {
                    if (!bctValue.isObject()) continue;

                    QJsonObject bct = bctValue.toObject();
                    QString bctTxid = bct["bct_txid"].toString();
                    QString status = bct["status"].toString();
                    int totalMiceInBct = bct["total_mice"].toInt();

                    totalBCTs++;
                    if (status != "mature") continue;

                    // Compute available mice count without enumerating all into the UI
                    QJsonArray availableMice = bct["available_mice"].toArray();
                    int availableCount = 0;
                    for (const QJsonValue& mouseValue : availableMice) {
                        if (!mouseValue.isObject()) continue;
                        QJsonObject mouse = mouseValue.toObject();
                        bool alreadyTokenized = mouse["already_tokenized"].toBool();
                        if (!alreadyTokenized) availableCount++;
                    }

                    QString displayText = QString("BCT %1 — %2/%3 mice available (%4)")
                                           .arg(bctTxid.left(8) + "...")
                                           .arg(availableCount)
                                           .arg(totalMiceInBct)
                                           .arg(status);
                    mouseSelectionCombo->addItem(displayText, bctTxid);
                    totalAvailableMice += availableCount;
                    // Update splash progress roughly based on loop
                    int denom = (int)bctArray.size();
                    if (denom <= 0) denom = 1;
                    int progress = bctArray.isEmpty() ? 100 : (idx * 100) / denom;
                    if (progress > 99) progress = 99;
                    if (progress < 1) progress = 1;
                    uiInterface.ShowProgress("Mice DB initialisieren", progress, false);
                    ++idx;
                }

                if (totalAvailableMice == 0) {
                    mouseSelectionCombo->clear();
                    mouseSelectionCombo->addItem(tr("No mature BCTs with available mice yet"), "");
                } else {
                    mouseSelectionCombo->insertItem(1, tr("--- %1 available mice across %2 BCTs ---").arg(totalAvailableMice).arg(totalBCTs), "");
                    mouseSelectionCombo->insertSeparator(2);
                }
                uiInterface.ShowProgress("Mice DB initialisieren", 100, false);
            } else {
                mouseSelectionCombo->clear();
                mouseSelectionCombo->addItem(tr("Error parsing mice data: %1").arg(error.errorString()), "");
                uiInterface.ShowProgress("Mice DB initialisieren", 100, false);
            }

            mouseSelectionCombo->setEnabled(true);
        }, Qt::QueuedConnection);
    }).detach();
}

void BeeNFTPage::loadAvailableMiceFromWallet()
{
    // Load BCTs from local database (much faster than blockchain sync)
    
    mouseSelectionCombo->clear();
    mouseSelectionCombo->addItem(tr("Select a mouse to tokenize..."), "");
    
    if (!bctDatabase) {
        mouseSelectionCombo->addItem(tr("BCT database not available"), "");
        return;
    }
    
    // Initialize database asynchronously to avoid blocking GUI
    // Don't initialize here, use cached data or show loading message
    
    // Try to load from cache file first (fast) - simplified without try-catch
    if (!bctDatabase->loadFromFile()) {
        // If no cache exists, show placeholder and setup lazy loading
        mouseSelectionCombo->addItem(tr("BCT database loading... Please wait"), "");
        mouseSelectionCombo->addItem(tr("(Database will be ready shortly)"), "");
        return;
    }
        
    // Load all BCTs from local database cache
    QList<BCTDatabase::BCTInfo> bctList = bctDatabase->getAllBCTs();
        
    if (bctList.isEmpty()) {
        // Just show placeholder - don't create sample data on startup
        mouseSelectionCombo->addItem(tr("No BCT data available yet"), "");
        mouseSelectionCombo->addItem(tr("(Data will load as blockchain syncs)"), "");
        return;
    }
        
    int totalAvailableMice = 0;
    int matureBCTs = 0;
    
    for (const BCTDatabase::BCTInfo& bct : bctList) {
        // Only show mature BCTs
        if (bct.status != "mature") continue;
        
        matureBCTs++;
        
        // Add each individual mouse from this BCT
        for (int mouseIndex = 0; mouseIndex < bct.availableMice.size(); mouseIndex++) {
            QString mouseId = bct.availableMice[mouseIndex];
            QString displayText = QString("BCT %1: Mouse #%2 (%3 - %4 mice total)")
                               .arg(bct.txid.left(8) + "...")
                               .arg(mouseIndex)
                               .arg(bct.status)
                               .arg(bct.totalMice);
            
            mouseSelectionCombo->addItem(displayText, mouseId);
            totalAvailableMice++;
        }
    }
    
    // Insert summary at top
    if (totalAvailableMice > 0) {
        mouseSelectionCombo->insertItem(1, tr("--- %1 Available Mice from %2 Mature BCTs ---").arg(totalAvailableMice).arg(matureBCTs), "");
        mouseSelectionCombo->insertSeparator(2);
        mouseSelectionCombo->insertItem(3, tr("(Using local BCT database - instant loading)"), "");
        mouseSelectionCombo->insertSeparator(4);
    } else {
        mouseSelectionCombo->clear();
        mouseSelectionCombo->addItem(tr("No mature BCTs with available mice found"), "");
    }
}

void BeeNFTPage::loadSampleBCTData()
{
    if (!bctDatabase) return;
    
    // Create sample BCT data quickly for immediate GUI response
    QStringList sampleTxids = {
        "a1b2c3d4e5f67890abcdef1234567890abcdef1234567890abcdef123456789a",
        "b2c3d4e5f67890abcdef1234567890abcdef1234567890abcdef123456789ab",
        "c3d4e5f67890abcdef1234567890abcdef1234567890abcdef123456789abc",
        "d4e5f67890abcdef1234567890abcdef1234567890abcdef123456789abcd",
        "e5f67890abcdef1234567890abcdef1234567890abcdef123456789abcde",
        "f67890abcdef1234567890abcdef1234567890abcdef123456789abcdef",
        "67890abcdef1234567890abcdef1234567890abcdef123456789abcdef6",
        "7890abcdef1234567890abcdef1234567890abcdef123456789abcdef67"
    };
    
    for (int i = 0; i < sampleTxids.size(); ++i) {
        BCTDatabase::BCTInfo bct;
        bct.txid = sampleTxids[i];
        bct.status = "mature";
        bct.totalMice = (i % 8) + 1; // 1-8 mice per BCT  
        bct.blocksLeft = 0;
        bct.honeyAddress = QString("CAddr%1Sample123...").arg(i + 1);
        bct.timestamp = QDateTime::currentSecsSinceEpoch() - (i * 3600);
        
        // Generate available mice for this BCT
        for (int mouseIndex = 0; mouseIndex < bct.totalMice; ++mouseIndex) {
            QString mouseId = QString("%1:%2").arg(bct.txid).arg(mouseIndex);
            bct.availableMice.append(mouseId);
        }
        
        bctDatabase->addBCT(bct);
    }
}

void BeeNFTPage::tokenizeBee()
{
    if (!walletModel) {
        return;
    }
    
    QString selectedMouseData = mouseSelectionCombo->currentData().toString();
    QString ownerAddress = ownerAddressEdit->text().trimmed();
    
    // Validate inputs
    if (selectedMouseData.isEmpty() || mouseSelectionCombo->currentIndex() == 0) {
        QMessageBox::warning(this, tr("Input Error"), tr("Please select a mouse to tokenize."));
        return;
    }
    
    if (ownerAddress.isEmpty()) {
        QMessageBox::warning(this, tr("Input Error"), tr("Please enter an owner address or generate a new one."));
        return;
    }
    
    // Extract mouse info from selection
    QString selectedMouseText = mouseSelectionCombo->currentText();
    
    // Confirm tokenization
    QString message = tr("Are you sure you want to tokenize this mouse?\n\n"
                        "Selected: %1\n"
                        "Owner: %2\n\n"
                        "This will create a transferable NFT for this mouse.")
                        .arg(selectedMouseText)
                        .arg(ownerAddress);
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Confirm Tokenization"), 
                                                             message,
                                                             QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // TODO: Call RPC to tokenize bee
        QMessageBox::information(this, tr("Tokenization"), tr("Mouse tokenization functionality will be implemented."));
    }
}

void BeeNFTPage::transferBeeNFT()
{
    if (!walletModel) {
        return;
    }
    
    QString beeNFTId = beeNFTCombo->currentData().toString();
    QString recipientAddress = recipientAddressEdit->text().trimmed();
    
    // Validate inputs
    if (beeNFTId.isEmpty()) {
        QMessageBox::warning(this, tr("Input Error"), tr("Please select a mice NFT to transfer."));
        return;
    }
    
    if (recipientAddress.isEmpty()) {
        QMessageBox::warning(this, tr("Input Error"), tr("Please enter a recipient address."));
        return;
    }
    
    // Confirm transfer
    QString message = tr("Are you sure you want to transfer this mice NFT to %1?\n\n"
                        "This action cannot be undone.")
                        .arg(recipientAddress);
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Confirm Transfer"), 
                                                             message,
                                                             QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // TODO: Call RPC to transfer bee NFT
        QMessageBox::information(this, tr("Transfer"), tr("Mice NFT transfer functionality will be implemented."));
    }
}

void BeeNFTPage::refreshBeeNFTs()
{
    if (!walletModel) {
        return;
    }
    
    // TODO: Refresh bee NFT model
    updateBeeNFTCombo();
}

void BeeNFTPage::showBeeNFTDetails()
{
    QModelIndexList selection = beeNFTView->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        return;
    }
    
    // TODO: Show detailed bee NFT information dialog
    QMessageBox::information(this, tr("Mice NFT Details"), tr("Mice NFT details dialog will be implemented."));
}

void BeeNFTPage::onBeeNFTSelectionChanged()
{
    QModelIndexList selection = beeNFTView->selectionModel()->selectedRows();
    bool hasSelection = !selection.isEmpty();
    
    detailsButton->setEnabled(hasSelection);
}

void BeeNFTPage::updateBeeNFTCombo()
{
    beeNFTCombo->clear();
    
    if (!walletModel) {
        return;
    }
    
    // TODO: Populate with actual bee NFTs from wallet
    beeNFTCombo->addItem(tr("No mice NFTs available"), QString());
    
    transferButton->setEnabled(beeNFTCombo->count() > 1);
}

void BeeNFTPage::generateNewAddress()
{
    if (!walletModel) {
        return;
    }
    
    // TODO: Generate new address for mice NFT
    ownerAddressEdit->setText(tr("CGeneratedAddress123..."));
}
