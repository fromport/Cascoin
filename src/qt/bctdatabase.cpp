// Copyright (c) 2024 The Cascoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/bctdatabase.h>

#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMutexLocker>
#include <QDebug>
#include <QDateTime>

BCTDatabase::BCTDatabase()
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dataDir.isEmpty()) {
        dataDir = QDir::homePath() + "/.cascoin";
    }
    databasePath = dataDir + "/bct_cache.json";
}

BCTDatabase::~BCTDatabase()
{
    // Auto-save on destruction
    saveToFile();
}

bool BCTDatabase::initialize()
{
    if (!ensureDirectoryExists()) {
        return false;
    }
    
    // Load existing data if available
    loadFromFile();
    
    // Cascoin: Memory leak fix - Don't create sample data on startup to reduce memory usage
    // Sample data will only be created when explicitly requested
    // if (bctList.isEmpty()) {
    //     createSampleData();
    // }
    
    return true;
}

bool BCTDatabase::addBCT(const BCTInfo& bct)
{
    QMutexLocker locker(&mutex);
    
    // Check if BCT already exists
    for (int i = 0; i < bctList.size(); ++i) {
        if (bctList[i].txid == bct.txid) {
            bctList[i] = bct; // Update existing
            return saveToFile();
        }
    }
    
    // Cascoin: Memory leak fix - Limit BCT cache to max 1000 entries instead of 10000
    if (bctList.size() >= 1000) {
        qDebug() << "BCT database cache limit reached (1000 entries). Removing oldest entries.";
        // Remove the oldest 100 entries to make room for new ones
        for (int i = 0; i < 100 && !bctList.isEmpty(); ++i) {
            bctList.removeFirst();
        }
    }
    
    // Add new BCT
    bctList.append(bct);
    return saveToFile();
}

bool BCTDatabase::updateBCT(const QString& txid, const BCTInfo& bct)
{
    QMutexLocker locker(&mutex);
    
    for (int i = 0; i < bctList.size(); ++i) {
        if (bctList[i].txid == txid) {
            bctList[i] = bct;
            return saveToFile();
        }
    }
    return false;
}

bool BCTDatabase::removeBCT(const QString& txid)
{
    QMutexLocker locker(&mutex);
    
    for (int i = 0; i < bctList.size(); ++i) {
        if (bctList[i].txid == txid) {
            bctList.removeAt(i);
            return saveToFile();
        }
    }
    return false;
}

QList<BCTDatabase::BCTInfo> BCTDatabase::getAllBCTs()
{
    QMutexLocker locker(&mutex);
    return bctList;
}

BCTDatabase::BCTInfo BCTDatabase::getBCT(const QString& txid)
{
    QMutexLocker locker(&mutex);
    
    for (const BCTInfo& bct : bctList) {
        if (bct.txid == txid) {
            return bct;
        }
    }
    return BCTInfo(); // Return empty BCTInfo if not found
}

int BCTDatabase::getTotalAvailableMice()
{
    QMutexLocker locker(&mutex);
    
    int total = 0;
    for (const BCTInfo& bct : bctList) {
        if (bct.status == "mature") {
            total += bct.availableMice.size();
        }
    }
    return total;
}

int BCTDatabase::getTotalBCTs()
{
    QMutexLocker locker(&mutex);
    return bctList.size();
}

bool BCTDatabase::hasBCT(const QString& txid)
{
    QMutexLocker locker(&mutex);
    
    for (const BCTInfo& bct : bctList) {
        if (bct.txid == txid) {
            return true;
        }
    }
    return false;
}

bool BCTDatabase::loadFromFile()
{
    QFile file(databasePath);
    if (!file.exists()) {
        return true; // No file exists yet, that's OK
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open BCT database file for reading:" << databasePath;
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Error parsing BCT database JSON:" << error.errorString();
        return false;
    }
    
    if (!doc.isObject()) {
        qDebug() << "BCT database file is not a JSON object";
        return false;
    }
    
    QJsonObject root = doc.object();
    QJsonArray bctArray = root["bcts"].toArray();
    
    QMutexLocker locker(&mutex);
    bctList.clear();
    
    for (const QJsonValue& value : bctArray) {
        if (value.isObject()) {
            BCTInfo bct = bctFromJson(value.toObject());
            if (!bct.txid.isEmpty()) {
                bctList.append(bct);
            }
        }
    }
    
    qDebug() << "Loaded" << bctList.size() << "BCTs from database";
    return true;
}

