#include "env.hh"

env::env() : _alive(true) {
    if (glfwInit() == GL_FALSE) {
	std::cerr << "something wrong happened when I tried to initialize openGL context =/" << std::endl;
    }
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    if (glfwOpenWindow(1270, 1024, 8, 8, 8, 8, 24, 8, GLFW_WINDOW) == GL_FALSE) {
	std::cerr << "something wrong append when a tried to open window =S" << std::endl;
    }
    glfwSetWindowTitle("giro tester");
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    int glfwGetNumberOfProcessors( void );
    std::cout << "render device : " << renderer << std::endl;
    std::cout << "openGL version : " << version << std::endl;

    obj = new arrow();
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glLightfv( GL_LIGHT0, GL_AMBIENT, ({ (float[4]){1.0f,1.0f,1.0f,1.0f}; }));
    glLightfv( GL_LIGHT0, GL_POSITION, ({ (float[4]){0.0f, 2.f, 3.f, 0.0f}; }));
    glLightfv( GL_LIGHT0, GL_DIFFUSE,({ (float[4]){1.0f,1.0f,1.0f,1.0f}; }));
    glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.01f );
}

env::~env () {
    delete obj;
}

void env::run() {
    float Gx = 0;
    float Gy = 0;
    float Gz = 0;

    float Ax = 0;
    float Ay = 0;
    float Az = 0;
    float Aa = 0;

    std::string Gtype;
    std::string Atype;

    while (_alive) {
	update();

	std::cin >> Atype >> Ax >> Ay >> Az >> Aa >> Gtype >> Gx >> Gy >> Gz;
	if (Gtype == "ypr") {
	    obj->setRx(-Gz); // Axe x
	    obj->setRy(Gx); // Axe z
	    obj->setRz(Gy); // Axe y
	}
	obj->draw();
    }
}

void env::update() {
    glfwSwapBuffers();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1, 0.1, 0.1, 1.0f);
    glClearDepth(1.0f);
    _alive = glfwGetWindowParam(GLFW_OPENED);
    if (glfwGetKey(GLFW_KEY_ESC)) {
	_alive = false;
    }
    else if (glfwGetKey(GLFW_KEY_UP)) {
	obj->addRx(10);
    } else if (glfwGetKey(GLFW_KEY_DOWN)) {
	obj->addRx(-10);
    } else if (glfwGetKey(GLFW_KEY_RIGHT)) {
	obj->addRz(-10);
    } else if (glfwGetKey(GLFW_KEY_LEFT)) {
	obj->addRz(10);
    } else if (glfwGetKey(GLFW_KEY_SPACE)) {
	obj->addRy(10);
    } else if (glfwGetKey(GLFW_KEY_BACKSPACE)) {
	obj->addRy(-10);
    } else if (glfwGetKey(GLFW_KEY_ENTER)) {
	obj->reset();
    }
}
