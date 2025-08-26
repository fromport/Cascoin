// Copyright (c) 2024 The Cascoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CASCOIN_QT_BCTDATABASE_H
#define CASCOIN_QT_BCTDATABASE_H

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QMutex>
#include <QFile>
#include <QDir>

/**
 * Local BCT (Bee Creation Transaction) database for GUI
 * Stores BCT data locally to avoid blockchain synchronization delays
 */
class BCTDatabase
{
public:
    struct BCTInfo {
        QString txid;
        QString status;
        int totalMice;
        int blocksLeft;
        QString honeyAddress;
        qint64 timestamp;
        QStringList availableMice;
    };

    BCTDatabase();
    ~BCTDatabase();

    // Database operations
    bool initialize();
    bool addBCT(const BCTInfo& bct);
    bool updateBCT(const QString& txid, const BCTInfo& bct);
    bool removeBCT(const QString& txid);
    QList<BCTInfo> getAllBCTs();
    BCTInfo getBCT(const QString& txid);
    
    // Convenience methods
    int getTotalAvailableMice();
    int getTotalBCTs();
    bool hasBCT(const QString& txid);
    
    // File operations
    bool loadFromFile();
    bool saveToFile();
    QString getDatabasePath();

private:
    QList<BCTInfo> bctList;
    QMutex mutex;
    QString databasePath;
    
    // Helper methods
    QJsonObject bctToJson(const BCTInfo& bct);
    BCTInfo bctFromJson(const QJsonObject& json);
    bool ensureDirectoryExists();
    void createSampleData();
};

#endif // CASCOIN_QT_BCTDATABASE_H
