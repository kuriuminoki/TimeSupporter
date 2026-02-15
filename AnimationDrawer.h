#ifndef ANIMATION_DRAWER_H_INCLUDED
#define ANIMATION_DRAWER_H_INCLUDED

class Animation;
class Camera;

class AnimationDrawer {
private:
	const Animation* m_animation;
public:
	AnimationDrawer(const Animation* animation);

	inline void setAnimation(const Animation* animation) { m_animation = animation; };

	void drawAnimation(const Camera* const camera = nullptr);
};

#endif