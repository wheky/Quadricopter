#ifndef ENV_H_
# define ENV_H_

#include <string>
#include <iostream>
#include <GL/glew.h>
#include "glfw.h"

#include "arrow.hh"

class env {
    public:
	env();
	~env();
	void run();
	void update();
    private:
	env(const env&);
	env& operator=(const env&);
	bool _alive;
	arrow* obj;
};



#endif /* !ENV_H_ */
