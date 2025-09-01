// Copyright (c) 2011-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include <config/bitcoin-config.h>
#endif

#include <qt/splashscreen.h>

#include <qt/networkstyle.h>

#include <clientversion.h>
#include <init.h>
#include <util.h>
#include <ui_interface.h>
#include <version.h>

#ifdef ENABLE_WALLET
#include <wallet/wallet.h>
#endif

#include <QApplication>
#include <QCloseEvent>
#if QT_VERSION < 0x060000
#include <QDesktopWidget>
#else
#include <QScreen>
#endif
#include <QPainter>
#include <QRadialGradient>

SplashScreen::SplashScreen(Qt::WindowFlags f, const NetworkStyle *networkStyle) :
    QWidget(0, f), curAlignment(0), startTime(QDateTime::currentDateTime())
{
    // set reference point, paddings
    int paddingRight            = 10;   // Cascoin: Fix ugly spacing on splash screen
    int paddingTop              = 50;
    int titleVersionVSpace      = 17;
    int titleCopyrightVSpace    = 40;

    float fontFactor            = 1.0;
    float devicePixelRatio      = 1.0;
#if QT_VERSION > 0x050100
    devicePixelRatio = ((QGuiApplication*)QCoreApplication::instance())->devicePixelRatio();
#endif

    // define text to place
    QString titleText       = tr("Cascoin");  // Cascoin: Show the pretty name, not package name
    QString versionText     = QString("Version %1").arg(QString::fromStdString(FormatFullVersion()));
    QString copyrightText   = QString::fromUtf8(CopyrightHolders(strprintf("\xc2\xA9 %u-%u ", 2011, COPYRIGHT_YEAR)).c_str());
    QString titleAddText    = networkStyle->getTitleAddText();

    QString font            = QApplication::font().toString();

    // create a larger bitmap for more information display (slightly reduced)
    QSize splashSize(700*devicePixelRatio,480*devicePixelRatio);
    pixmap = QPixmap(splashSize);

#if QT_VERSION > 0x050100
    // change to HiDPI if it makes sense
    pixmap.setDevicePixelRatio(devicePixelRatio);
#endif

    QPainter pixPaint(&pixmap);
    pixPaint.setPen(QColor(100,100,100));

    // draw a slightly radial gradient
    QRadialGradient gradient(QPoint(0,0), splashSize.width()/devicePixelRatio);
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, QColor(247,247,247));
    QRect rGradient(QPoint(0,0), splashSize);
    pixPaint.fillRect(rGradient, gradient);

    // draw the bitcoin icon, expected size of PNG: 1024x1024
    // Cascoin: Adjusted to show whole logo
    QRect rectIcon(QPoint(20,20), QSize(256,256));
    const QSize requiredSize(256,256);

    QPixmap icon(networkStyle->getAppIcon().pixmap(requiredSize));

    pixPaint.drawPixmap(rectIcon, icon);

    // check font size and drawing with
    pixPaint.setFont(QFont(font, 33*fontFactor));
    QFontMetrics fm = pixPaint.fontMetrics();
    int titleTextWidth = (
        #if QT_VERSION >= 0x050B00
        fm.horizontalAdvance(titleText)
        #else
        fm.width(titleText)
        #endif
    );
    if (titleTextWidth > 176) {
        fontFactor = fontFactor * 176 / titleTextWidth;
    }

    pixPaint.setFont(QFont(font, 33*fontFactor));
    fm = pixPaint.fontMetrics();
    titleTextWidth  = (
        #if QT_VERSION >= 0x050B00
        fm.horizontalAdvance(titleText)
        #else
        fm.width(titleText)
        #endif
    );
    pixPaint.drawText(pixmap.width()/devicePixelRatio-titleTextWidth-paddingRight,paddingTop,titleText);

    pixPaint.setFont(QFont(font, 15*fontFactor));

    // if the version string is to long, reduce size
    fm = pixPaint.fontMetrics();
    int versionTextWidth  = (
        #if QT_VERSION >= 0x050B00
        fm.horizontalAdvance(versionText)
        #else
        fm.width(versionText)
        #endif
    );
    if(versionTextWidth > titleTextWidth+paddingRight-10) {
        pixPaint.setFont(QFont(font, 10*fontFactor));
        titleVersionVSpace -= 5;
    }
    pixPaint.drawText(pixmap.width()/devicePixelRatio-titleTextWidth-paddingRight+2,paddingTop+titleVersionVSpace,versionText);

    // draw copyright stuff
    {
        pixPaint.setFont(QFont(font, 10*fontFactor));
        const int x = pixmap.width()/devicePixelRatio-titleTextWidth-paddingRight;
        const int y = paddingTop+titleCopyrightVSpace;
        QRect copyrightRect(x, y, pixmap.width() - x - paddingRight, pixmap.height() - y);
        pixPaint.drawText(copyrightRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, copyrightText);
    }

    // draw additional text if special network
    if(!titleAddText.isEmpty()) {
        QFont boldFont = QFont(font, 10*fontFactor);
        boldFont.setWeight(QFont::Bold);
        pixPaint.setFont(boldFont);
        fm = pixPaint.fontMetrics();
        int titleAddTextWidth  = (
            #if QT_VERSION >= 0x050B00
            fm.horizontalAdvance(titleAddText)
            #else
            fm.width(titleAddText)
            #endif
        );
        pixPaint.drawText(pixmap.width()/devicePixelRatio-titleAddTextWidth-10,15,titleAddText);
    }
    
    pixPaint.end();

    // Set window title
    setWindowTitle(titleText + " " + titleAddText);

    // Resize window and move to center of desktop, disallow resizing
    QRect r(QPoint(), QSize(pixmap.size().width()/devicePixelRatio,pixmap.size().height()/devicePixelRatio));
    resize(r.size());
    setFixedSize(r.size());
    #if QT_VERSION >= 0x060000
    const QRect screenGeom = screen() ? screen()->geometry() : QGuiApplication::primaryScreen()->geometry();
    move(screenGeom.center() - r.center());
    #else
    move(QApplication::desktop()->screenGeometry().center() - r.center());
    #endif

    subscribeToCoreSignals();
    installEventFilter(this);
}

