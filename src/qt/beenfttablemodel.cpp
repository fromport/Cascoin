// Copyright (c) 2024 The Cascoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/beenfttablemodel.h>
#include <qt/walletmodel.h>
#include <qt/bitcoinunits.h>
#include <qt/guiutil.h>

#include <QTimer>
#include <QIcon>
#include <QDateTime>

// Column alignments for the table
static int column_alignments[] = {
    Qt::AlignLeft|Qt::AlignVCenter,   /* BeeNFTId */
    Qt::AlignLeft|Qt::AlignVCenter,   /* OriginalBCT */
    Qt::AlignRight|Qt::AlignVCenter,  /* BeeIndex */
    Qt::AlignLeft|Qt::AlignVCenter,   /* Status */
    Qt::AlignLeft|Qt::AlignVCenter,   /* Owner */
    Qt::AlignRight|Qt::AlignVCenter,  /* BlocksLeft */
    Qt::AlignRight|Qt::AlignVCenter,  /* MaturityHeight */
    Qt::AlignRight|Qt::AlignVCenter   /* ExpiryHeight */
};

BeeNFTTableModel::BeeNFTTableModel(WalletModel *parent) :
    QAbstractTableModel(parent),
    walletModel(parent),
    includeExpired(false)
{
    columns << tr("Mice NFT ID") << tr("Original BCT") << tr("Mouse #") 
            << tr("Status") << tr("Owner") << tr("Blocks Left")
            << tr("Maturity Height") << tr("Expiry Height");

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateBeeNFTs()));
    timer->start(30000); // Update every 30 seconds
    
    updateBeeNFTList();
}

BeeNFTTableModel::~BeeNFTTableModel()
{
    if (timer) {
        timer->stop();
    }
}

int BeeNFTTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return cachedBeeNFTList.size();
}

int BeeNFTTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant BeeNFTTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= cachedBeeNFTList.size()) {
        return QVariant();
    }

    const BeeNFTRecord &rec = cachedBeeNFTList[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch(index.column()) {
        case BeeNFTId:
            return rec.beeNFTId.left(16) + "..."; // Truncate for display
        case OriginalBCT:
            return rec.originalBCT.left(16) + "..."; // Truncate for display
        case BeeIndex:
            return rec.beeIndex;
        case Status:
            return rec.status;
        case Owner:
            return rec.currentOwner.left(16) + "..."; // Truncate for display
        case BlocksLeft:
            return rec.blocksLeft;
        case MaturityHeight:
            return rec.maturityHeight;
        case ExpiryHeight:
            return rec.expiryHeight;
        }
    } else if (role == Qt::TextAlignmentRole) {
        return column_alignments[index.column()];
    } else if (role == Qt::ToolTipRole) {
        switch(index.column()) {
        case BeeNFTId:
            return tr("Unique mice NFT identifier: %1").arg(rec.beeNFTId);
        case OriginalBCT:
            return tr("Original BCT transaction: %1").arg(rec.originalBCT);
        case Owner:
            return tr("Current owner: %1").arg(rec.currentOwner);
        case Status:
            if (rec.status == "mature") {
                return tr("This mouse is mature and can be used for mining");
            } else if (rec.status == "immature") {
                return tr("This mouse is still gestating and cannot mine yet");
            } else if (rec.status == "expired") {
                return tr("This mouse has expired and can no longer mine");
            }
            return rec.status;
        }
    } else if (role == Qt::DecorationRole) {
        if (index.column() == Status) {
            if (rec.status == "mature") {
                return QIcon(":/icons/tx_confirmed");
            } else if (rec.status == "immature") {
                return QIcon(":/icons/tx_unconfirmed");
            } else if (rec.status == "expired") {
                return QIcon(":/icons/tx_conflicted");
            }
        }
    } else if (role == Qt::BackgroundRole) {
        if (rec.status == "expired") {
            return QColor(255, 0, 0, 25); // Light red background for expired
        } else if (rec.status == "immature") {
            return QColor(255, 255, 0, 25); // Light yellow background for immature
        } else if (rec.status == "mature") {
            return QColor(0, 255, 0, 25); // Light green background for mature
        }
    }

    return QVariant();
}

QVariant BeeNFTTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole && section < columns.size()) {
            return columns[section];
        }
        if (role == Qt::ToolTipRole) {
            switch(section) {
            case BeeNFTId:
                return tr("Unique identifier for this mice NFT");
            case OriginalBCT:
                return tr("Transaction ID of the original BCT that created this mouse");
            case BeeIndex:
                return tr("Index of this mouse within the original BCT");
            case Status:
                return tr("Current status of the mouse (immature/mature/expired)");
            case Owner:
                return tr("Current owner of this mice NFT");
            case BlocksLeft:
                return tr("Number of blocks until this mouse expires");
            case MaturityHeight:
                return tr("Block height when this mouse matures for mining");
            case ExpiryHeight:
                return tr("Block height when this mouse expires");
            }
        }
    }
    return QVariant();
}

QModelIndex BeeNFTTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (row >= 0 && row < cachedBeeNFTList.size() && column >= 0 && column < columns.size()) {
        return createIndex(row, column);
    }
    return QModelIndex();
}

Qt::ItemFlags BeeNFTTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void BeeNFTTableModel::setIncludeExpired(bool _includeExpired)
{
    if (includeExpired != _includeExpired) {
        includeExpired = _includeExpired;
        updateBeeNFTList();
    }
}

void BeeNFTTableModel::updateBeeNFTList()
{
    if (!walletModel) {
        return;
    }

    beginResetModel();
    cachedBeeNFTList.clear();
    
    // TODO: Fetch actual mice NFTs from wallet
    // For now, add sample data for demonstration
    
    // Sample mice NFT record
    BeeNFTRecord sample;
    sample.beeNFTId = "bee123456789abcdef...";
    sample.originalBCT = "bct987654321fedcba...";
    sample.beeIndex = 0;
    sample.currentOwner = "COwnerAddress123...";
    sample.status = "mature";
    sample.maturityHeight = 100000;
    sample.expiryHeight = 200000;
    sample.tokenizedHeight = 50000;
    sample.blocksLeft = 5000;
    
    cachedBeeNFTList.append(sample);
    
    // Add expired sample if including expired
    if (includeExpired) {
        BeeNFTRecord expiredSample;
        expiredSample.beeNFTId = "bee987654321abcdef...";
        expiredSample.originalBCT = "bct123456789fedcba...";
        expiredSample.beeIndex = 1;
        expiredSample.currentOwner = "COwnerAddress456...";
        expiredSample.status = "expired";
        expiredSample.maturityHeight = 80000;
        expiredSample.expiryHeight = 180000;
        expiredSample.tokenizedHeight = 40000;
        expiredSample.blocksLeft = 0;
        
        cachedBeeNFTList.append(expiredSample);
    }

    endResetModel();
    
    Q_EMIT beeNFTsChanged();
}

void BeeNFTTableModel::updateBeeNFTs()
{
    updateBeeNFTList();
}
