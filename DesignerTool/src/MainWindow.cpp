#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "DlgOpenProject.h"
#include "DlgNewProject.h"
#include "DlgNewItem.h"
#include "DlgInputName.h"

#include "WidgetExplorer.h"
#include "WidgetAtlas.h"

#include "ItemSprite.h"

#include "HyGlobal.h"

#include <QFileDialog>
#include <QShowEvent>
#include <QStringBuilder>
#include <QVBoxLayout>
#include <QTcpSocket>

/*static*/ MainWindow * MainWindow::sm_pInstance = NULL;

MainWindow::MainWindow(QWidget *parent) :   QMainWindow(parent),
                                            ui(new Ui::MainWindow),
                                            m_Settings("Overture Games", "Harmony Designer Tool"),
                                            m_bIsInitialized(false)
{
    ui->setupUi(this);
    sm_pInstance = this;
    
    SetSelectedProj(NULL);

    HYLOG("Harmony Designer Tool v0.0.1", LOGTYPE_Title);
    HYLOG("Initializing...", LOGTYPE_Normal);
    
    ui->actionCloseProject->setEnabled(false);
    ui->actionNewSprite->setEnabled(false);
    ui->actionNewFont->setEnabled(false);
    ui->actionNewParticle->setEnabled(false);
    ui->actionNewAudio->setEnabled(false);
    
    // Link the actions to their proper widgets
    ui->explorer->addAction(ui->actionCloseProject);
    ui->explorer->addAction(ui->actionCopy);
    ui->explorer->addAction(ui->actionCut);
    ui->explorer->addAction(ui->actionNewProject);
    ui->explorer->addAction(ui->actionNewAudio);
    ui->explorer->addAction(ui->actionNewParticle);
    ui->explorer->addAction(ui->actionNewFont);
    ui->explorer->addAction(ui->actionNewSprite);
    ui->explorer->addAction(ui->actionOpenProject);
    ui->explorer->addAction(ui->actionPaste);
    ui->explorer->addAction(ui->actionRemove);
    ui->explorer->addAction(ui->actionRename);
    ui->explorer->addAction(ui->actionSave);
    
    ui->dockWidgetAtlas->hide();
    ui->dockWidgetGlyphCreator->hide();
    
    // Network initialization
    m_pTcpServer = new QTcpServer(this);
    connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    if(!m_pTcpServer->listen(QHostAddress::LocalHost, HY_TCP_PORT))
        HYLOG("Cannot start TCP server", LOGTYPE_Error)
    else
        HYLOG("TCP server initialized", LOGTYPE_Normal);
    
    
    
    HYLOG("Recovering previously opened session...", LOGTYPE_Normal);
    //m_Settings.clear();
    LoadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *pEvent)
{
    QMainWindow::showEvent(pEvent);
    if(pEvent->spontaneous())
        return;

    if(m_bIsInitialized == false)
    {
        ui->actionViewExplorer->setChecked(ui->dockWidgetExplorer->isVisible());
        ui->actionViewProperties->setChecked(ui->dockWidgetGlyphCreator->isVisible());
        ui->actionViewOutputLog->setChecked(ui->dockWidgetOutputLog->isVisible());

        m_bIsInitialized = true;
    }
}

/*static*/ void MainWindow::OpenItem(Item *pItem)
{
    sm_pInstance->ui->renderer->OpenItem(pItem);
    sm_pInstance->ui->explorer->SelectItem(pItem);
}

/*static*/ void MainWindow::CloseItem(Item *pItem)
{
    // TODO: Ask to save file if changes have been made
    sm_pInstance->ui->renderer->CloseItem(pItem);
}

/*static*/ void MainWindow::SetSelectedProj(ItemProject *pProj)
{
    if(sm_pInstance->m_pCurSelectedProj == pProj)
        return;
        
    sm_pInstance->m_pCurSelectedProj = pProj;
    if(sm_pInstance->m_pCurSelectedProj)
    {
        sm_pInstance->ui->dockWidgetAtlas->setWidget(sm_pInstance->m_pCurSelectedProj->GetAtlasManager());
        sm_pInstance->m_pCurSelectedProj->GetAtlasManager()->show();
    }
    else
    {
        sm_pInstance->ui->dockWidgetAtlas->setWidget(NULL);
    }
}

void MainWindow::on_actionNewProject_triggered()
{
    DlgNewProject *pDlg = new DlgNewProject(m_sDefaultProjLocation, this);
    if(pDlg->exec())
    {
        QString sProjDirPath = pDlg->GetProjPath();

        ui->explorer->AddItem(ITEM_Project, sProjDirPath, true);
    }
    delete pDlg;
}

