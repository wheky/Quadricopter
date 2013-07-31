#ifndef ARROW_H_
# define ARROW_H_

#include <GL/glew.h>
#include "glfw.h"

class arrow {
    public:
	arrow(float = 0 , float = 0 , float = 0);
	void setRx(float r);
	void setRy(float r);
	void setRz(float r);
	void addRx(float r);
	void addRy(float r);
	void addRz(float r);
	void reset();
	void draw();
    private:
	float _rx;
	float _ry;
	float _rz;
	GLuint _v;
};

#endif /* !ARROW_H_ */
