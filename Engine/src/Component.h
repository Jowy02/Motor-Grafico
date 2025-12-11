class GameObject;

class Component
{
public:
	enum class Type : unsigned char
	{
		TRANSFORM,
		MESH,
		MATERIAL
	};

	Component(Type t, const GameObject* go) : type(t), owner(go) {}
	virtual ~Component() {};
	virtual void Update(float dt) = 0;
	Type GetType() const { return type; }
protected:
	const GameObject* owner;
private:
	Type type;
};