SplashScreen::~SplashScreen()
{
    unsubscribeFromCoreSignals();
}

bool SplashScreen::eventFilter(QObject * obj, QEvent * ev) {
    if (ev->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(ev);
        if(keyEvent->text()[0] == 'q') {
            StartShutdown();
        }
    }
    return QObject::eventFilter(obj, ev);
}

void SplashScreen::slotFinish(QWidget *mainWin)
{
    Q_UNUSED(mainWin);

    /* If the window is minimized, hide() will be ignored. */
    /* Make sure we de-minimize the splashscreen window before hiding */
    if (isMinimized())
        showNormal();
    hide();
    deleteLater(); // No more need for this
}

static void InitMessage(SplashScreen *splash, const std::string &message)
{
    // Only show InitMessage if it's not a progress update
    if (message.find("━━━") != std::string::npos) {
        // This is already a formatted progress message, show as-is
        QMetaObject::invokeMethod(splash, "showMessage",
            Qt::QueuedConnection,
            Q_ARG(QString, QString::fromStdString(message)),
            Q_ARG(int, Qt::AlignBottom|Qt::AlignHCenter),
            Q_ARG(QColor, QColor(55,55,55)));
        return;
    }
    
    // Simple startup messages only
    std::string simpleMessage;
    
    if (message.find("Done") != std::string::npos) {
        simpleMessage = std::string("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n") +
                       "          CASCOIN READY!\n" +
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n" +
                       "Loading GUI...\n" +
                       "Initializing wallet interface...\n" +
                       "Loading BCTs and Mice NFTs.";
    } else if (message.find("Loading") != std::string::npos) {
        simpleMessage = std::string("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n") +
                       "        CASCOIN LOADING\n" +
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n" +
                       "Loading wallet database...\n" +
                       "Please wait a moment.";
    } else {
        simpleMessage = std::string("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n") +
                       "        CASCOIN STARTING\n" +
                       "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n" +
                       "Initialization in progress...\n" +
                       "Please wait a moment.";
    }
    
    QMetaObject::invokeMethod(splash, "showMessage",
        Qt::QueuedConnection,
        Q_ARG(QString, QString::fromStdString(simpleMessage)),
        Q_ARG(int, Qt::AlignBottom|Qt::AlignHCenter),
        Q_ARG(QColor, QColor(55,55,55)));
}

