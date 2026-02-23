#include "AnimationDrawer.h"
#include "Animation.h"
#include "Camera.h"
#include "GraphHandle.h"
#include "Define.h"
#include "DxLib.h"


AnimationDrawer::AnimationDrawer(const Animation* animation) {
	m_animation = animation;
}

void AnimationDrawer::drawAnimation(const Camera* camera) const {
	int x = m_animation->getX();
	int y = m_animation->getY();
	const GraphHandle* handle = m_animation->getHandle();
	double ex = handle->getEx();

	// ƒJƒƒ‰‚Å•â³‚µ‚Ä•`‰æ
	if (camera != nullptr) {
		camera->setCamera(&x, &y, &ex);
	}
	handle->draw(x, y, ex);
}

void AnimationDrawer::adjustSizeAndDraw() {
	int wide = 0, height = 0;
	int handle = m_animation->getHandle()->getHandle();
	GetGraphSize(handle, &wide, &height);
	double ex = max((double)GAME_WIDE / (double)wide, (double)GAME_HEIGHT / (double)height);
	m_animation->getHandle()->draw(GAME_WIDE / 2, GAME_HEIGHT / 2, ex);
}
