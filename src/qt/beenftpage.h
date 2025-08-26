// Copyright (c) 2024 The Cascoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CASCOIN_QT_BEENFTPAGE_H
#define CASCOIN_QT_BEENFTPAGE_H

#include <QWidget>
#include <QTabWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QCheckBox>

class WalletModel;
class BeeNFTTableModel;
class PlatformStyle;

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Widget for managing Bee NFTs */
class BeeNFTPage : public QWidget
{
    Q_OBJECT

public:
    explicit BeeNFTPage(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~BeeNFTPage();

    void setModel(WalletModel *model);

private Q_SLOTS:
    void tokenizeBee();
    void transferBeeNFT();
    void refreshBeeNFTs();
    void showBeeNFTDetails();
    void onBeeNFTSelectionChanged();
    void generateNewAddress();

private:
    WalletModel *walletModel;
    BeeNFTTableModel *beeNFTModel;
    const PlatformStyle *platformStyle;

    // UI Components
    QTabWidget *tabWidget;
    
    // Bee NFT List Tab
    QWidget *listTab;
    QTableView *beeNFTView;
    QPushButton *refreshButton;
    QPushButton *detailsButton;
    QCheckBox *showExpiredCheckBox;
    
    // Tokenize Tab
    QWidget *tokenizeTab;
    QLineEdit *bctTxidEdit;
    QSpinBox *beeIndexSpin;
    QLineEdit *ownerAddressEdit;
    QPushButton *tokenizeButton;
    QPushButton *generateAddressButton;
    
    // Transfer Tab
    QWidget *transferTab;
    QComboBox *beeNFTCombo;
    QLineEdit *recipientAddressEdit;
    QPushButton *transferButton;
    
    void setupUI();
    void updateBeeNFTCombo();
};

#endif // CASCOIN_QT_BEENFTPAGE_H
