#include "Text.h"


Text::Text(HarmonyInit &initStruct) :	IHyApplication(initStruct),
										m_Text(nullptr, "NewFont")
{
}

Text::~Text()
{
}

/*virtual*/ bool Text::Initialize()
{
	m_pCamera = Window().CreateCamera2d();

	std::vector<glm::vec2> vertexList;
	vertexList.push_back(glm::vec2(-1000.0f, 0.0f));
	vertexList.push_back(glm::vec2(+1000.0f, 0.0f));
	m_Origin.SetAsLineChain(vertexList);

	m_Text.TextSet("Onge twjo thrqee");
	m_Text.SetAsScaleBox(400.0f, 100.0f, true);
	m_Text.TextSetAlignment(HYALIGN_Center);

	m_TextBox.SetAsQuad(m_Text.TextGetBox().x, m_Text.TextGetBox().y, true);
	m_Text.AddChild(m_TextBox);

	m_Origin.Load();
	m_Text.Load();

	return true;
}

/*virtual*/ bool Text::Update()
{
	return true;
}

/*virtual*/ void Text::Shutdown()
{
}
