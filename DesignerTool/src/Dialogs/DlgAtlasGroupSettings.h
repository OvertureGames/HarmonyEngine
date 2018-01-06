/**************************************************************************
 *	DlgAtlasGroupSettings.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGATLASGROUPSETTINGS_H
#define DLGATLASGROUPSETTINGS_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class DlgAtlasGroupSettings;
}

class DlgAtlasGroupSettings : public QDialog
{
	Q_OBJECT
	
	QJsonObject m_InitialPackerSettingsObj;
	bool        m_bAtlasGrpHasImages;

public:
	explicit DlgAtlasGroupSettings(bool bAtlasGrpHasImages, QJsonObject packerSettingsObj, QWidget *parent = 0);
	~DlgAtlasGroupSettings();
	
	static QJsonObject GenerateDefaultSettingsObj();
	
	bool IsSettingsDirty();
	bool IsNameChanged();
	
	void ApplyCurrentSettingsToObj(QJsonObject &settingsObjOut);

private Q_SLOTS:
	void on_btnTexSize128_clicked();
	void on_btnTexSize256_clicked();
	void on_btnTexSize512_clicked();
	void on_btnTexSize1024_clicked();
	void on_btnTexSize2048_clicked();
	void on_btnTexSize4096_clicked();
	void on_btnTexSize8192_clicked();
	void on_btnTexSize16384_clicked();
	
	virtual void done(int r);

private:
	Ui::DlgAtlasGroupSettings *ui;
};

#endif // DLGATLASGROUPSETTINGS_H
