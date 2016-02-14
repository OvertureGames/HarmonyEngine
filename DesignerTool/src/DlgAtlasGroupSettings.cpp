#include "DlgAtlasGroupSettings.h"
#include "ui_DlgAtlasGroupSettings.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

#include "HyGlobal.h"

DlgAtlasGroupSettings::DlgAtlasGroupSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgAtlasGroupSettings)
{
    ui->setupUi(this);
    DataFromWidgets();
}

DlgAtlasGroupSettings::~DlgAtlasGroupSettings()
{
    delete ui;
}

void DlgAtlasGroupSettings::DataFromWidgets()
{
    m_iTextureWidth = ui->sbTextureWidth->value();
    m_iTextureHeight = ui->sbTextureHeight->value();
    m_iHeuristicIndex = ui->cmbHeuristic->currentIndex();
    m_iSortOrderIndex = ui->cmbSortOrder->currentIndex();
    m_iFrameMarginTop = ui->sbFrameMarginTop->value();
    m_iFrameMarginLeft = ui->sbFrameMarginLeft->value();
    m_iFrameMarginRight = ui->sbFrameMarginRight->value();
    m_iFrameMarginBottom = ui->sbFrameMarginBottom->value();
    m_iExtrude = ui->extrude->value();
    m_bMerge = ui->chkMerge->isChecked();
    m_bAutoSize = ui->chkAutosize->isChecked();
    m_bSquare = ui->chkSquare->isChecked();
    m_iFillRate = ui->minFillRate->value();
    m_iRotationStrategyIndex = ui->cmbRotationStrategy->currentIndex();
}

void DlgAtlasGroupSettings::DataToWidgets()
{
    ui->cmbSortOrder->setCurrentIndex(m_iSortOrderIndex);
    ui->sbFrameMarginTop->setValue(m_iFrameMarginTop);
    ui->sbFrameMarginLeft->setValue(m_iFrameMarginLeft);
    ui->sbFrameMarginRight->setValue(m_iFrameMarginRight);
    ui->sbFrameMarginBottom->setValue(m_iFrameMarginBottom);
    ui->extrude->setValue(m_iExtrude);
    ui->chkMerge->setChecked(m_bMerge);
    ui->chkSquare->setChecked(m_bSquare);
    ui->chkAutosize->setChecked(m_bAutoSize);
    ui->minFillRate->setValue(m_iFillRate);
    ui->cmbRotationStrategy->setCurrentIndex(m_iRotationStrategyIndex);

    ui->sbTextureWidth->setValue(m_iTextureWidth);
    ui->sbTextureHeight->setValue(m_iTextureHeight);
    ui->cmbHeuristic->setCurrentIndex(m_iHeuristicIndex);
}

int DlgAtlasGroupSettings::TextureWidth()
{
    return m_iTextureWidth;//ui->sbTextureWidth->value();
}

int DlgAtlasGroupSettings::TextureHeight()
{
    return m_iTextureHeight;//ui->sbTextureHeight->value();
}

int DlgAtlasGroupSettings::GetHeuristic()
{
    return m_iHeuristicIndex;//ui->cmbHeuristic->currentIndex();
}

void DlgAtlasGroupSettings::SetPackerSettings(ImagePacker *pPacker)
{
    pPacker->sortOrder = m_iSortOrderIndex;//ui->cmbSortOrder->currentIndex();
    pPacker->border.t = m_iFrameMarginTop;//ui->sbFrameMarginTop->value();
    pPacker->border.l = m_iFrameMarginLeft;//ui->sbFrameMarginLeft->value();
    pPacker->border.r = m_iFrameMarginRight;//ui->sbFrameMarginRight->value();
    pPacker->border.b = m_iFrameMarginBottom;//ui->sbFrameMarginBottom->value();
    pPacker->extrude = m_iExtrude;//ui->extrude->value();
    pPacker->merge = m_bMerge;//ui->chkMerge->isChecked();
    pPacker->square = m_bSquare;//ui->chkSquare->isChecked();
    pPacker->autosize = m_bAutoSize;//ui->chkAutosize->isChecked();
    pPacker->minFillRate = m_iFillRate;//ui->minFillRate->value();
    pPacker->mergeBF = false;
    pPacker->rotate = m_iRotationStrategyIndex;//ui->cmbRotationStrategy->currentIndex();
}

