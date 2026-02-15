#include "ObjectDrawer.h"
#include "Camera.h"
#include "Object.h"
#include "GraphHandle.h"
#include "Define.h"
#include "DxLib.h"


ObjectDrawer::ObjectDrawer(const Object* object) {
	m_object = object;
	getGameEx(m_exX, m_exY);
	m_fontSize = (int)(50 * m_exX);
	m_font = CreateFontToHandle(nullptr, m_fontSize, 3);
}

ObjectDrawer::~ObjectDrawer() {
	DeleteFontToHandle(m_font);
}

void ObjectDrawer::drawObject(const Camera* const camera) {
	// À•W‚ÆŠg‘å—¦Žæ“¾
	int x1, y1, x2, y2;
	double ex = 1.0;
	x1 = m_object->getX1();
	y1 = m_object->getY1();
	x2 = m_object->getX2();
	y2 = m_object->getY2();

	// ‰æ‘œ‚ª‚È‚¢‚È‚ç}Œ`•`‰æA‚ ‚é‚È‚ç•’Ê‚É‰æ‘œ•`‰æ
	const GraphHandle* graphHandle = m_object->getHandle();
	if (graphHandle == nullptr) {
		// ƒJƒƒ‰‚Å’²®
		camera->setCamera(&x1, &y1, &ex);
		camera->setCamera(&x2, &y2, &ex);
		// •`‰æ
		m_object->drawObject(x1, y1, x2, y2);
	}
	else {
		// •`‰æ
		if (m_object->extendGraph()) {
			if (m_object->lineUpType()) {
				graphHandle->lineUpDraw(x1, y1, x2, y2, camera);
			}
			else {
				camera->setCamera(&x1, &y1, &ex);
				camera->setCamera(&x2, &y2, &ex);
				graphHandle->extendDraw(x1, y1, x2, y2);
			}
		}
		else {
			camera->setCamera(&x1, &y1, &ex);
			camera->setCamera(&x2, &y2, &ex);
			graphHandle->draw((x1 + x2) / 2, (y1 + y2) / 2, camera->getEx() * graphHandle->getEx());
		}
	}
	if (m_object->getTextDisp()) {
		int halfStrSize = m_fontSize * (int)(m_object->getText().size()) / 4;
		int centerX = (x1 + x2) / 2;
		DrawBox(centerX - halfStrSize, y1 - m_fontSize, centerX + halfStrSize, y1, WHITE, TRUE);
		DrawStringToHandle(centerX - halfStrSize, y1 - m_fontSize, m_object->getText().c_str(), BLACK, m_font);
	}
}