static void ShowProgress(SplashScreen *splash, const std::string &title, int nProgress, bool resume_possible)
{
    // Calculate elapsed time
    QDateTime currentTime = QDateTime::currentDateTime();
    qint64 elapsedSeconds = splash->startTime.secsTo(currentTime);
    
    // Estimate remaining time
    std::string timeInfo;
    if (nProgress >= 1 && elapsedSeconds >= 1) {
        qint64 estimatedTotalSeconds = (elapsedSeconds * 100) / nProgress;
        qint64 remainingSeconds = estimatedTotalSeconds - elapsedSeconds;
        
        if (remainingSeconds > 60) {
            int remainingMinutes = remainingSeconds / 60;
            timeInfo = strprintf("About %d minute(s) remaining", remainingMinutes);
        } else if (remainingSeconds > 0) {
            timeInfo = strprintf("About %d second(s) remaining", (int)remainingSeconds);
        } else {
            timeInfo = "Almost finished!";
        }
    } else {
        timeInfo = "Calculating remaining time...";
    }
    
    // Create single, clear status message
    std::string statusMessage;
    
    if (title.find("Mice") != std::string::npos || title.find("MICE") != std::string::npos) {
        statusMessage = "INITIALIZING MICE DATABASE";
    } else if (title.find("Loading") != std::string::npos) {
        statusMessage = "LOADING BLOCKCHAIN DATABASE";
    } else if (title.find("Verifying") != std::string::npos) {
        statusMessage = "VERIFYING BLOCKCHAIN";
    } else if (title.find("Rescanning") != std::string::npos) {
        statusMessage = "SCANNING TRANSACTIONS";
    } else if (title.find("GUI") != std::string::npos || title.find("Interface") != std::string::npos) {
        statusMessage = "LOADING USER INTERFACE";
    } else if (nProgress >= 95) {
        statusMessage = "FINALIZING WALLET";
    } else {
        statusMessage = "INITIALIZING CASCOIN";
    }
    
    // Format single, clear message
    std::string finalMessage = 
        std::string("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n") +
        "            " + statusMessage + "\n" +
        "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n" +
        "Status: " + title + "\n\n" +
        "PROGRESS: " + strprintf("%d", nProgress) + "%\n" +
        "TIME: " + strprintf("%d", (int)elapsedSeconds) + " seconds\n" +
        "REMAINING: " + timeInfo + "\n\n";
    
    if (nProgress < 30) {
        finalMessage += "This is normal on first startup\n";
    } else if (nProgress < 70) {
        finalMessage += "Synchronization in progress...\n";
    } else if (nProgress < 95) {
        finalMessage += "Almost ready!\n";
    } else {
        finalMessage += "Loading GUI...\n";
    }
    
    finalMessage += "\n(Press 'q' to quit)";
    
    // Use direct message display instead of InitMessage to avoid confusion
    QMetaObject::invokeMethod(splash, "showMessage",
        Qt::QueuedConnection,
        Q_ARG(QString, QString::fromStdString(finalMessage)),
        Q_ARG(int, Qt::AlignBottom|Qt::AlignHCenter),
        Q_ARG(QColor, QColor(55,55,55)));
}