QJsonObject DlgAtlasGroupSettings::GetSettings()
{
    QJsonObject settings;
    settings.insert("cmbSortOrder", QJsonValue(m_iSortOrderIndex/*ui->cmbSortOrder->currentIndex()*/));
    settings.insert("sbFrameMarginTop", QJsonValue(m_iFrameMarginTop/*ui->sbFrameMarginTop->value()*/));
    settings.insert("sbFrameMarginLeft", QJsonValue(m_iFrameMarginLeft/*ui->sbFrameMarginLeft->value()*/));
    settings.insert("sbFrameMarginRight", QJsonValue(m_iFrameMarginRight/*ui->sbFrameMarginRight->value()*/));
    settings.insert("sbFrameMarginBottom", QJsonValue(m_iFrameMarginBottom/*ui->sbFrameMarginBottom->value()*/));
    settings.insert("extrude", QJsonValue(m_iExtrude/*ui->extrude->value()*/));
    settings.insert("chkMerge", QJsonValue(m_bMerge/*ui->chkMerge->isChecked()*/));
    settings.insert("chkSquare", QJsonValue(m_bSquare/*ui->chkSquare->isChecked()*/));
    settings.insert("chkAutosize", QJsonValue(m_bAutoSize/*ui->chkAutosize->isChecked()*/));
    settings.insert("minFillRate", QJsonValue(m_iFillRate/*ui->minFillRate->value()*/));
    settings.insert("cmbRotationStrategy", QJsonValue(m_iRotationStrategyIndex/*ui->cmbRotationStrategy->currentIndex()*/));

    settings.insert("sbTextureWidth", QJsonValue(m_iTextureWidth/*ui->sbTextureWidth->value()*/));
    settings.insert("sbTextureHeight", QJsonValue(m_iTextureHeight/*ui->sbTextureHeight->value()*/));
    settings.insert("cmbHeuristic", QJsonValue(m_iHeuristicIndex/*ui->cmbHeuristic->currentIndex()*/));

    return settings;
}

void DlgAtlasGroupSettings::LoadSettings(QJsonObject settings)
{
    m_iSortOrderIndex = JSONOBJ_TOINT(settings, "cmbSortOrder");
    m_iFrameMarginTop = JSONOBJ_TOINT(settings, "sbFrameMarginTop");
    m_iFrameMarginLeft = JSONOBJ_TOINT(settings, "sbFrameMarginLeft");
    m_iFrameMarginRight = JSONOBJ_TOINT(settings, "sbFrameMarginRight");
    m_iFrameMarginBottom = JSONOBJ_TOINT(settings, "sbFrameMarginBottom");
    m_iExtrude = JSONOBJ_TOINT(settings, "extrude");
    m_bMerge = settings["chkMerge"].toBool();
    m_bAutoSize = settings["chkAutosize"].toBool();
    m_bSquare = settings["chkSquare"].toBool();
    m_iFillRate = JSONOBJ_TOINT(settings, "minFillRate");
    m_iRotationStrategyIndex = JSONOBJ_TOINT(settings, "cmbRotationStrategy");

    m_iTextureWidth = JSONOBJ_TOINT(settings, "sbTextureWidth");
    m_iTextureHeight = JSONOBJ_TOINT(settings, "sbTextureHeight");
    m_iHeuristicIndex = JSONOBJ_TOINT(settings, "cmbHeuristic");
}

void DlgAtlasGroupSettings::on_cmbSortOrder_currentIndexChanged(int index)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_cmbRotationStrategy_currentIndexChanged(int index)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_cmbHeuristic_currentIndexChanged(int index)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_alphaThreshold_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_extrude_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_minFillRate_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_chkMerge_stateChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_chkAutosize_stateChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_chkSquare_stateChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_sbFrameMarginTop_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_sbFrameMarginLeft_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_sbFrameMarginRight_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_sbFrameMarginBottom_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_sbTextureWidth_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_sbTextureHeight_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_btnMatchTextureWidthHeight_clicked()
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_btnTexSize256_clicked()
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_btnTexSize512_clicked()
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_btnTexSize1024_clicked()
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_btnTexSize2048_clicked()
{
    m_bSettingsDirty = true;
}

void DlgAtlasGroupSettings::on_buttonBox_accepted()
{
    DataFromWidgets();
}

void DlgAtlasGroupSettings::on_buttonBox_rejected()
{
    DataToWidgets();
}
