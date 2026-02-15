#ifndef OBJECT_DRAWER_H_INCLUDED
#define OBJECT_DRAWER_H_INCLUDED

class Object;
class Camera;

class ObjectDrawer {
private:

	// •`‰æ‘ÎÛ
	const Object* m_object;

	// 1920‚ğŠî€‚Æ‚µ‚½GAME_WIDE‚Ì”{—¦
	double m_exX;
	// 1080‚ğŠî€‚Æ‚µ‚½GAME_HEIGHT‚Ì”{—¦
	double m_exY;

	// ƒtƒHƒ“ƒg
	int m_font;
	int m_fontSize;

public:
	ObjectDrawer(const Object* object);

	~ObjectDrawer();

	inline void setObject(const Object* object) { m_object = object; }

	void drawObject(const Camera* const camera);
};

#endif