#ifdef ENABLE_WALLET
void SplashScreen::ConnectWallet(CWallet* wallet)
{
    // Cascoin: Memory leak fix - Avoid duplicate wallet connections
    if (std::find(connectedWallets.begin(), connectedWallets.end(), wallet) == connectedWallets.end()) {
        wallet->ShowProgress.connect(boost::bind(ShowProgress, this, boost::placeholders::_1, boost::placeholders::_2, false));
        connectedWallets.push_back(wallet);
        
        // Limit maximum connected wallets to prevent memory overflow
        const size_t MAX_CONNECTED_WALLETS = 10;
        if (connectedWallets.size() > MAX_CONNECTED_WALLETS) {
            // Disconnect oldest wallet
            CWallet* oldWallet = connectedWallets.front();
            oldWallet->ShowProgress.disconnect(boost::bind(ShowProgress, this, boost::placeholders::_1, boost::placeholders::_2, false));
            connectedWallets.erase(connectedWallets.begin());
        }
    }
}
#endif

void SplashScreen::subscribeToCoreSignals()
{
    // Connect signals to client
    uiInterface.InitMessage.connect(boost::bind(InitMessage, this, boost::placeholders::_1));
    uiInterface.ShowProgress.connect(boost::bind(ShowProgress, this, boost::placeholders::_1, boost::placeholders::_2, boost::placeholders::_3));
#ifdef ENABLE_WALLET
    uiInterface.LoadWallet.connect(boost::bind(&SplashScreen::ConnectWallet, this, boost::placeholders::_1));
#endif
}

void SplashScreen::unsubscribeFromCoreSignals()
{
    // Disconnect signals from client
    uiInterface.InitMessage.disconnect(boost::bind(InitMessage, this, boost::placeholders::_1));
    uiInterface.ShowProgress.disconnect(boost::bind(ShowProgress, this, boost::placeholders::_1, boost::placeholders::_2, boost::placeholders::_3));
#ifdef ENABLE_WALLET
    for (CWallet* const & pwallet : connectedWallets) {
        pwallet->ShowProgress.disconnect(boost::bind(ShowProgress, this, boost::placeholders::_1, boost::placeholders::_2, false));
    }
#endif
}

void SplashScreen::showMessage(const QString &message, int alignment, const QColor &color)
{
    curMessage = message;
    // Keep alignment provided by caller; we'll elide in paintEvent to avoid overflow
    curAlignment = alignment;
    curColor = color;
    update();
}

void SplashScreen::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
    QRect r = rect().adjusted(10, 10, -10, -10);
    painter.setPen(curColor);
    
    // Use smaller font for more text
    QFont font = painter.font();
    font.setPointSize(9);
    painter.setFont(font);
    
    // Don't limit lines - show all information
    QStringList lines = curMessage.split('\n');
    // Only remove empty lines at the end
    while (!lines.isEmpty() && lines.last().trimmed().isEmpty()) {
        lines.removeLast();
    }
    
    QFontMetrics fm(painter.font());
    for (int i = 0; i < lines.size(); ++i) {
        // Allow text wrapping for long lines
        QString line = lines[i];
        if (fm.horizontalAdvance(line) > r.width()) {
            // Simple word wrapping
            QStringList words = line.split(' ');
            QString currentLine;
            for (const QString& word : words) {
                QString testLine = currentLine.isEmpty() ? word : currentLine + " " + word;
                if (fm.horizontalAdvance(testLine) <= r.width()) {
                    currentLine = testLine;
                } else {
                    if (!currentLine.isEmpty()) {
                        lines[i] = currentLine;
                        lines.insert(i + 1, word);
                        currentLine = word;
                    } else {
                        lines[i] = fm.elidedText(word, Qt::ElideRight, r.width());
                    }
                }
            }
            if (!currentLine.isEmpty() && currentLine != lines[i]) {
                lines[i] = currentLine;
            }
        }
    }
    const QString toDraw = lines.join('\n');
    painter.drawText(r, curAlignment, toDraw);
}

void SplashScreen::closeEvent(QCloseEvent *event)
{
    StartShutdown(); // allows an "emergency" shutdown during startup
    event->ignore();
}
