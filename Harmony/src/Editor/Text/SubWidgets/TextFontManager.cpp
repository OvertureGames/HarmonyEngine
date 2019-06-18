/**************************************************************************
*	TextFontManager.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "TextFontManager.h"
#include "TextModel.h"
#include "Project.h"

#define TEXTFONTMANAGER_AtlasSize 2048, 2048

TextLayerHandle TextFontManager::sm_hHandleCount = 0;

TextFontManager::TextFontManager(ProjectItem &itemRef, QJsonObject availGlyphsObj, QJsonArray fontArray) :
	m_FontArray(fontArray),
	m_GlyphsModel(itemRef, 0, 0, nullptr),
	m_pPreviewAtlas(nullptr)
{
	bool b09 = true;
	bool bAZ = true;
	bool baz = true;
	bool bSymbols = true;
	QString sAdditional = "";

	// If item's init value is defined, parse and initialize with it, otherwise make default empty font
	if(availGlyphsObj.empty() == false)
	{
		b09 = availGlyphsObj[TEXTPROP_09].toBool();
		bAZ = availGlyphsObj[TEXTPROP_AZ].toBool();
		baz = availGlyphsObj[TEXTPROP_az].toBool();
		bSymbols = availGlyphsObj["symbols"].toBool();
		sAdditional = availGlyphsObj["additional"].toString();
	}

	m_GlyphsModel.AppendCategory("Uses Glyphs", HyGlobal::ItemColor(ITEM_Prefix));
	m_GlyphsModel.AppendProperty("Uses Glyphs", TEXTPROP_09, PROPERTIESTYPE_bool, QVariant(b09 ? Qt::Checked : Qt::Unchecked), "Include numerical glyphs 0-9");
	m_GlyphsModel.AppendProperty("Uses Glyphs", TEXTPROP_AZ, PROPERTIESTYPE_bool, QVariant(bAZ ? Qt::Checked : Qt::Unchecked), "Include capital letter glyphs A-Z");
	m_GlyphsModel.AppendProperty("Uses Glyphs", TEXTPROP_az, PROPERTIESTYPE_bool, QVariant(baz ? Qt::Checked : Qt::Unchecked), "Include lowercase letter glyphs a-z");
	m_GlyphsModel.AppendProperty("Uses Glyphs", TEXTPROP_Symbols, PROPERTIESTYPE_bool, QVariant(bSymbols ? Qt::Checked : Qt::Unchecked), "Include common punctuation and symbol glyphs");
	m_GlyphsModel.AppendProperty("Uses Glyphs", TEXTPROP_AdditionalSyms, PROPERTIESTYPE_LineEdit, QVariant(sAdditional), "Include specified glyphs");
	m_GlyphsModel.AppendCategory("Atlas Info");
	m_GlyphsModel.AppendProperty("Atlas Info", TEXTPROP_Dimensions, PROPERTIESTYPE_ivec2, QPoint(0, 0), "The required portion size needed to fit on an atlas", true);
	m_GlyphsModel.AppendProperty("Atlas Info", TEXTPROP_UsedPercent, PROPERTIESTYPE_double, 0.0, "Percentage of the maximum size dimensions used", true);
}

TextFontManager::~TextFontManager()
{
	for(auto iter = m_LayerMap.begin(); iter != m_LayerMap.end(); ++iter)
		delete iter.value();

	for(int i = 0; i < m_PreviewFontList.size(); ++i)
		delete m_PreviewFontList[i];

	texture_atlas_delete(m_pPreviewAtlas);
}

PropertiesTreeModel *TextFontManager::GetGlyphsModel()
{
	return &m_GlyphsModel;
}

const PropertiesTreeModel *TextFontManager::GetGlyphsModel() const
{
	return &m_GlyphsModel;
}

QList<TextLayerHandle> TextFontManager::RegisterLayers(QJsonArray layerArray)
{
	QList<TextLayerHandle> retLayerHandleList;
	for(int i = 0; i < layerArray.size(); ++i)
	{
		QJsonObject layerObj = layerArray[i].toObject();

		glm::vec4 vBotColor;
		vBotColor.r = layerObj["botR"].toDouble();
		vBotColor.g = layerObj["botG"].toDouble();
		vBotColor.b = layerObj["botB"].toDouble();
		glm::vec4 vTopColor;
		vTopColor.r = layerObj["topR"].toDouble();
		vTopColor.g = layerObj["topG"].toDouble();
		vTopColor.b = layerObj["topB"].toDouble();

		uint32 uiFontIndex = layerObj["typefaceIndex"].toInt();

		TextLayerHandle hNewLayer = ++sm_hHandleCount;
		m_LayerMap[hNewLayer] = new Layer(hNewLayer, uiFontIndex, vBotColor, vTopColor);

		retLayerHandleList.append(hNewLayer);
	}

	return retLayerHandleList;
}

QJsonArray TextFontManager::GetFontArray() const
{
	return m_FontArray;
}

rendermode_t TextFontManager::GetRenderMode(uint uiFontIndex)
{
	if(uiFontIndex >= m_FontArray.size())
	{
		HyGuiLog("TextFontManager::GetRenderMode passed an index outside 'm_FontArray's bounds", LOGTYPE_Error);
		return RENDER_NORMAL;
	}

	QJsonObject fontObj = m_FontArray.at[uiFontIndex].toObject();
	return static_cast<rendermode_t>(fontObj["mode"].toInt());
}

float TextFontManager::GetOutlineThickness(uint uiFontIndex)
{
	if(uiFontIndex >= m_FontArray.size())
	{
		HyGuiLog("TextFontManager::GetOutlineThickness passed an index outside 'm_FontArray's bounds", LOGTYPE_Error);
		return RENDER_NORMAL;
	}

	QJsonObject fontObj = m_FontArray.at[uiFontIndex].toObject();
	return static_cast<float>(fontObj["outlineThickness"].toDouble());
}

TextLayerHandle TextFontManager::AddNewLayer(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize)
{
	// If font already exists, don't generate preview
	for(int i = 0; i < m_FontArray.size(); ++i)
	{
		QJsonObject fontObj = m_FontArray[i].toObject();
		
		if(fontObj["font"].toString().compare(sFontName, Qt::CaseInsensitive) == 0 &&
			fontObj["mode"].toInt() == eRenderMode &&
			HyCompareFloat(fontObj["outlineThickness"].toDouble(), static_cast<double>(fOutlineThickness)) &&
			HyCompareFloat(fontObj["size"].toDouble(), static_cast<double>(fSize)))
		{
			TextLayerHandle hNewLayer = ++sm_hHandleCount;
			m_LayerMap[hNewLayer] = new Layer(hNewLayer, i, glm::vec3(0.0f), glm::vec3(0.0f));

			return hNewLayer;
		}
	}

	PrepPreview();
	{
		QList<QStandardItem *> foundFontList = m_GlyphsModel.GetOwner().GetProject().GetFontListModel()->findItems(sFontName);
		if(foundFontList.size() == 1)
		{
			PreviewFont *pNewPreviewFont = new PreviewFont(m_pPreviewAtlas,
														   GetAvailableTypefaceGlyphs(),
														   foundFontList[0]->data().toString(),
														   fSize,
														   fOutlineThickness,
														   eRenderMode);
			if(pNewPreviewFont->GetMissedGlyphs() != 0)
				HyGuiLog("PrepPreview could not create all preview fonts", LOGTYPE_Error);

			m_PreviewFontList.append(pNewPreviewFont);
		}
	}
	RegenFontArray();
}

void TextFontManager::PrepPreview()
{
	if(m_pPreviewAtlas == nullptr)
	{
		m_pPreviewAtlas = texture_atlas_new(TEXTFONTMANAGER_AtlasSize, 1);

		QString sAvailGlyphs = GetAvailableTypefaceGlyphs();
		QStandardItemModel *pFontListModel = m_GlyphsModel.GetOwner().GetProject().GetFontListModel();

		if(m_PreviewFontList.isEmpty() == false)
		{
			HyGuiLog("PrepPreview has uncleared fonts in 'm_PreviewFontList'", LOGTYPE_Error);
			m_PreviewFontList.clear();
		}
		for(int i = 0; i < m_FontArray.size(); ++i)
		{
			QJsonObject fontObj = m_FontArray[i].toObject();

			QList<QStandardItem *> foundFontList = pFontListModel->findItems(fontObj["font"].toString());
			if(foundFontList.size() == 1)
			{
				PreviewFont *pNewPreviewFont = new PreviewFont(m_pPreviewAtlas,
																sAvailGlyphs,
																foundFontList[0]->data().toString(),
																fontObj["size"].toDouble(),
																fontObj["outlineThickness"].toDouble(),
																static_cast<rendermode_t>(fontObj["mode"].toInt()));
				if(pNewPreviewFont->GetMissedGlyphs() != 0)
					HyGuiLog("PrepPreview could not create all preview fonts", LOGTYPE_Error);

				m_PreviewFontList.append(pNewPreviewFont);
			}
		}
	}
}

void TextFontManager::RegenFontArray()
{
	QString sAvailGlyphs = GetAvailableTypefaceGlyphs();

	QJsonArray fontArray;
	for(int i = 0; i < m_PreviewFontList.count(); ++i)
	{
		texture_font_t *pFtglFont = m_PreviewFontList[i]->GetTextureFont();

		QJsonObject stageObj;
		QFileInfo fontFileInfo(pFtglFont->filename);

		stageObj.insert("font", fontFileInfo.fileName());
		stageObj.insert("size", pFtglFont->size);
		stageObj.insert("mode", pFtglFont->rendermode);
		stageObj.insert("outlineThickness", pFtglFont->outline_thickness);

		QJsonArray glyphsArray;
		for(int j = 0; j < sAvailGlyphs.count(); ++j)
		{
			// NOTE: Assumes LITTLE ENDIAN
			QString sSingleChar = sAvailGlyphs[j];
			texture_glyph_t *pGlyph = texture_font_get_glyph(pFtglFont, sSingleChar.toUtf8().data());

			QJsonObject glyphInfoObj;
			if(pGlyph == nullptr)
			{
				HyGuiLog("Could not find glyph: '" % sSingleChar % "'\nPlace a breakpoint and walk into texture_font_get_glyph() below before continuing", LOGTYPE_Error);
				pGlyph = texture_font_get_glyph(pFtglFont, sSingleChar.toUtf8().data());
			}
			else
			{
				glyphInfoObj.insert("code", QJsonValue(static_cast<qint64>(pGlyph->codepoint)));
				glyphInfoObj.insert("advance_x", pGlyph->advance_x);
				glyphInfoObj.insert("advance_y", pGlyph->advance_y);
				glyphInfoObj.insert("width", static_cast<int>(pGlyph->width));
				glyphInfoObj.insert("height", static_cast<int>(pGlyph->height));
				glyphInfoObj.insert("offset_x", pGlyph->offset_x);
				glyphInfoObj.insert("offset_y", pGlyph->offset_y);
				glyphInfoObj.insert("left", pGlyph->s0);
				glyphInfoObj.insert("top", pGlyph->t0);
				glyphInfoObj.insert("right", pGlyph->s1);
				glyphInfoObj.insert("bottom", pGlyph->t1);
			}

#if 0		// Not using kerning data
			QJsonObject kerningInfoObj;
			for(int k = 0; k < sAvailableTypefaceGlyphs.count(); ++k)
			{
				char cTmpChar = sAvailableTypefaceGlyphs.toStdString().c_str()[k];
				float fKerningAmt = texture_glyph_get_kerning(pGlyph, &cTmpChar);

				if(fKerningAmt != 0.0f)
					kerningInfoObj.insert(QString(sAvailableTypefaceGlyphs[k]), fKerningAmt);
			}
			glyphInfoObj.insert("kerning", kerningInfoObj);
#endif
			glyphsArray.append(glyphInfoObj);
		}
		stageObj.insert("glyphs", glyphsArray);

		fontArray.append(QJsonValue(stageObj));
	}

	m_FontArray = fontArray;
}

QString TextFontManager::GetAvailableTypefaceGlyphs() const
{
	// Assemble glyph set
	QString sAvailableTypefaceGlyphs;
	sAvailableTypefaceGlyphs.clear();
	sAvailableTypefaceGlyphs += ' ';

	QVariant propValue;

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", TEXTPROP_09);
	if(propValue.toInt() == Qt::Checked)
		sAvailableTypefaceGlyphs += "0123456789";

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", TEXTPROP_az);
	if(propValue.toInt() == Qt::Checked)
		sAvailableTypefaceGlyphs += "abcdefghijklmnopqrstuvwxyz";

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", TEXTPROP_AZ);
	if(propValue.toInt() == Qt::Checked)
		sAvailableTypefaceGlyphs += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", TEXTPROP_Symbols);
	if(propValue.toInt() == Qt::Checked)
		sAvailableTypefaceGlyphs += TEXTPROP_Symbols;

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", TEXTPROP_AdditionalSyms);
	sAvailableTypefaceGlyphs += propValue.toString(); // May contain duplicates as stated in freetype-gl documentation

	return sAvailableTypefaceGlyphs;
}