void MainWindow::on_actionOpenProject_triggered()
{
    DlgOpenProject *pDlg = new DlgOpenProject(this);
    if(pDlg->exec() == QDialog::Accepted)
    {
        ui->explorer->AddItem(ITEM_Project, pDlg->SelectedDir(), true);
    }
    delete pDlg;
}

void MainWindow::on_actionCloseProject_triggered()
{
    ui->explorer->RemoveItem(ui->explorer->GetCurProjSelected());
}

void MainWindow::on_actionNewAudio_triggered()
{
    NewItem(ITEM_Audio);
}

void MainWindow::on_actionNewParticle_triggered()
{
    NewItem(ITEM_Particles);
}

void MainWindow::on_actionNewSprite_triggered()
{
    NewItem(ITEM_Sprite);
}

void MainWindow::on_actionNewFont_triggered()
{
    NewItem(ITEM_Font);
}

void MainWindow::NewItem(eItemType eItem)
{
    QString sProjPath = ui->explorer->GetCurProjSelected()->GetPath();
    QString sSpritePath = sProjPath % HYGUIPATH_RelDataDir % HyGlobal::ItemName(eItem) % "/";
            
    DlgNewItem *pDlg = new DlgNewItem(sSpritePath, eItem, this);
    if(pDlg->exec())
    {
        QString sPath = QDir::cleanPath(sSpritePath % pDlg->GetPrefix() % "/" % pDlg->GetName() % HyGlobal::ItemExt(eItem));
        ui->explorer->AddItem(eItem, sPath, true);
    }
    delete pDlg;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    SaveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::SaveSettings()
{
    m_Settings.beginGroup("MainWindow");
    {
        m_Settings.setValue("geometry", saveGeometry());
        m_Settings.setValue("windowState", saveState());
    }
    m_Settings.endGroup();

    m_Settings.beginGroup("OpenData");
    {
        m_Settings.setValue("openProjs", QVariant(ui->explorer->GetOpenProjectPaths()));
    }
    m_Settings.endGroup();
}

void MainWindow::LoadSettings()
{
    // Restore workspace
    m_Settings.beginGroup("MainWindow");
    {
        restoreGeometry(m_Settings.value("geometry").toByteArray());
        restoreState(m_Settings.value("windowState").toByteArray());
    }
    m_Settings.endGroup();
    
    ui->actionViewAtlasManager->setChecked(!ui->dockWidgetAtlas->isHidden());
    ui->actionViewExplorer->setChecked(!ui->dockWidgetExplorer->isHidden());
    ui->actionViewOutputLog->setChecked(!ui->dockWidgetOutputLog->isHidden());
    
    m_Settings.beginGroup("OpenData");
    {
        QStringList sListOpenProjs = m_Settings.value("openProjs").toStringList();
        foreach(QString sProjPath, sListOpenProjs)
        {
            ui->explorer->AddItem(ITEM_Project, sProjPath, false);
        }
    }
    m_Settings.endGroup();

    // Restore opened items/tabs
    ui->renderer->ClearItems();
    //ui->renderer->OpenItem(ITEM_Sprite, QString("Test"));
    //ui->renderer->OpenItem(ITEM_Sprite, QString("Test2"));

    // Grab additional data
    m_sDefaultProjLocation = m_Settings.value("defaultProjLoc").toString();

    if(m_sDefaultProjLocation.isEmpty())
    {
        QDir defaultProjLocDir = QDir::current();
        defaultProjLocDir.makeAbsolute();
#ifdef QT_DEBUG
        defaultProjLocDir.cd("../../Projects/");
#else
        defaultProjLocDir.cd("../Projects/");
#endif

        m_sDefaultProjLocation = defaultProjLocDir.path();
    }
}

void MainWindow::UpdateActions()
{
    //ui->explorer->GetCurProjPath(
}

void MainWindow::newConnection()
{
    QTcpSocket *pSocket = m_pTcpServer->nextPendingConnection();
    
    pSocket->write("Hello client\r\n");
    pSocket->flush();
    
    pSocket->waitForBytesWritten(3000);
    
    pSocket->close();
    //m_TcpClients
}


void MainWindow::on_actionViewExplorer_triggered()
{
    ui->dockWidgetExplorer->setHidden(!ui->dockWidgetExplorer->isHidden());
}


void MainWindow::on_actionViewAtlasManager_triggered()
{
    ui->dockWidgetAtlas->setHidden(!ui->dockWidgetAtlas->isHidden());
}

void MainWindow::on_actionViewOutputLog_triggered()
{
    ui->dockWidgetOutputLog->setHidden(!ui->dockWidgetOutputLog->isHidden());
}
