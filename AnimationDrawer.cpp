#include "AnimationDrawer.h"
#include "Animation.h"
#include "Camera.h"
#include "GraphHandle.h"


AnimationDrawer::AnimationDrawer(const Animation* animation) {
	m_animation = animation;
}

void AnimationDrawer::drawAnimation(const Camera* camera) {
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