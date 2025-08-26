// Copyright (c) 2024 The Cascoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/beenftpage.h>
#include <qt/walletmodel.h>
#include <qt/platformstyle.h>
#include <qt/guiutil.h>
#include <qt/bitcoinunits.h>

#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QGridLayout>
#include <QSortFilterProxyModel>

BeeNFTPage::BeeNFTPage(const PlatformStyle *_platformStyle, QWidget *parent) :
    QWidget(parent),
    walletModel(0),
    beeNFTModel(0),
    platformStyle(_platformStyle)
{
    setupUI();
}

BeeNFTPage::~BeeNFTPage()
{
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
        loadAvailableMice();
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
    
    // Load available mice on startup
    if (walletModel) {
        loadAvailableMice();
    }
}

void BeeNFTPage::loadAvailableMice()
{
    if (!walletModel) {
        return;
    }
    
    mouseSelectionCombo->clear();
    mouseSelectionCombo->addItem(tr("Loading available mice..."));
    mouseSelectionCombo->setEnabled(false);
    
    // Call RPC to get available mice
    QString command = "miceavailable";
    
    // TODO: Use proper RPC call through walletModel
    // For now, add placeholder items
    mouseSelectionCombo->clear();
    mouseSelectionCombo->addItem(tr("Select a mouse to tokenize..."), "");
    
    // Placeholder data - in real implementation, this would come from RPC
    QStringList placeholderMice;
    placeholderMice << "BCT abc123...:0 (Mouse #0 - Mature)"
                   << "BCT abc123...:1 (Mouse #1 - Mature)"
                   << "BCT def456...:0 (Mouse #0 - Mature)"
                   << "BCT def456...:1 (Mouse #1 - Mature)";
    
    for (const QString& mouseDesc : placeholderMice) {
        QString mouseId = mouseDesc.split(" ").first();
        mouseSelectionCombo->addItem(mouseDesc, mouseId);
    }
    
    mouseSelectionCombo->setEnabled(true);
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
