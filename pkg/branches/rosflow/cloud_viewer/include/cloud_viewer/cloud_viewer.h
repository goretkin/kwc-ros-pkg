#ifndef CLOUD_VIEWER_H
#define CLOUD_VIEWER_H

#include <vector>

class CloudViewerPoint
{
public:
	float x, y, z;
	uint8_t r, g, b;
	CloudViewerPoint(float x, float y, float z, uint8_t r, uint8_t g, uint8_t b) :
		x(x), y(y), z(z), r(r), g(g), b(b) { }
};

class CloudViewer
{
public:
	CloudViewer();
	~CloudViewer();

	void clear_cloud();
        void add_point(CloudViewerPoint p);
        void add_point(float x, float y, float z, uint8_t r, uint8_t g, uint8_t b);
	void set_opengl_params(unsigned width, unsigned height);
	void render();
	void mouse_button(int x, int y, int button, bool is_down);
	void mouse_motion(int x, int y, int dx, int dy);
	void keypress(char c);

private:
	std::vector<CloudViewerPoint> points;
	float cam_x, cam_y, cam_z, cam_rho, cam_ele, cam_azi;
	float look_tgt_x, look_tgt_y, look_tgt_z;
	bool left_button_down, right_button_down;
  bool hide_axes;
};

#endif