bool BCTDatabase::saveToFile()
{
    if (!ensureDirectoryExists()) {
        return false;
    }
    
    QJsonArray bctArray;
    
    // Don't lock mutex here since we're called from locked methods
    for (const BCTInfo& bct : bctList) {
        bctArray.append(bctToJson(bct));
    }
    
    QJsonObject root;
    root["version"] = "1.0";
    root["timestamp"] = QDateTime::currentSecsSinceEpoch();
    root["bcts"] = bctArray;
    
    QJsonDocument doc(root);
    
    QFile file(databasePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Could not open BCT database file for writing:" << databasePath;
        return false;
    }
    
    qint64 written = file.write(doc.toJson());
    file.close();
    
    return written > 0;
}

QString BCTDatabase::getDatabasePath()
{
    return databasePath;
}

QJsonObject BCTDatabase::bctToJson(const BCTInfo& bct)
{
    QJsonObject obj;
    obj["txid"] = bct.txid;
    obj["status"] = bct.status;
    obj["total_mice"] = bct.totalMice;
    obj["blocks_left"] = bct.blocksLeft;
    obj["honey_address"] = bct.honeyAddress;
    obj["timestamp"] = bct.timestamp;
    
    QJsonArray miceArray;
    for (const QString& mouse : bct.availableMice) {
        miceArray.append(mouse);
    }
    obj["available_mice"] = miceArray;
    
    return obj;
}

BCTDatabase::BCTInfo BCTDatabase::bctFromJson(const QJsonObject& json)
{
    BCTInfo bct;
    bct.txid = json["txid"].toString();
    bct.status = json["status"].toString();
    bct.totalMice = json["total_mice"].toInt();
    bct.blocksLeft = json["blocks_left"].toInt();
    bct.honeyAddress = json["honey_address"].toString();
    bct.timestamp = json["timestamp"].toVariant().toLongLong();
    
    QJsonArray miceArray = json["available_mice"].toArray();
    for (const QJsonValue& value : miceArray) {
        bct.availableMice.append(value.toString());
    }
    
    return bct;
}

bool BCTDatabase::ensureDirectoryExists()
{
    QFileInfo fileInfo(databasePath);
    QDir dir = fileInfo.dir();
    
    if (!dir.exists()) {
        return dir.mkpath(".");
    }
    
    return true;
}

void BCTDatabase::createSampleData()
{
    // Create sample BCT data for testing
    QStringList sampleTxids = {
        "a1b2c3d4e5f6789012345678901234567890abcdef1234567890abcdef123456",
        "b2c3d4e5f6789012345678901234567890abcdef1234567890abcdef1234567a",
        "c3d4e5f6789012345678901234567890abcdef1234567890abcdef1234567ab2",
        "d4e5f6789012345678901234567890abcdef1234567890abcdef1234567abc3",
        "e5f6789012345678901234567890abcdef1234567890abcdef1234567abcd4",
        "f6789012345678901234567890abcdef1234567890abcdef1234567abcde5",
        "789012345678901234567890abcdef1234567890abcdef1234567abcdef6",
        "89012345678901234567890abcdef1234567890abcdef1234567abcdef67"
    };
    
    for (int i = 0; i < sampleTxids.size(); ++i) {
        BCTInfo bct;
        bct.txid = sampleTxids[i];
        bct.status = "mature";
        bct.totalMice = (i % 8) + 1; // 1-8 mice per BCT
        bct.blocksLeft = 0;
        bct.honeyAddress = QString("CAddr%1Test123...").arg(i + 1);
        bct.timestamp = QDateTime::currentSecsSinceEpoch() - (i * 3600); // Different timestamps
        
        // Generate available mice for this BCT
        for (int mouseIndex = 0; mouseIndex < bct.totalMice; ++mouseIndex) {
            QString mouseId = QString("%1:%2").arg(bct.txid).arg(mouseIndex);
            bct.availableMice.append(mouseId);
        }
        
        bctList.append(bct);
    }
    
    qDebug() << "Created" << bctList.size() << "sample BCTs";
}
